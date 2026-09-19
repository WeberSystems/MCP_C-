#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RifleConnectActor.generated.h"

class USceneComponent;
class UStaticMeshComponent;

/** Separate equipped weapon, spawned only after a successful pickup. */
UCLASS()
class MCPTEST_C_API ARifleConnectActor : public AActor
{
	GENERATED_BODY()

public:
	ARifleConnectActor();

private:
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<USceneComponent> AttachmentRoot;

	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
};
