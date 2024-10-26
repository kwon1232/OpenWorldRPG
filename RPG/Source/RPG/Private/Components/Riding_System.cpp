// Fill out your copyright notice in the Description page of Project Settings.



#include "Components/Riding_System.h"
#include "Components/Rider_Component.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Math/UnrealMathUtility.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Components/CapsuleComponent.h"


URiding_System::URiding_System()
{
    PrimaryComponentTick.bCanEverTick = true;

    bMontagePlaying = false;
    bDisableAnimation = false;

    MoveForward = 0.f;
    MoveUp = 0.f;
    Turns = 0.f;
    FlyRotatinonRate = 90.f;
    FlyCooldown = 2.f;
    TraceStartOffset = 0.f;
    TraceEndOffset = 0.f;
    TrotJumpZVelocity = 400.f;
    WalkJumpZVelocity = 350.f;
    CanterJumpZVelocity = 400.f;
    GallopJumpZVelocity = 400.f;
    SprintJumpZVelocity = 400.f;
    FlightJumpZVelocity = 400.f;
    FlightToLandJumpZVelocity = 100.f;
    FlightRotationInterpSpeed = 2.f;
    ToggleMotionCooldown = 1.f;
    MotionCounter = 0;
    DismountTraceXYLength = 150.f;
    DismountTraceRearLength = 400.f;
    DismountTraceZLength = 200.f;
    DismountTraceZOffset = -100.f;
    DismountLocation = FVector(0.f, 0.f, 0.f);
    DismountTraceRadius = 30.f;

    bShowDismountTraceHits = false;
    bEnableDismountLocationCheck = true;

    WalkToIdleDuration = 0.5f;
    TrotToWalkDuration = 1.f;
    CanterToTrotDuration = 1.f;
    GallopToCanterDuration = 1.f;

    bIsDead = false;
    bIsJumping = false;;
    bIsTurning = false;
    bMotionLock = false;
    bIsWalking = false;
    bIsTrotting = false;
    bIsCantering = false;
    bIsGalloping = false;
    bIsSprinting = false;
    bUseFlight = false;

    bCanJump = true;

    bSprintDoOnce = false;

    bFlyDoOnce = false;

    bIsMounted = false;
    bIsMountable = true;
    MountDelayTime = 0.f;
    bValidReturn = false;
    CurrentRotation = FRotator(0.f, 0.f, 0.f);

    bIsMounted = false;
    MountDelayTime = 0.f;

    bStopProcessFnFlow = false;
    bStopProcess = false;
    bStopTrotDoOnce = true;
    bStopCanterDoOnce = true;
    bStopCanterTurnDoOnce = true;
    bWalkTurnsDoOnce = false;
    bStopGallopDoOnce = true;
    bStopGallopTurnDoOnce = true;

    StartMotion = EStartMotion::Trot;

    DismountSocketName = "spine_3";

    ReinsRightSocket = "Reins_Bn_Hand_R";
    ReinsLeftSocket = "Reins_Bn_Hand_L";
}

void URiding_System::BeginDestroy()
{
    UnbindDelegates();

	Super::BeginDestroy();
}

void URiding_System::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void URiding_System::BeginPlay()
{
    Super::BeginPlay();

    Ride = Cast<ACharacter>(GetOwner());
    World = GetWorld();
}

void URiding_System::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
    if (bInterrupted)
    {
        FnOnMontageCompleted();
        //OnInterrupted.Broadcast(NAME_None);
        bInterruptedCalledBeforeBlendingOut = true;
    }
    else
    {
        CollisionSettings();
        //OnBlendOut.Broadcast(NAME_None);
    }
}

void URiding_System::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
    if (!bInterrupted)
    {
        FnOnMontageCompleted();
        //OnCompleted.Broadcast(NAME_None);
    }
    else if (!bInterruptedCalledBeforeBlendingOut)
    {
        //OnInterrupted.Broadcast(NAME_None);
    }

    UnbindDelegates();
}

void URiding_System::OnNotifyBeginReceived(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload)
{
    if (IsNotifyValid(NotifyName, BranchingPointNotifyPayload))
    {
        CollisionSettings();
        FnOnMontageCompleted();
        //OnNotifyBegin.Broadcast(NotifyName);
    }
}

void URiding_System::ResetTurns()
{
    bTurnsDoOnce = false;
}

void URiding_System::ResetIdle()
{
    bIdleDoOnce = false;
}

void URiding_System::ResetWalk()
{
    bWalkDoOnce = false;
}

void URiding_System::ResetWalkBackTurns()
{
    bWalkBackTurnsDoOnce = false;
}

void URiding_System::ResetWalkBack()
{
    bWalkBackDoOnce = false;
}

void URiding_System::ResetWalkTurns()
{
    bWalkTurnsDoOnce = false;
}

void URiding_System::ResetTrot()
{
    bTrotDoOnce = false;
}

void URiding_System::ResetTrotTurns()
{
    bTrotTurnsDoOnce = false;
}

void URiding_System::ResetCanter()
{
    bCanterDoOnce = false;
}

void URiding_System::ResetCanterJump()
{
    bCanterJumpDoOnce = false;
}

void URiding_System::ResetCanterTurns()
{
    bCanterTurnsDoOnce = false;
}

void URiding_System::ResetGallop()
{
    bGallopDoOnce = false;
}

void URiding_System::ResetGallopJump()
{
    bGallopJumpDoOnce = false;
}

void URiding_System::ResetGallopTurns()
{
    bGallopTurnsDoOnce = false;
}

void URiding_System::ResetSprint()
{
    bSprintTurnsDoOnce = false;
}

void URiding_System::ResetSprintJump()
{
    bSprintJumpDoOnce = false;
}

