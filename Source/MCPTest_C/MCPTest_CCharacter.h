// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "MCPTest_CCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class ARifleConnectActor;
class ARifleInteractActor;
class AGrenadeInteractActor;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/**
 *  A simple player-controllable third person character
 *  Implements a controllable orbiting camera
 */
UCLASS(abstract)
class AMCPTest_CCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
protected:

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	/** Mouse Look Input Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MouseLookAction;

public:

	/** Constructor */
	AMCPTest_CCharacter();	

protected:

	/** Initialize input action bindings */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Checked on mouse button presses, measured from the character in centimeters. */
	UPROPERTY(EditAnywhere, Category="Weapon", meta=(ClampMin="1.0", Units="cm"))
	float PickupDistance = 200.0f;

	/** Adjust the cube's position and rotation relative to hand_r. */
	UPROPERTY(EditAnywhere, Category="Weapon")
	FTransform WeaponHandOffset = FTransform::Identity;

	UPROPERTY(Transient, VisibleInstanceOnly, Category="Weapon")
	TObjectPtr<ARifleConnectActor> EquippedRifle;

	/** Position and rotation of the grenade relative to hand_l. */
	UPROPERTY(EditAnywhere, Category="Weapon")
	FTransform GrenadeHandOffset = FTransform::Identity;

	UPROPERTY(Transient, VisibleInstanceOnly, Category="Weapon")
	TObjectPtr<AGrenadeInteractActor> EquippedGrenade;

	ARifleInteractActor* FindPickupInView() const;
	void TryPickupRifle();
	void TryPickupGrenade();

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

public:

	/** Handles move inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoMove(float Right, float Forward);

	/** Handles look inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoLook(float Yaw, float Pitch);

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpStart();

	/** Handles jump pressed inputs from either controls or UI interfaces */
	UFUNCTION(BlueprintCallable, Category="Input")
	virtual void DoJumpEnd();

public:

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

