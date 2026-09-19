// Copyright Epic Games, Inc. All Rights Reserved.

#include "MCPTest_CCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "MCPTest_C.h"
#include "RifleInteractActor.h"
#include "GrenadeInteractActor.h"
#include "RifleConnectActor.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "InputCoreTypes.h"

AMCPTest_CCharacter::AMCPTest_CCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AMCPTest_CCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AMCPTest_CCharacter::TryPickupRifle);
	PlayerInputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AMCPTest_CCharacter::TryPickupGrenade);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMCPTest_CCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AMCPTest_CCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMCPTest_CCharacter::Look);
	}
	else
	{
		UE_LOG(LogMCPTest_C, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

ARifleInteractActor* AMCPTest_CCharacter::FindPickupInView() const
{
	if (!Controller || !GetWorld())
	{
		return nullptr;
	}

	// The center of the actual camera view must hit the ground weapon first.
	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
	const float TraceDistance = FVector::Distance(ViewLocation, GetActorLocation()) + PickupDistance;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(RiflePickup), false, this);
	if (IsValid(EquippedRifle))
	{
		QueryParams.AddIgnoredActor(EquippedRifle);
	}
	if (IsValid(EquippedGrenade))
	{
		QueryParams.AddIgnoredActor(EquippedGrenade);
	}
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation,
		ViewLocation + ViewRotation.Vector() * TraceDistance, ECC_Visibility, QueryParams))
	{
		return nullptr;
	}

	ARifleInteractActor* Pickup = Cast<ARifleInteractActor>(Hit.GetActor());
	if (!IsValid(Pickup) || FVector::DistSquared(GetActorLocation(), Hit.ImpactPoint) > FMath::Square(PickupDistance))
	{
		return nullptr;
	}

	// Also prevent reaching through a wall when the third-person camera can see around it.
	FHitResult ReachHit;
	if (GetWorld()->LineTraceSingleByChannel(ReachHit, GetActorLocation(), Hit.ImpactPoint,
		ECC_Visibility, QueryParams) && ReachHit.GetActor() != Pickup)
	{
		return nullptr;
	}
	return Pickup;
}

void AMCPTest_CCharacter::TryPickupRifle()
{
	if (IsValid(EquippedRifle))
	{
		return;
	}

	ARifleInteractActor* Pickup = FindPickupInView();
	// Grenades inherit from the rifle pickup, but must never enter the right hand.
	if (!IsValid(Pickup) || Pickup->IsA<AGrenadeInteractActor>())
	{
		return;
	}

	const FName HandBone(TEXT("hand_r"));
	if (!GetMesh() || GetMesh()->GetBoneIndex(HandBone) == INDEX_NONE)
	{
		UE_LOG(LogMCPTest_C, Warning, TEXT("Cannot equip rifle: character mesh has no hand_r bone."));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ARifleConnectActor* Weapon = GetWorld()->SpawnActor<ARifleConnectActor>(
		ARifleConnectActor::StaticClass(), GetMesh()->GetSocketTransform(HandBone), SpawnParams);
	if (!Weapon)
	{
		return;
	}

	if (!Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBone))
	{
		Weapon->Destroy();
		return;
	}
	Weapon->SetActorRelativeTransform(WeaponHandOffset);
	// Keep the pickup if spawning/attachment failed; roll back if it cannot be removed.
	if (!Pickup->Destroy())
	{
		Weapon->Destroy();
		return;
	}
	EquippedRifle = Weapon;
}

void AMCPTest_CCharacter::TryPickupGrenade()
{
	if (IsValid(EquippedGrenade))
	{
		return;
	}

	AGrenadeInteractActor* Pickup = Cast<AGrenadeInteractActor>(FindPickupInView());
	if (!IsValid(Pickup))
	{
		return;
	}

	const FName HandBone(TEXT("hand_l"));
	if (!GetMesh() || GetMesh()->GetBoneIndex(HandBone) == INDEX_NONE)
	{
		UE_LOG(LogMCPTest_C, Warning, TEXT("Cannot equip grenade: character mesh has no hand_l bone."));
		return;
	}

	// Keep the placed mesh and its scale when moving the grenade into the hand.
	const FVector PickupScale = Pickup->GetActorScale3D();
	if (!Pickup->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, HandBone))
	{
		return;
	}
	FTransform HandTransform = GrenadeHandOffset;
	HandTransform.SetScale3D(HandTransform.GetScale3D() * PickupScale);
	Pickup->SetActorRelativeTransform(HandTransform);
	Pickup->SetActorEnableCollision(false);
	Pickup->SetOwner(this);
	Pickup->SetInstigator(this);
	EquippedGrenade = Pickup;
}

void AMCPTest_CCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsValid(EquippedGrenade))
	{
		EquippedGrenade->Destroy();
		EquippedGrenade = nullptr;
	}
	if (IsValid(EquippedRifle))
	{
		EquippedRifle->Destroy();
		EquippedRifle = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void AMCPTest_CCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AMCPTest_CCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AMCPTest_CCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AMCPTest_CCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AMCPTest_CCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AMCPTest_CCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}