void URiding_System::ResetSprintTurns()
{
    bSprintTurnsDoOnce = false;
}

void URiding_System::ResetFlight()
{
    bFlyDoOnce = false;
}

void URiding_System::ResetSwimForward()
{
    bSwimForwardDoOnce = false;
}

void URiding_System::ResetSwimForwardTurns()
{
    bSwimForwardTurnsDoOnce = false;
}

void URiding_System::ResetSwimReverse()
{
    bSwimReverseDoOnce = false;
}

void URiding_System::ResetSwimReverseTurns()
{
    bSwimReverseTurnsDoOnce = false;
}

void URiding_System::ResetSwimIdle()
{
    bSwimIdleDoOnce = false;
}

void URiding_System::ResetSwimTurns()
{
    bSwimTurnsDoOnce = false;
}

void URiding_System::JumpEnd(float Duration, EMovementMode MoveMode)
{
    if (!World) return;

    if (Duration > 0.f)
    {
        FTimerHandle UnusedHandle;
        FTimerDelegate TimerDel;

        TimerDel.BindUObject(this, &URiding_System::JumpEndDelay, MoveMode);
        World->GetTimerManager().SetTimer(UnusedHandle, TimerDel, Duration, false);
    }
    else
    {
        JumpEndDelay(MoveMode);
    }
}

void URiding_System::MotionEnd(FTimerDelegate InTimerMethod, float Duration)
{
    if (!World) return;

    FTimerHandle UnusedHandle;
    World->GetTimerManager().SetTimer(UnusedHandle, InTimerMethod, Duration, false);
}

void URiding_System::ResetTrotJump()
{
    bTrotJumpDoOnce = false;
}

void URiding_System::ResetWalkJump()
{
    bWalkJumpDoOnce = false;
}

void URiding_System::ResetAllTurns()
{
    ResetWalkBackTurns();
    ResetTurns();
    ResetWalkTurns();
    ResetTrotTurns();
    ResetCanterTurns();
    ResetGallopTurns();
    ResetSprintTurns();
    ResetSwimTurns();
    ResetSwimForwardTurns();
    ResetSwimReverseTurns();
}

void URiding_System::JumpEndDelay(EMovementMode MoveMode)
{
    ServerSetMovementMode(Ride, MoveMode);
    ResetMotions(EMotions::None);
    ResetAllTurns();
    SetMotionLock(false);
}

void URiding_System::Dismount()
{
    ServerSetControlRotation(Ride);

    // 만약 컴포넌트에 권한이 있다면,
    if (GetOwner()->HasAuthority())
    {
        //라이드가 라이더의 컨트롤러를 조정한다.
        Ride->GetController()->Possess(Rider);
    }

    const USkeletalMeshComponent* SKMesh = Rider->GetMesh();

    Rider->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));

    AController* Ctrl = Rider->GetController();

    if (Rider->IsPlayerControlled() && Ctrl)
    {
        Ctrl->SetControlRotation(ControlRotation);
    }
}

void URiding_System::DisableMotions()
{
    bIsWalking = false;
    bIsTrotting = false;
    bIsCantering = false;
    bIsGalloping = false;
    bIsSprinting = false;
    MotionCounter = 0;
}

void URiding_System::Movements(float FMoveForward, float FFurns, float FMoveUp)
{
    if (!Ride) return;

    MoveForward = FMoveForward;
    Turns = FFurns;
    MoveUp = FMoveUp;
    bIsTurning = Turns < 0.f || Turns > 0.f;

    if (bIsMounted || !bIsMountable)
    {
        if (bUseFlight)
        {
            if (Ride && Ride->GetMovementComponent()->IsFlying() && bIsFlying)
            {
                MotionFly(MoveForward, Turns, MoveUp);
            }
            else
            {
                SetMovements();
            }
        }
        else
        {
            SetMovements();
        }
    }
    else
    {
        ResetMotions(EMotions::None);
        DisableMotions();
        bIsTurning = false;
    }
}

void URiding_System::ServerCanJump(bool bCnJump)
{
    bCanJump = bCnJump;
}

void URiding_System::ServerSetZVelocity(float JumpZVelocity)
{
    if (Ride) Ride->GetCharacterMovement()->JumpZVelocity = JumpZVelocity;
}

void URiding_System::ResetMotions(EMotions Motions)
{
    switch (Motions)
    {
    case EMotions::None:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::WalkReverse:

        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::Idle:
        ResetWalkBack();

        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::Walk:
        ResetWalkBack();
        ResetIdle();

        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::Trot:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();

        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::Canter:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();

        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::Gallop:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();

        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::Sprint:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();

        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::SwimIdle:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();

        ResetSwimForward();
        ResetSwimReverse();
        break;
    case EMotions::SwimForward:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();

        ResetSwimReverse();
        break;
    case EMotions::SwimReverse:
        ResetWalkBack();
        ResetIdle();
        ResetWalk();
        ResetWalkJump();
        ResetTrot();
        ResetTrotJump();
        ResetCanter();
        ResetCanterJump();
        ResetGallop();
        ResetGallopJump();
        ResetSprint();
        ResetSprintJump();
        ResetSwimIdle();
        ResetSwimForward();

        break;
    }
}

void URiding_System::ShouldPlayDismountAnim(bool bDismountAnim)
{
    bDisableAnimation = bDismountAnim;
}

void URiding_System::JumpFromAir()
{
    if (!World) return;

    SetMotionLock(true);
    ServerStopAnimMontage(Ride);
    ServerSetZVelocity(FlightToLandJumpZVelocity);
    LaunchCharacter(Ride, FlightToLandJumpZVelocity);
    World->GetTimerManager().SetTimer(JumpHandle, this, &URiding_System::TimerJumpFromAir, 0.05f, true);
}

