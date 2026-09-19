#include "GrenadeInteractActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

AGrenadeInteractActor::AGrenadeInteractActor()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Sphere(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	WeaponMesh->SetStaticMesh(Sphere.Object);
	WeaponMesh->SetRelativeScale3D(FVector(0.12f, 0.12f, 0.16f));
}
