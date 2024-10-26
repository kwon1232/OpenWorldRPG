// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonComponent.h"
#include "TimerManager.h"
#include "Animation/BlendSpace.h"
#include "Animation/AnimInstance.h"
#include "FastRidingSystem/Public/Enums.h"
#include "Riding_System.generated.h"




class URider_Component;
class ACharacter;
class UAnimMontage;


/**
 * 
 */
UCLASS(Blueprintable)
class RPG_API URiding_System : public UCommonComponent
{
	GENERATED_BODY()

public:

	URiding_System();

	virtual void BeginDestroy() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Riding System | Dispatchers")
	void FnOnMontageCompleted();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float ToggleMotionCooldown;

protected:
	
	virtual void BeginPlay() override;

	//UPROPERTY(BlueprintAssignable)
	//FOnRSMontagePlayDelegate OnCompleted;

	//UPROPERTY(BlueprintAssignable)
	//FOnRSMontagePlayDelegate OnBlendOut;

	//UPROPERTY(BlueprintAssignable)
	//FOnRSMontagePlayDelegate OnInterrupted;

	//UPROPERTY(BlueprintAssignable)
	//FOnRSMontagePlayDelegate OnNotifyBegin;

	//UPROPERTY(BlueprintAssignable)
	//FOnRSMontagePlayDelegate OnNotifyEnd;


	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	UWorld* World;
	FTimerHandle TimerHandler;

	float DelayTime;

	bool bMontagePlaying;
	bool bIsTurning;

	void MotionMoves(UAnimMontage* Anim, EMotions eMotion);
	void MotionTurns(UAnimMontage* TurnAnim);
	void MotionJump(UAnimMontage* Jump);

	void Idle();
	void Walk();
	void Trots();
	void Canter();
	void Gallop();
	void Sprint();
	void WalkBackward();
	void SwimForward();
	void SwimReverse();
	void SwimIdle();

	bool bIdleDoOnce;
	bool bTurnsDoOnce;
	bool bTrotJumpDoOnce;
	bool bWalkJumpDoOnce;
	bool bCanterJumpDoOnce;
	bool bGallopJumpDoOnce;
	bool bSprintJumpDoOnce;

	bool bWalkDoOnce;
	bool bTrotDoOnce;
	bool bCanterDoOnce;
	bool bGallopDoOnce;
	bool bSprintDoOnce;
	bool bWalkBackDoOnce;
	bool bWalkBackTurnsDoOnce;
	bool bWalkTurnsDoOnce;
	bool bTrotTurnsDoOnce;
	bool bCanterTurnsDoOnce;
	bool bGallopTurnsDoOnce;
	bool bSprintTurnsDoOnce;

	bool bSwimForwardDoOnce;
	bool bSwimForwardTurnsDoOnce;
	bool bSwimReverseDoOnce;
	bool bSwimReverseTurnsDoOnce;
	bool bSwimIdleDoOnce;
	bool bSwimTurnsDoOnce;
	bool bFlyDoOnce;

	void ResetTurns();
	void ResetIdle();
	void ResetWalk();
	void ResetWalkBackTurns();
	void ResetWalkBack();
	void ResetWalkTurns();
	void ResetTrot();
	void ResetTrotJump();
	void ResetWalkJump();
	void ResetTrotTurns();
	void ResetCanter();
	void ResetCanterJump();
	void ResetCanterTurns();
	void ResetGallop();
	void ResetGallopJump();
	void ResetGallopTurns();
	void ResetSprint();
	void ResetSprintJump();
	void ResetSprintTurns();
	void ResetFlight();

	void ResetSwimForward();
	void ResetSwimForwardTurns();
	void ResetSwimReverse();
	void ResetSwimReverseTurns();
	void ResetSwimIdle();
	void ResetSwimTurns();
	void ResetAllTurns();


	void JumpEnd(float Duration, EMovementMode MoveMode);

	void MotionEnd(FTimerDelegate InTimerMethod, float Duration);

	void JumpEndDelay(EMovementMode MoveMode);

	bool bValidReturn;

	UPROPERTY()
	bool bCanJump;

	float MoveForward;
	float MoveUp;
	float Turns;
	float MountDelayTime;

	void Dismount();
	void DisableMotions();



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	bool bUseRearing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	bool bUseFlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	bool bIsMountable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float TraceStartOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float TraceEndOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float TrotJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float WalkJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float CanterJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float GallopJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float SprintJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float FlightJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float FlightToLandJumpZVelocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float FlightRotationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float JumpCooldown;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	TEnumAsByte<EStartMotion> StartMotion;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float WalkToIdleDuration;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float TrotToWalkDuration;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float CanterToTrotDuration;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float GallopToCanterDuration;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float DismountTraceXYLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float DismountTraceRearLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float DismountTraceZLength;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float DismountTraceZOffset;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	FName DismountSocketName;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	float DismountTraceRadius;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	TArray<FString> DismountableLandscapes;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	FName ReinsRightSocket;

	UPROPERTY(EditAnywhere, Category = "Riding System | Config")
	FName ReinsLeftSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float FlyRotatinonRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Config")
	float FlyCooldown;