void URiding_System::TimerJumpFromAir()
{
    if (!World) return;

    if (Ride->GetCharacterMovement()->IsSwimming())
    {
        World->GetTimerManager().ClearTimer(JumpHandle);
        JumpEnd(0.f, EMovementMode::MOVE_Swimming);
    }
    else {
        if (Ride->GetCharacterMovement()->IsFalling())
        {
            if (bIsFlying)
            {
                World->GetTimerManager().ClearTimer(JumpHandle);
                JumpEnd(0.f, EMovementMode::MOVE_Flying);
            }
        }
        else
        {
            World->GetTimerManager().ClearTimer(JumpHandle);
            JumpEnd(0.f, EMovementMode::MOVE_Walking);
        }
    }
}

void URiding_System::OnRep_Dismount()
{
    if (bIsMounted) return;

    RiderComponent->SetIsMounted(false);

    if (!bDisableAnimation)
    {
        Ride->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
        Rider->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
    
        if (UAnimInstance* AnimInstance = Rider->GetMesh()->GetAnimInstance())
        {
            const float MontageLength = AnimInstance->Montage_Play(DismountAnim, 1.f);

            if (MontageLength > 0.f)
            {
                AnimInstancePtr = AnimInstance;

                if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(DismountAnim))
                {
                    MontageInstanceID = MontageInstance->GetInstanceID();
                }

                switch (DismountMontageCallbacks)
                {
                case EMontageCallbacks::OnBlendOut:
                    BlendingOutDelegate.BindUObject(this, &URiding_System::OnMontageBlendingOut);
                    AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, DismountAnim);
                    break;

                case EMontageCallbacks::OnNotifyBegin:
                    AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &URiding_System::OnNotifyBeginReceived);
                    break;
                }

                MontageEndedDelegate.BindUObject(this, &URiding_System::OnMontageEnded);
                AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, DismountAnim);

                Dismount();

                // 날 때 감속 속도
                Rider->GetCharacterMovement()->BrakingDecelerationFlying = 15000.f;
                Rider->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
            }

            else
            {
                Dismount();

                Rider->GetCharacterMovement()->DisableMovement();
                float RideRotYaw = Ride ? Ride->GetActorRotation().Yaw : 0.f;
                MulticastSetActorTransform(Rider, DismountLocation, FRotator(0.f, RideRotYaw, 0.f));
                CollisionSettings();

                //OnInterrupted.Broadcast(NAME_None);
            }
        }

        else
        {
            Dismount();

            Rider->GetCharacterMovement()->DisableMovement();
            float RideRotYaw = Ride ? Ride->GetActorRotation().Yaw : 0.f;
            MulticastSetActorTransform(Rider, DismountLocation, FRotator(0.f, RideRotYaw, 0.f));
            CollisionSettings();

            //OnInterrupted.Broadcast(NAME_None);
        }
    }

    else
    {
        Dismount();

        Rider->GetCharacterMovement()->DisableMovement();
        float RideRotYaw = Ride ? Ride->GetActorRotation().Yaw : 0.f;
        MulticastSetActorTransform(Rider, DismountLocation, FRotator(0.f, RideRotYaw, 0.f));
        CollisionSettings();

        //OnInterrupted.Broadcast(NAME_None);
    }
}

void URiding_System::SetIsMounted(bool IsMounted)
{
    bIsMounted = IsMounted;
    OnRep_Dismount();
}

void URiding_System::SetSetIsDead(bool IsMounted)
{
}

void URiding_System::SetDismountLocation(FVector Location)
{
    DismountLocation = Location;
}

void URiding_System::EventDismount(bool bDisableAnim)
{
    // 이동 상태가 멈추거나 죽었을 때
    if ((MoveForward == 0.f && MoveUp == 0.f && Turns == 0.f) || bIsDead)
    {
        // 멈춘 상태가 아니고, 죽지 않았을 때
        if (bStopProcess && !bIsDead) return;

        // 타있는 것이 없거나 라이더 컴포넌트가 없을 때
        if (!Ride || !RiderComponent) return;

        // 탈 것이 있고, 탈 것이 수영중이고, 죽지 않았을 때
        if (Ride && Ride->GetMovementComponent()->IsSwimming() && !bIsDead) return;

        if ((bMotionLock || bIsFlying) && !bIsDead) return;

        if (RiderComponent->bMountLock) return;

        if (bMountLock) return;

        const FTraceData DismountInfo = GetDismountLocation();

        if (DismountInfo.bMatchFound || bIsDead)
        {
            bMountLock = true;
           ShouldPlayDismountAnim(bDisableAnim);
            const FVector RideLoc = FVector(Ride->GetActorLocation().X, Ride->GetActorLocation().Y, Ride->GetActorLocation().Z + 100);
            SetDismountLocation(bIsDead ? RideLoc : DismountInfo.Location);

            SetIsMounted(false);
        }
    }
}

void URiding_System::SetRotation(ACharacter* Char, FRotator Rot)
{
    Char->SetActorRotation(Rot);
}

void URiding_System::SetMovementType(EMovementStates MovStates)
{
    MovementStates = MovStates;
    if (RiderComponent) RiderComponent->SetMovementType(MovStates);
}

FSyncedReinsData URiding_System::GetSyncedReinsLocation(float DeltaTime, float InterpSpeed, bool LeftReins)
{
    if (!Rider || !Ride) return SynchedReinsData;

    const FName ReinsHandSocket = LeftReins ? RiderComponent->ReinsLeftHandSocket : RiderComponent->ReinsRightHandSocket;
    const FVector RidersSocketLocation = Rider->GetMesh()->GetSocketLocation(ReinsHandSocket);

    const FName ReinsSocket = LeftReins ? ReinsLeftSocket : ReinsRightSocket;
    const FVector RidesSocketLocation = Ride->GetMesh()->GetSocketLocation(ReinsSocket);

    SynchedReinsData.SocketLocation = FMath::VInterpTo(RidesSocketLocation, RidersSocketLocation, DeltaTime, InterpSpeed);
    SynchedReinsData.ReinsAlpha = LeftReins ? RiderComponent->LeftHandReinsAlpha : RiderComponent->RightHandReinsAlpha;

    return SynchedReinsData;
}

