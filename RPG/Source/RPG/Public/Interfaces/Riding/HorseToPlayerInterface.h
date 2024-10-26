

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HorseToPlayerInterface.generated.h"


UINTERFACE(MinimalAPI)
class UHorseToPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPG_API IHorseToPlayerInterface
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintNativeEvent)
	bool IsPlayerHorseCollision(bool isPlayer);
};
