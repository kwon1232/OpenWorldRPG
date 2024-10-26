// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"

#include "Interfaces/PickupInterface.h"
#include "Interfaces/Riding/HorseToPlayerInterface.h"

#include "SlashCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class USlashOverlay;
class USphereComponent;

class AItem;
class ASoul;
class ATreasure;
class AShield;

class IHorseToPlayerInterface;

UCLASS()
class RPG_API ASlashCharacter : public ABaseCharacter, public IPickupInterface, public IHorseToPlayerInterface
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USpringArmComponent> CameraBoom;

	ASlashCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(class AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	
	float SprintSpeedMultiplier;

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	void EquipWeapon(AWeapon* Weapon);
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	void Dodge();

	virtual void DodgeEnd() override;



	/*
	* Player Behavior State
	*/

	void Crouch();
	void Sprint();
	

	/*
	* Play montage functions
	*/

	bool IsWeapon;
	bool IsShield;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsRiding;
	
	void IsWeaponState();
	void IsRidingState();

	void PPlayAttackMontage();


	int32 currentATK = 0;

	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();
	virtual void Die_Implementation() override;
	bool HasEnoughStamina();
	bool IsOccupied();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void ShieldPickUp();

	UFUNCTION(BlueprintCallable)
	void ShieldBack();

	UFUNCTION(BlueprintCallable)
	void HitReactEnd();



private:
	bool IsUnoccupied();
	void InitializeSlashOverlay();
	void SetHUDHealth();

	/*
	* Character components
	*/




	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> DodgeHelper;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AShield* EquippedShield;

	/*
	* Animation montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	ECharacterEquipState CharacterEquipState = ECharacterEquipState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	ECharacterRidingMosionState RidingState = ECharacterRidingMosionState::ERS_RidingIdle;

	ECharacterMosionState CharacterMosionState = ECharacterMosionState::EMS_StandingUp;


	UPROPERTY()
	USlashOverlay* SlashOverlay;

public:
	FORCEINLINE ECharacterEquipState GetCharacterEquipState() const { return CharacterEquipState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