void URiding_System::CollisionSettings()
{
    Rider->GetCharacterMovement()->BrakingDecelerationFlying = 0.f;

    Ride->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
    Ride->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

    Rider->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

    Rider->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
    Rider->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);

    Rider->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    Rider->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

    bMountLock = false;
    RiderComponent->bCheckIfMounted = false;
}

bool URiding_System::StoppingProcess()
{
    if (bStopProcess) return false;

    FTimerDelegate TimerDel;
    bool IsIdle = false;

    switch (MovementStates)
    {
    case EMovementStates::Trot:
        if (!bStopTrotDoOnce) bStopProcessFnFlow = false;

        IsIdle = true;
        TimerDel = FTimerDelegate::CreateUObject(this, &URiding_System::EventStopProcessHandler, IsIdle);
        GetWorld()->GetTimerManager().SetTimer(StopProcessHandler, TimerDel, WalkToIdleDuration, false);
        bStopProcess = true;
        bIsWalking = true;
        TriggerWalk();

        break;
    case EMovementStates::Canter:
        if (!bStopTrotDoOnce) bStopProcessFnFlow = false;

        IsIdle = false;
        TimerDel = FTimerDelegate::CreateUObject(this, &URiding_System::EventStopProcessHandler, IsIdle);
        GetWorld()->GetTimerManager().SetTimer(StopProcessHandler, TimerDel, TrotToWalkDuration, false);
        bStopProcess = true;
        bIsTrotting = true;
        TriggerTrot();

        break;
    case EMovementStates::Gallop:
        if (!bStopCanterDoOnce) bStopProcessFnFlow = false;

        IsIdle = false;
        TimerDel = FTimerDelegate::CreateUObject(this, &URiding_System::EventStopProcessHandler, IsIdle);
        GetWorld()->GetTimerManager().SetTimer(StopProcessHandler, TimerDel, CanterToTrotDuration, false);
        bStopProcess = true;
        bIsCantering = true;
        TriggerCanter();

        break;
    case EMovementStates::Sprint:
        if (!bStopGallopDoOnce) bStopProcessFnFlow = false;

        IsIdle = false;
        TimerDel = FTimerDelegate::CreateUObject(this, &URiding_System::EventStopProcessHandler, IsIdle);
        GetWorld()->GetTimerManager().SetTimer(StopProcessHandler, TimerDel, GallopToCanterDuration, false);
        bStopProcess = true;
        bIsGalloping = true;
        TriggerGallop();

        break;
    case EMovementStates::GallopLeftTurn:
    case EMovementStates::GallopRightTurn:
        if (!bStopCanterTurnDoOnce) bStopProcessFnFlow = false;

        IsIdle = false;
        TimerDel = FTimerDelegate::CreateUObject(this, &URiding_System::EventStopProcessHandler, IsIdle);
        GetWorld()->GetTimerManager().SetTimer(StopProcessHandler, TimerDel, CanterToTrotDuration, false);
        bStopProcess = true;
        bIsTurning = true;
        bIsCantering = true;
        TriggerCanter();

        break;
    case EMovementStates::SprintLeftTurn:
    case EMovementStates::SprintRightTurn:
        if (!bStopGallopTurnDoOnce) bStopProcessFnFlow = false;

        IsIdle = false;
        TimerDel = FTimerDelegate::CreateUObject(this, &URiding_System::EventStopProcessHandler, IsIdle);
        GetWorld()->GetTimerManager().SetTimer(StopProcessHandler, TimerDel, GallopToCanterDuration, false);
        bStopProcess = true;
        bIsTurning = true;
        bIsGalloping = true;
        TriggerGallop();

        break;
    default:
        ResetAllStops();
        MotionCounter = 0;
        bStopProcessFnFlow = true;

        break;
    }

    return bStopProcessFnFlow;
}

void URiding_System::ResetAllStops()
{
    bStopTrotDoOnce = true;
    bStopCanterDoOnce = true;
    bStopCanterTurnDoOnce = true;
    bStopGallopDoOnce = true;
    bStopGallopTurnDoOnce = true;
}

void URiding_System::TriggerWalk()
{
    MotionCounter = 1;

    bIsWalking = true;
    bIsTrotting = false;
    bIsCantering = false;
    bIsGalloping = false;
    bIsSprinting = false;
    bIsJumping = bIsJumping ? bIsJumping : (Ride ? Ride->GetCharacterMovement()->IsFalling() : false);

    Walk();
}

void URiding_System::TriggerTrot()
{
    MotionCounter = 2;

    bIsWalking = false;
    bIsTrotting = true;
    bIsCantering = false;
    bIsGalloping = false;
    bIsSprinting = false;
    bIsJumping = bIsJumping ? bIsJumping : (Ride ? Ride->GetCharacterMovement()->IsFalling() : false);

    Trots();
}

void URiding_System::TriggerCanter()
{
    MotionCounter = 3;

    bIsWalking = false;
    bIsTrotting = false;
    bIsCantering = true;
    bIsGalloping = false;
    bIsSprinting = false;
    bIsJumping = bIsJumping ? bIsJumping : (Ride ? Ride->GetCharacterMovement()->IsFalling() : false);

    Canter();
}

