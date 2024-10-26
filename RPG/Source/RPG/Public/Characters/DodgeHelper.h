

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DodgeHelper.generated.h"

UCLASS()
class RPG_API ADodgeHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	ADodgeHelper();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
