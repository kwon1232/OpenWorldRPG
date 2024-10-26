// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonComponent.h"
#include "Animation/BlendSpace.h"
#include "Camera/CameraComponent.h"
#include "FastRidingSystem/Public/Enums.h"
#include "Rider_Component.generated.h"

/**
 *
 */

enum ERiderMountState
{
	LeftBoxCollision,
	RightBoxCollision,
	FrontBoxCollision
};
class ACharacter;
class UAnimationAsset;
class URiding_System;

UCLASS(Blueprintable)
class RPG_API URider_Component : public UCommonComponent
{
	GENERATED_BODY()

public:
	URider_Component();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void BeginPlay() override;

	bool bMontagePlaying;
	bool bIsTurning;
	bool bIsJumping;
	bool bIsSprinting;
	bool bIsWalking;

	bool bIdleDoOnce;
	bool bTurnsDoOnce;
	bool bJumpDoOnce;
	bool bTrotJumpDoOnce;
	bool bSprintJumpDoOnce;

	bool bTrotDoOnce;
	bool bSprintDoOnce;
	bool bWalkBackDoOnce;
	bool bWalkBackTurnsDoOnce;
	bool bTrotTurnsDoOnce;
	bool bSprintTurnsDoOnce;

	float DelayTime;

	void ResetVariables();

	bool bValidReturn;


	void Mount();
	 //Delaytime을 조정하는 함수
	void MountDelay();

	float MountDelayTime;
	float MountAnimLength;

	UPROPERTY()
	URiding_System* RidingSystem;

	UAnimationAsset* MountAnim;

	UPROPERTY()
	ACharacter* Rider;
	/**
	* 스페어 트레이스를 사용하여 라이더의 전방에 존재하는 장애물 또는 타겟을 감지하는 함수.
	* 라이더가 이동할 때 호출되며, 스페어 트레이스를 통해 전방에 존재하는 장애물 또는 타겟을 검출하여 처리한다.
	*
	* @return true: 전방에 장애물 또는 타겟이 감지되어 처리가 필요한 경우, false: 감지된 장애물 또는 타겟이 없어 계속 진행 가능한 경우
	*/
	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	bool Trace();

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	UAnimSequence* RiderMountedAnim;

	UPROPERTY(BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Blendspace")
	UBlendSpace* FlightBlendspace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Animations")
	FMountDismount MountDismountAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Animations")
	FMotionsAnimAsset RidersAnims;

	UPROPERTY(EditAnywhere, Category = "Riding System | Reins Config")
	bool bEnableHoldingReins;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	float MountFromLeftLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	float MountFromFrontLeftLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	float MountFromRightLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	float MountFromFrontRightLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Trace Config")
	float TraceDistance;

	UPROPERTY(EditAnywhere, Category = "Riding System | Trace Config")
	float TraceRadius = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Riding System | Trace Config")
	float TraceVerticalOffset = 60.0f;

	UPROPERTY(EditAnywhere, Category = "Riding System | Trace Config")
	float MaxTraceDistance = 400.0f;

	UPROPERTY(EditAnywhere, Category = "Riding System | Debug")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	FName RiderSocketName;

public:

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	void SetMountType(int32 MntType);

	UFUNCTION()
	int32 GetMountType();

	UFUNCTION()
	UAnimationAsset* GetDismountAnim() const;
	
	UPROPERTY(BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bMountLock;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	ACharacter* Ride;

	 //탑승 후 움직임에 따라 변하는 enum
	UPROPERTY(BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	EMovementStates MovementStates;
	
	 //탑승 상태를 확인하는 값
	UPROPERTY(BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsMounted;

	UFUNCTION()
	void IsMounted();

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	void EventDisMounted();

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	bool bCheckIfMounted;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	int32 RidersMountedAnimType;

	UFUNCTION()
	void SetIsMounted(bool TraceResult);

	UFUNCTION(BlueprintCallable)
	void EventMount();

	void SetMovementType(EMovementStates MovStates);

	UPROPERTY(EditAnywhere, Category = "Riding System | Reins Config")
	FName ReinsRightHandSocket;

	UPROPERTY(EditAnywhere, Category = "Riding System | Reins Config")
	FName ReinsLeftHandSocket;

	UPROPERTY(EditAnywhere, Category = "Riding System | Reins Config")
	float LeftHandReinsAlpha;

	UPROPERTY(EditAnywhere, Category = "Riding System | Reins Config")
	float RightHandReinsAlpha;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	bool bRiderRagdoll;

private:
	
	void SetCollision(ACharacter* Char, ECollisionEnabled::Type Collision);

	void MulticastSetCollision(ACharacter* Char, ECollisionEnabled::Type Collision);

	void SetRide(ACharacter* Char);

	void Attach(ACharacter* CRider, ACharacter* CRide, FName SocketName);

	UFUNCTION()
	void SetMountLock(bool MountLock);

	UFUNCTION()
	void CollisionSettings();

};