void URiding_System::TriggerGallop()
{
    MotionCounter = 4;

    bIsWalking = false;
    bIsTrotting = false;
    bIsCantering = false;
    bIsGalloping = true;
    bIsSprinting = false;
    bIsJumping = bIsJumping ? bIsJumping : (Ride ? Ride->GetCharacterMovement()->IsFalling() : false);

    Gallop();
}

void URiding_System::TriggerSprint()
{
    MotionCounter = 5;

    bIsWalking = false;
    bIsTrotting = false;
    bIsCantering = false;
    bIsGalloping = false;
    bIsSprinting = true;
    bIsJumping = bIsJumping ? bIsJumping : (Ride ? Ride->GetCharacterMovement()->IsFalling() : false);

    Sprint();
}

void URiding_System::EventStopProcessHandler(bool isIdle)
{
    if (StopProcessHandler.IsValid()) GetWorld()->GetTimerManager().ClearTimer(StopProcessHandler);
    bStopProcess = false;
    ResetAllStops();
    if (isIdle) bStopProcessFnFlow = true;
}

void URiding_System::TriggerIdle()
{
    const bool isIdle = StoppingProcess();

    if (isIdle)
    {
        MotionCounter = 0;
        Idle();
        DisableMotions();
    }
}

bool URiding_System::IsNotifyValid(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload) const
{
    return ((MontageInstanceID != INDEX_NONE) && (BranchingPointNotifyPayload.MontageInstanceID == MontageInstanceID));
}

void URiding_System::UnbindDelegates()
{
    if (UAnimInstance* AnimInstance = AnimInstancePtr.Get())
    {
        AnimInstance->OnPlayMontageNotifyBegin.RemoveDynamic(this, &URiding_System::OnNotifyBeginReceived);
    }
}

void URiding_System::UpdateRotation()
{
    if (!Ride || !World) return;

    const FRotator TargetRot = FRotator(0.f, Ride->GetBaseAimRotation().Yaw, 0.f);
    CurrentRotation = FMath::RInterpTo(CurrentRotation, TargetRot, World->GetDeltaSeconds(), FlightRotationInterpSpeed);

    Ride->SetActorRotation(CurrentRotation);
    SetRotation(Ride, CurrentRotation);
}

FTraceData URiding_System::GetDismountLocation()
{
    FTraceData TraceData;

    if (RiderComponent && (RiderComponent->GetMountType() == 1 || RiderComponent->GetMountType() == 3))
    {
        TraceData = LineTrace(-1.f, false);

        if (TraceData.bMatchFound)
        {
            return TraceData;
        }
        else
        {
            TraceData = LineTrace(1.f, false);
            RiderComponent->SetMountType(0);
            ServerSetDismountAnim(RiderComponent->DismountAnim);

            if (TraceData.bMatchFound)
            {
                return TraceData;
            }
            else
            {
                TraceData = LineTrace(-1.f, true);
                RiderComponent->SetMountType(2);
                ServerSetDismountAnim(RiderComponent->DismountAnim);
                return TraceData;
            }
        }
    }
    else if (RiderComponent && RiderComponent->GetMountType() == 2)
    {
        TraceData = LineTrace(-1.f, true);

        if (TraceData.bMatchFound)
        {
            return TraceData;
        }
        else
        {
            TraceData = LineTrace(1.f, false);
            RiderComponent->SetMountType(0);
            ServerSetDismountAnim(RiderComponent->DismountAnim);

            if (TraceData.bMatchFound)
            {
                return TraceData;
            }
            else
            {
                TraceData = LineTrace(-1.f, false);
                RiderComponent->SetMountType(1);
                ServerSetDismountAnim(RiderComponent->DismountAnim);
                return TraceData;
            }
        }
    }
    else
    {
        TraceData = LineTrace(1.f, false);

        if (TraceData.bMatchFound)
        {
            return TraceData;
        }
        else
        {
            TraceData = LineTrace(-1.f, false);
            RiderComponent->SetMountType(1);
            ServerSetDismountAnim(RiderComponent->DismountAnim);

            if (TraceData.bMatchFound)
            {
                return TraceData;
            }
            else
            {
                TraceData = LineTrace(-1.f, true);
                RiderComponent->SetMountType(2);
                ServerSetDismountAnim(RiderComponent->DismountAnim);
                return TraceData;
            }
        }
    }

    return TraceData;
}

FTraceData URiding_System::LineTrace(float diff, bool bRear)
{
    if (!World) return FTraceData();

    const FVector StartLocation = Ride->GetMesh()->GetSocketLocation(DismountSocketName);
    const FVector EndLocation = (StartLocation + ((bRear ? Ride->GetMesh()->GetRightVector() : Ride->GetMesh()->GetForwardVector()) * (diff * (bRear ? DismountTraceRearLength : DismountTraceXYLength)))) - FVector(0.f, 0.f, DismountTraceZLength);

    TArray<AActor*> toignore;
    toignore.Empty();
    toignore.Add(GetOwner());
    toignore.Add(Ride);
    FHitResult HitResult = FHitResult(ForceInit);

    UKismetSystemLibrary::SphereTraceSingle(Rider->GetWorld(), StartLocation, EndLocation, DismountTraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, toignore, DismountTraceDebug, HitResult, true);

    bool bMatchFound = false;

    if (!bEnableDismountLocationCheck)
    {
        bMatchFound = true;
    }

    else
    {
        for (FString Landscape : DismountableLandscapes)
        {
            const AActor* HActor = HitResult.GetActor();

            if (bShowDismountTraceHits && GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, FString::Printf(TEXT("HitActor = %s"), (*HActor->GetName())));
            }

            if (HActor && HActor->GetName().Contains(Landscape))
            {
                bMatchFound = true;
                break;
            }
            else
            {
                bMatchFound = false;
            }
        }
    }

    FTraceData TraceData;
    TraceData.bMatchFound = bMatchFound;
    TraceData.Location = FVector(HitResult.ImpactPoint.X, HitResult.ImpactPoint.Y, HitResult.ImpactPoint.Z - DismountTraceZOffset);

    return TraceData;
}