	UPROPERTY(EditAnywhere, Category = "Riding System | debug")
	TEnumAsByte<EDrawDebugTrace::Type> DismountTraceDebug;

	UPROPERTY(EditAnywhere, Category = "Riding System | debug")
	bool bShowDismountTraceHits;

	UPROPERTY(EditAnywhere, Category = "Riding System | debug")
	bool bEnableDismountLocationCheck;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	float RearingDuration;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	float RearingLeftRightDuration;

	UPROPERTY(EditAnywhere, Category = "Riding System | Animation Config")
	EMontageCallbacks DismountMontageCallbacks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Animations")
	FMotionsAnimMontage Animations;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Animations")
	FMotionsAnimAsset WingsAnims;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Blendspace")
	UBlendSpace* MotionBlendspace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Blendspace")
	UBlendSpace* FlightBlendspace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | Blendspace")
	UBlendSpace* WingsBlendspace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsWalking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsTrotting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsCantering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsGalloping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsFlying;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsJumping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bIsSprinting;

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	void Movements(float FMoveForward, float FFurns, float FMoveUp);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	int32 MotionCounter;

	UFUNCTION()
	void ServerCanJump(bool bCnJump);

	UFUNCTION()
	void ServerSetZVelocity(float JumpZVelocity);

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	void ResetMotions(EMotions Motions);

	UFUNCTION()
	void ShouldPlayDismountAnim(bool bDismountAnim);

	UPROPERTY()
	bool bDisableAnimation;

	UFUNCTION()
	void JumpFromAir();

	UFUNCTION()
	void TimerJumpFromAir();

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	EMovementStates MovementStates;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Riding System | DONOTEDIT")
	bool bMotionLock;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	ACharacter* Rider;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	ACharacter* Ride;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	bool bIsMounted;

	UPROPERTY(BlueprintReadOnly, Category = "Riding System | DONOTEDIT")
	bool bIsDead;

	UFUNCTION()
	void OnRep_Dismount();

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	bool GetIsMounted() const;

	UPROPERTY()
	bool bMountLock;

	// 탔을 때 값 변경
	UFUNCTION()
	void SetIsMounted(bool IsMounted);

	// 죽었을 때 값 변경
	UFUNCTION()
	void SetSetIsDead(bool IsMounted);

	// 내릴 때 값 변경
	UFUNCTION()
	void SetDismountLocation(FVector Location);

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	void EventDismount(bool bDisableAnim);

	UFUNCTION()
	void SetRider(ACharacter* Char);

	UFUNCTION()
	void SetRidersComponent(URider_Component* RiderComp);

	UFUNCTION()
	void SetMovementType(EMovementStates MovStates);

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	void SetRotation(ACharacter* Char, FRotator Rot);

	UFUNCTION(BlueprintCallable, Category = "Riding System | DONOTEDIT")
	FSyncedReinsData GetSyncedReinsLocation(float DeltaTime, float InterpSpeed, bool LeftReins);

private:
	
	FRotator CurrentRotation;
	FTimerHandle JumpHandle;
	FTimerHandle SprintJumpHandle;
	FTimerHandle StopProcessHandler;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	TWeakObjectPtr<UAnimInstance> AnimInstancePtr;
	int32 MontageInstanceID;
	uint32 bInterruptedCalledBeforeBlendingOut : 1;
	bool IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const;
	void UnbindDelegates();

	void CollisionSettings();
	bool StoppingProcess();
	void ResetAllStops();
	void EventStopProcessHandler(bool isIdle);
	void TriggerIdle();
	void TriggerWalk();
	void TriggerTrot();
	void TriggerCanter();
	void TriggerGallop();
	void TriggerSprint();

	bool bStopProcess;
	bool bStopProcessFnFlow;
	bool bStopTrotDoOnce;
	bool bStopCanterDoOnce;
	bool bStopCanterTurnDoOnce;
	bool bStopGallopDoOnce;
	bool bStopGallopTurnDoOnce;

	FSyncedReinsData SynchedReinsData;

	UPROPERTY()
	FVector DismountLocation;

	UFUNCTION()
	void UpdateRotation();

	UFUNCTION()
	FTraceData GetDismountLocation();

	UFUNCTION()
	FTraceData LineTrace(float diff, bool bRear);

	UFUNCTION()
	void SetMovements();

	UFUNCTION()
	void MotionFly(float FMoveForward, float FFurns, float FMoveUp);

	UFUNCTION()
	void EventJump(EMovementStates MoveState);

	UFUNCTION()
	void EventJumpEnd();

	UPROPERTY()
	URider_Component* RiderComponent;

	UFUNCTION()
	void ServerDetach(ACharacter* Char);

	UFUNCTION()
	void SetActorTransform(ACharacter* Char, FVector Location, FRotator Rotation);
	
	UFUNCTION()
	void MulticastSetActorTransform(ACharacter* Char, FVector Location, FRotator Rotation);

	UFUNCTION()
	void SetMotionLock(bool MotionLock);
	
	UFUNCTION()
	void LaunchCharacter(ACharacter* Char, float velocity);
};
