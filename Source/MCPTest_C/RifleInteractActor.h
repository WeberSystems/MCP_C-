#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RifleInteractActor.generated.h"

class UStaticMeshComponent;

/** Place this actor on the ground. Only an explicit interaction consumes it. */
UCLASS()
class MCPTEST_C_API ARifleInteractActor : public AActor
{
	GENERATED_BODY()

public:
	ARifleInteractActor();

private:
	UPROPERTY(VisibleAnywhere, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMesh;
};