void URiding_System::SetMovements()
{
    if (bMotionLock) return;

    if (Ride->GetMovementComponent()->IsSwimming())
    {
        DisableMotions();

        if (MoveForward > 0.f)
        {
            SwimForward();
        }
        else if (MoveForward < 0.f)
        {
            SwimReverse();
        }
        else
        {
            SwimIdle();
        }
    }
    else if (bIsFlying && bUseFlight)
    {
        if (!bFlyDoOnce)
        {
            bFlyDoOnce = true;
            SetMovementType(EMovementStates::Flying);
            SetMotionLock(true);
            ServerStopAnimMontage(Ride);
            ServerSetMovementMode(Ride, EMovementMode::MOVE_Flying);
            SetMotionLock(false);
        }
    }
    else if (MoveForward == 1.f)
    {
        EventStopProcessHandler(false);
        bStopProcessFnFlow = false;

        if (bIsSprinting)
        {
            TriggerSprint();
        }
        else if (bIsGalloping)
        {
            TriggerGallop();
        }
        else if (bIsCantering)
        {
            TriggerCanter();
        }
        else
        {
            switch (StartMotion)
            {
            case EStartMotion::Walk:
                if (bIsTrotting)
                {
                    TriggerTrot();
                }
                else
                {
                    TriggerWalk();
                }
                break;
            case EStartMotion::Trot:
                if (bIsWalking)
                {
                    TriggerWalk();
                }
                else
                {
                    TriggerTrot();
                }
                break;
            }
        }
    }
    else if (MoveForward == -1.f && !bStopProcess)
    {
        MotionCounter = -1;
        WalkBackward();
    }
    else
    {
        TriggerIdle();
    }
}

void URiding_System::MotionFly(float FMoveForward, float FFurns, float FMoveUp)
{
    DisableMotions();

    const FVector FwdVector = Ride->GetActorForwardVector();
    const FVector RightVector = Ride->GetActorRightVector();
    const FVector UpVector = Ride->GetActorUpVector();

    Ride->AddMovementInput(FwdVector, FMoveForward);
    Ride->AddMovementInput(RightVector, FFurns);
    Ride->AddMovementInput(UpVector, FMoveUp);
}

void URiding_System::EventJump(EMovementStates MoveState)
{
    if (!World) return;

    if (Ride->GetCharacterMovement()->IsSwimming())
    {
        World->GetTimerManager().ClearTimer(JumpHandle);
    }
    else if (!Ride->GetCharacterMovement()->IsFalling())
    {
        World->GetTimerManager().ClearTimer(JumpHandle);
        JumpEnd(0.f, EMovementMode::MOVE_Walking);
        SetMovementType(MoveState);

        FTimerHandle UnusedHandle;
        World->GetTimerManager().SetTimer(UnusedHandle, this, &URiding_System::EventJumpEnd, JumpCooldown, false);
    }
}

void URiding_System::EventJumpEnd()
{
    ServerCanJump(true);
}

void URiding_System::ServerDetach(ACharacter* Char)
{
    if (Char) Char->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));
}

void URiding_System::SetActorTransform(ACharacter* Char, FVector Location, FRotator Rotation)
{
    MulticastSetActorTransform(Char, Location, Rotation);
}

void URiding_System::MulticastSetActorTransform(ACharacter* Char, FVector Location, FRotator Rotation)
{
    if (Char)
    {
        Char->SetActorLocation(Location);
        Char->SetActorRotation(Rotation);
    }
}

void URiding_System::SetMotionLock(bool MotionLock)
{
    bMotionLock = MotionLock;
}

void URiding_System::LaunchCharacter(ACharacter* Char, float velocity)
{
    Char->LaunchCharacter(FVector(0.f, 0.f, velocity), false, true);
}

bool URiding_System::GetIsMounted() const
{
    return bIsMounted;
}

void URiding_System::SetRider(ACharacter* Char)
{
    Rider = Char;
}

void URiding_System::SetRidersComponent(URider_Component* RiderComp)
{
    RiderComponent = RiderComp;
}

void URiding_System::MotionMoves(UAnimMontage* Anim, EMotions eMotion)
{
    PlayMontage(Ride, Anim);

    ResetAllTurns();
    ResetMotions(eMotion);
}

void URiding_System::MotionTurns(UAnimMontage* TurnAnim)
{
    PlayMontage(Ride, TurnAnim);
    ResetMotions(EMotions::None);
}

void URiding_System::MotionJump(UAnimMontage* Jump)
{
    PlayMontage(Ride, Jump);
}

void URiding_System::Idle()
{
    if (bIsTurning)
    {
        if (bIsJumping && bUseRearing)
        {
            SetMovementType(Turns == 1.f ? EMovementStates::RearingRightTurn : EMovementStates::RearingLeftTurn);
            SetMotionLock(true);
            if (Animations.RearingLeftTurn && Animations.RearingRightTurn) MotionJump(Turns == 1.f ? Animations.RearingLeftTurn : Animations.RearingRightTurn);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::JumpEndDelay, EMovementMode::MOVE_Walking);
            MotionEnd(TimerDel, RearingLeftRightDuration);
        }

        if (!bTurnsDoOnce)
        {
            bTurnsDoOnce = true;
            SetMovementType(Turns == 1.f ? EMovementStates::WalkRightTurn : EMovementStates::WalkLeftTurn);
            if (Animations.IdleLeftTurn && Animations.IdleRightTurn) MotionTurns(Turns < 0.f ? Animations.IdleLeftTurn : Animations.IdleRightTurn);
        }
    }
    else
    {
        if (bIsJumping && bUseRearing)
        {
            SetMovementType(EMovementStates::Rearing);
            SetMotionLock(true);
            if (Animations.Rearing) MotionJump(Animations.Rearing);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::JumpEndDelay, EMovementMode::MOVE_Walking);
            MotionEnd(TimerDel, RearingDuration);
        }
        else
        {
            if (!bIdleDoOnce)
            {
                bIdleDoOnce = true;
                SetMovementType(EMovementStates::Idle);
                if (Animations.Idle) MotionMoves(Animations.Idle, EMotions::Idle);
            }
        }
    }
}

