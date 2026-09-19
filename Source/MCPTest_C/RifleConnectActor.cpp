#include "RifleConnectActor.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ARifleConnectActor::ARifleConnectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	AttachmentRoot = CreateDefaultSubobject<USceneComponent>(TEXT("AttachmentRoot"));
	SetRootComponent(AttachmentRoot);
	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(AttachmentRoot);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	WeaponMesh->SetStaticMesh(Cube.Object);
	WeaponMesh->SetRelativeScale3D(FVector(0.8f, 0.12f, 0.16f));
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);
}
