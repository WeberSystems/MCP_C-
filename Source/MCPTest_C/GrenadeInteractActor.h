#pragma once

#include "CoreMinimal.h"
#include "RifleInteractActor.h"
#include "GrenadeInteractActor.generated.h"

/** Ground grenade picked up with the right mouse button into hand_l. */
UCLASS()
class MCPTEST_C_API AGrenadeInteractActor : public ARifleInteractActor
{
	GENERATED_BODY()

public:
	AGrenadeInteractActor();
};