void URiding_System::Walk()
{
    if (bIsJumping && !bWalkJumpDoOnce)
    {
        if (bCanJump)
        {
            SetMotionLock(true);
            ServerCanJump(false);
            ServerStopAnimMontage(Ride);
            ServerSetZVelocity(WalkJumpZVelocity);
            Ride->Jump();
            SetMovementType(EMovementStates::WalkJump);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::EventJump, EMovementStates::Walk);
            if (World) World->GetTimerManager().SetTimer(JumpHandle, TimerDel, 0.05f, true);
        }
        else
        {
            ResetWalkJump();
        }

    }
    else if (bIsTurning)
    {
        if (!bWalkTurnsDoOnce)
        {
            bWalkTurnsDoOnce = true;
            SetMovementType(Turns == 1.f ? EMovementStates::WalkRightTurn : EMovementStates::WalkLeftTurn);
            if (Animations.WalkLeftTurn && Animations.WalkRightTurn) MotionTurns(Turns < 0.f ? Animations.WalkLeftTurn : Animations.WalkRightTurn);
        }
    }
    else
    {
        if (!bWalkDoOnce)
        {
            bWalkDoOnce = true;
            SetMovementType(EMovementStates::Walk);
            if (Animations.Walk) MotionMoves(Animations.Walk, EMotions::Walk);
        }
    }
}

void URiding_System::Trots()
{
    if (bIsJumping && !bTrotJumpDoOnce)
    {
        if (bCanJump)
        {
            SetMotionLock(true);
            ServerCanJump(false);
            ServerStopAnimMontage(Ride);
            ServerSetZVelocity(TrotJumpZVelocity);
            Ride->Jump();
            SetMovementType(EMovementStates::TrotJump);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::EventJump, EMovementStates::Trot);
            if (World) World->GetTimerManager().SetTimer(JumpHandle, TimerDel, 0.05f, true);
        }

        else
        {
            ResetTrotJump();
        }
    }
    else if (bIsTurning)
    {
        if (!bTrotTurnsDoOnce)
        {
            bTrotTurnsDoOnce = true;
            SetMovementType(Turns == 1.f ? EMovementStates::TrotRightTurn : EMovementStates::TrotLeftTurn);
            if (Animations.TrotLeftTurn && Animations.TrotRightTurn) MotionTurns(Turns < 0.f ? Animations.TrotLeftTurn : Animations.TrotRightTurn);
        }
    }
    else
    {
        if (!bTrotDoOnce)
        {
            bTrotDoOnce = true;
            SetMovementType(EMovementStates::Trot);
            if (Animations.Trot) MotionMoves(Animations.Trot, EMotions::Trot);
        }
    }
}

void URiding_System::Canter()
{
    if (bIsJumping && !bCanterJumpDoOnce)
    {
        if (bCanJump)
        {
            SetMotionLock(true);
            ServerCanJump(false);
            ServerStopAnimMontage(Ride);
            ServerSetZVelocity(CanterJumpZVelocity);
            Ride->Jump();
            SetMovementType(EMovementStates::CanterJump);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::EventJump, EMovementStates::Canter);
            if (World) World->GetTimerManager().SetTimer(JumpHandle, TimerDel, 0.05f, true);
        }

        else
        {
            ResetCanterJump();
        }
    }
    else if (bIsTurning)
    {
        if (!bCanterTurnsDoOnce)
        {
            bCanterTurnsDoOnce = true;
            SetMovementType(Turns == 1.f ? EMovementStates::CanterRightTurn : EMovementStates::CanterLeftTurn);
            if (Animations.CanterLeftTurn && Animations.CanterRightTurn) MotionTurns(Turns < 0.f ? Animations.CanterLeftTurn : Animations.CanterRightTurn);
        }
    }
    else
    {
        if (!bCanterDoOnce)
        {
            bCanterDoOnce = true;
            SetMovementType(EMovementStates::Canter);
            if (Animations.Canter) MotionMoves(Animations.Canter, EMotions::Canter);
        }
    }
}

void URiding_System::Gallop()
{
    if (bIsJumping && !bGallopJumpDoOnce)
    {
        if (bCanJump)
        {
            SetMotionLock(true);
            ServerCanJump(false);
            ServerStopAnimMontage(Ride);
            ServerSetZVelocity(GallopJumpZVelocity);
            Ride->Jump();
            SetMovementType(EMovementStates::GallopJump);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::EventJump, EMovementStates::Gallop);
            if (World) World->GetTimerManager().SetTimer(JumpHandle, TimerDel, 0.05f, true);
        }
    }

    else
    {
        ResetGallopJump();
    }
}

