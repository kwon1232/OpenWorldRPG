// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Interfaces/Riding/HorseToPlayerInterface.h"
#include "Components/SceneComponent.h"
#include "Components/Riding_System.h"
#include "Camera/CameraComponent.h"
#include "Item.h"
#include "Horse.generated.h"

class UBoxComponent;
class IHorseToPlayerInterface;
class UInputComponent;
class USceneComponent;


UCLASS()
class RPG_API AHorse : public ACharacter, public IHorseToPlayerInterface
{
	GENERATED_BODY()

public:
	AHorse();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USceneComponent> RiderLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<URiding_System> RidingSystem;

	UFUNCTION(BlueprintCallable)
	float SetRiderSpeed();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UCameraComponent> ViewCamera;

	virtual void BeginPlay() override;

	float ForwardWalkSpeed = 0.f;
	float BackWalkSpeed = 0.f;
	float RightWalkSpeed = 0.f;
	float MaxWalkSpeed = 0.f;

	bool bIsMoving;
	bool bIsBoosting;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void IncreaseMotions();

	void ResetWalkSpeed();

private:


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> FrontBoxCollision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> RightBoxCollision;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> LeftBoxCollision;

};