void URiding_System::Sprint()
{
    if (bIsJumping && !bSprintJumpDoOnce)
    {
        if (bCanJump)
        {
            SetMotionLock(true);
            ServerCanJump(false);
            ServerStopAnimMontage(Ride);
            ServerSetZVelocity(SprintJumpZVelocity);
            Ride->Jump();
            SetMovementType(EMovementStates::SprintJump);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::EventJump, EMovementStates::Sprint);
            if (World) World->GetTimerManager().SetTimer(JumpHandle, TimerDel, 0.05f, true);
        }

        else
        {
            ResetSprintJump();
        }
    }
    else if (bIsTurning)
    {
        if (!bSprintTurnsDoOnce)
        {
            bSprintTurnsDoOnce = true;
            SetMovementType(Turns == 1.f ? EMovementStates::SprintRightTurn : EMovementStates::SprintLeftTurn);
            if (Animations.SprintLeftTurn && Animations.SprintRightTurn) MotionTurns(Turns < 0.f ? Animations.SprintLeftTurn : Animations.SprintRightTurn);
        }
    }
    else
    {
        if (!bSprintDoOnce)
        {
            bSprintDoOnce = true;
            SetMovementType(EMovementStates::Sprint);
            if (Animations.Sprint) MotionMoves(Animations.Sprint, EMotions::Sprint);
        }
    }
}

void URiding_System::WalkBackward()
{
    if (bIsTurning)
    {
        if (bIsJumping && bUseRearing)
        {
            SetMovementType(Turns == 1.f ? EMovementStates::RearingRightTurn : EMovementStates::RearingLeftTurn);
            SetMotionLock(true);
            if (Animations.RearingRightTurn && Animations.RearingLeftTurn) MotionJump(Turns == 1.f ? Animations.RearingRightTurn : Animations.RearingLeftTurn);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::JumpEndDelay, EMovementMode::MOVE_Walking);
            MotionEnd(TimerDel, RearingLeftRightDuration);
        }

        if (!bWalkBackTurnsDoOnce)
        {
            bWalkBackTurnsDoOnce = true;
            SetMovementType(Turns == 1.f ? EMovementStates::WalkReverseRightTurn : EMovementStates::WalkReverseLeftTurn);
            if (Animations.WalkReverseLeftTurn && Animations.WalkReverseRightTurn) MotionTurns(Turns < 0.f ? Animations.WalkReverseLeftTurn : Animations.WalkReverseRightTurn);
        }
    }
    else
    {
        if (bIsJumping)
        {
            SetMovementType(EMovementStates::Rearing);
            SetMotionLock(true);
            if (Animations.Rearing) MotionJump(Animations.Rearing);

            FTimerDelegate TimerDel;
            TimerDel.BindUObject(this, &URiding_System::JumpEndDelay, EMovementMode::MOVE_Walking);
            MotionEnd(TimerDel, RearingDuration);
        }
        else
        {
            if (!bWalkBackDoOnce)
            {
                bWalkBackDoOnce = true;
                SetMovementType(EMovementStates::WalkReverse);
                if (Animations.WalkReverse) MotionMoves(Animations.WalkReverse, EMotions::WalkReverse);
            }
        }
    }
}

void URiding_System::SwimForward()
{
    if (bIsTurning)
	{
		if (!bSwimForwardTurnsDoOnce)
		{
			bSwimForwardTurnsDoOnce = true;
			ServerSetMovementMode(Rider, EMovementMode::MOVE_None);
			SetMovementType(Turns == 1.f ? EMovementStates::SwimForwardRightTurn : EMovementStates::SwimForwardLeftTurn);
			if (Animations.SwimForwardLeftTurn && Animations.SwimForwardRightTurn) MotionTurns(Turns < 0.f ? Animations.SwimForwardLeftTurn : Animations.SwimForwardRightTurn);
		}
	}
    else
    {
        if (!bSwimForwardDoOnce)
        {
            bSwimForwardDoOnce = true;
            ServerSetMovementMode(Rider, EMovementMode::MOVE_None);
            SetMovementType(EMovementStates::SwimForward);
            if (Animations.SwimForward) MotionMoves(Animations.SwimForward, EMotions::SwimForward);
        }
    }
}

void URiding_System::SwimReverse()
{
    if (bIsTurning)
    {
        if (!bSwimReverseTurnsDoOnce)
        {
            bSwimReverseTurnsDoOnce = true;
            ServerSetMovementMode(Rider, EMovementMode::MOVE_None);
            SetMovementType(Turns == 1.f ? EMovementStates::SwimReverseRightTurn : EMovementStates::SwimReverseLeftTurn);
            if (Animations.SwimReverseLeftTurn && Animations.SwimReverseRightTurn) MotionTurns(Turns < 0.f ? Animations.SwimReverseLeftTurn : Animations.SwimReverseRightTurn);
        }
    }
    else
    {
        if (!bSwimReverseDoOnce)
        {
            bSwimReverseDoOnce = true;
            ServerSetMovementMode(Rider, EMovementMode::MOVE_None);
            SetMovementType(EMovementStates::SwimReverse);
            if (Animations.SwimReverse) MotionMoves(Animations.SwimReverse, EMotions::SwimReverse);
        }
    }
}

void URiding_System::SwimIdle()
{
    if (bIsTurning)
    {
        if (!bSwimTurnsDoOnce)
        {
            bSwimTurnsDoOnce = true;
            ServerSetMovementMode(Rider, EMovementMode::MOVE_None);
            SetMovementType(Turns == 1.f ? EMovementStates::SwimRightTurn : EMovementStates::SwimLeftTurn);
            if (Animations.SwimLeftTurn && Animations.SwimRightTurn) MotionTurns(Turns < 0.f ? Animations.SwimLeftTurn : Animations.SwimRightTurn);
        }
    }
    else
    {
        if (!bSwimIdleDoOnce)
        {
            bSwimIdleDoOnce = true;
            ServerSetMovementMode(Rider, EMovementMode::MOVE_None);
            SetMovementType(EMovementStates::SwimIdle);
            if (Animations.SwimIdle) MotionMoves(Animations.SwimIdle, EMotions::SwimIdle);
        }
    }
}

