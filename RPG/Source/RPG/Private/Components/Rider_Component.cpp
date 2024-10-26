// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Rider_Component.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/Character.h"
#include "Components/Riding_System.h"
#include "TimerManager.h"
#include "GameFramework/Controller.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "Animation/AnimMontage.h"

URider_Component::URider_Component()
{
	PrimaryComponentTick.bCanEverTick = false;

	TraceRadius = 130.f;
	TraceVerticalOffset = 0.f;
	MaxTraceDistance = 60.f;
	bIsMounted = false;
	bMountLock = false;
	MountDelayTime = 0.f;
	bValidReturn = false;
	RidersMountedAnimType = 0.f;
	RiderSocketName = "MountPoint";
	MovementStates = EMovementStates::Idle;
	bRiderRagdoll = false;

	bEnableHoldingReins = false;
	ReinsRightHandSocket = "Reins_Right_Socket";
	ReinsLeftHandSocket = "Reins_Left_Socket";
	LeftHandReinsAlpha = 0.f;
	RightHandReinsAlpha = 0.f;

	SetIsReplicatedByDefault(true);
}

void URider_Component::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void URider_Component::BeginPlay()
{
	Super::BeginPlay();

	Rider = Cast<ACharacter>(GetOwner());
}

void URider_Component::ResetVariables()
{
	SetMountLock(false);
	LeftHandReinsAlpha = 0.f;
	RightHandReinsAlpha = 0.f;
	bValidReturn = false;
}

void URider_Component::Mount()
{
	if (!Ride || !Rider) ResetVariables();

	ServerSetControlRotation(Rider);

	Rider->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// 권한이 있다면 라이더 컨트롤러를 라이드에게 허용한다.
	if (GetOwner()->HasAuthority())
	{
		Rider->GetController()->Possess(Ride);
	}
			
	// 라이더를 라이드에게 부착해준다.
	Rider->AttachToComponent(Ride->GetMesh(), 
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, 
			EAttachmentRule::SnapToTarget, 
			EAttachmentRule::KeepWorld, false), 
		RiderSocketName);

	AController* Ctrl = Ride->GetController();

	if (Ride->IsPlayerControlled() && Ctrl)
	{
		Ctrl->SetControlRotation(ControlRotation);
	}
}

void URider_Component::MountDelay()
{
	Rider->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	SetMountLock(false);

	if (RidingSystem) RidingSystem->bMountLock = false;

	LeftHandReinsAlpha = bEnableHoldingReins ? 1.f : 0.f;
	RightHandReinsAlpha = bEnableHoldingReins ? 1.f : 0.f;

	// Trace 호출과 관련있는 함수
	if (RidingSystem && GetOwner()->HasAuthority())
	{
		RidingSystem->SetRider(Rider);
		RidingSystem->SetRidersComponent(this);
		RidingSystem->SetIsMounted(true);
		RidingSystem->ServerSetDismountAnim(DismountAnim);
	}
}

bool URider_Component::Trace()
{
	// 추적할 때 무시할 액터를 담는 배열
	TArray<AActor*> toignore;
	toignore.Empty();
	toignore.Add(Rider);

	// 스페어 트레이스 결과를 저장할 변수
	FHitResult HitResult = FHitResult(ForceInit);

	// 스페어 트레이스 시작점
	FVector Points = FVector(0.f, 0.f, 0.f);

	Points = Rider->GetRootComponent()->GetComponentLocation() + (Rider->GetRootComponent()->GetComponentRotation().Vector() * MaxTraceDistance);


	// 스페어 트레이스의 시작점과 끝점 설정
	FVector StartTrace = FVector(Points.X, Points.Y, Points.Z + TraceVerticalOffset);
	FVector EndTrace = FVector(Points.X, Points.Y, Points.Z + TraceVerticalOffset);
	UKismetSystemLibrary::SphereTraceSingle(Rider->GetWorld(), StartTrace, EndTrace, TraceRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility), false, toignore, EDrawDebugTrace::Type::ForDuration, HitResult, true);

	AActor* HitActor = HitResult.GetActor();

	// 히트된 액터를 가져오고 ACharacter로 캐스팅하여 Ride에 할당
	if (HitResult.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("GetName : %s"), *HitResult.GetActor()->GetName());
		UE_LOG(LogTemp, Log, TEXT("StartTrace.X : %d"), StartTrace.X);
		UE_LOG(LogTemp, Log, TEXT("EndTrace.X : %d"), EndTrace.X);

		Ride = Cast<ACharacter>(HitActor);
		SetRide(Ride);
		if (Ride)
		{
			RidingSystem = Ride->FindComponentByClass<URiding_System>();
		}
		// Ride가 존재하고, 라이딩 시스템이 존재하며, Ride가 이미 마운트된 상태이거나 Ride가 죽은 상태라면 false 반환
		if (!Ride || !RidingSystem || RidingSystem->GetIsMounted() || RidingSystem->bIsDead)
			bValidReturn = false;

		else
		{
			// 히트된 Component가 유효한 경우 해당 Component의 이름을 확인하여 적절한 동작 수행
			if (HitResult.Component.IsValid())
			{
				const FString CompName = HitResult.GetComponent()->GetName();

				if (CompName == "LeftBoxCollision")
				{
					SetMountType(LeftBoxCollision);
					bValidReturn = true;
				}
				else if (CompName == "RightBoxCollision")
				{
					SetMountType(RightBoxCollision);
					bValidReturn = true;
				}
				else if (CompName == "FrontBoxCollision")
				{
					SetMountType(FrontBoxCollision);
					bValidReturn = true;
				}
			}
			else
			{
				bValidReturn = false;
			}
		}
	}
	else
	{
		bValidReturn = false;
	}

	return bValidReturn;
}

void URider_Component::SetCollision(ACharacter* Char, ECollisionEnabled::Type Collision)
{
	MulticastSetCollision(Char, Collision);
}

void URider_Component::MulticastSetCollision(ACharacter* Char, ECollisionEnabled::Type Collision)
{
	Char->GetCapsuleComponent()->SetCollisionEnabled(Collision);
}

void URider_Component::SetRide(ACharacter* Char)
{
	Ride = Char;
}

void URider_Component::Attach(ACharacter* CRider, ACharacter* CRide, FName SocketName)
{
	CRider->AttachToComponent(CRide->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false), SocketName);
}

void URider_Component::SetMountLock(bool MountLock)
{
	bMountLock = MountLock;
}

void URider_Component::CollisionSettings()
{
	if (!Ride || !Rider) return;

	Rider->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Rider->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Ride->GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	Ride->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Ride->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}


void URider_Component::SetMountType(int32 MntType)
{
	// 콜리전에 따라 Mount상태 변환
	UAnimInstance* AnimInstance = Rider->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		switch (MntType)
		{
		case LeftBoxCollision:

			MountDelayTime = MountFromLeftLength;
			break;
		case RightBoxCollision:

			MountDelayTime = MountFromRightLength;
			break;
		case FrontBoxCollision:
			const int32 Selection = FMath::RandRange(1, 2);
			if (Selection == 1)
			{
				MountDelayTime = MountFromFrontRightLength;
			}
			else
			{
				MountDelayTime = MountFromFrontLeftLength;
			}
			break;
		}
	}
	
}

int32 URider_Component::GetMountType()
{
	return MountType;
}

UAnimationAsset* URider_Component::GetDismountAnim() const
{
	return DismountAnim;
}

void URider_Component::IsMounted()
{
	// 만약 탑승 상태라면 탑승 체크 bool값을 변경해준다.
	if (bIsMounted)
	{
		bCheckIfMounted = bIsMounted;
		
		// 만약 탈 것이 없다면 값을 다시 돌려 놓는다.
		if (!Ride) ResetVariables();

		RidingSystem = Ride->FindComponentByClass<URiding_System>();

		// 충돌이 난 후 불려지는 함수이니, 캐릭터 부속품의 충돌 설정한다.
		CollisionSettings();

		if (RidingSystem) RidingSystem->bMountLock = true;

		// 해당 마운트 애님 타입을 설정
		SetMountType(MountType);
		// 해당 애님 타입 시퀀스 호출
		PlaySequence(Rider, MountAnim);

		Mount();

		// 마운트 딜레이 타임이 0보다 크면 즉, 애니메이션이 실행된다면
		if (MountDelayTime > 0.f)
		{
			FTimerHandle UnusedHandle;
			GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &URider_Component::MountDelay, MountDelayTime, false);
		}
		else
		{
			MountDelay();
		}
	}

	else
	{
		// False일 시, 라이딩 관련된 모든 변수를 초기화해준다.
		ResetVariables(); 
	}
}

void URider_Component::EventDisMounted()
{
	if (bMountLock)
	{
		Rider->DetachFromActor(FDetachmentTransformRules(EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, EDetachmentRule::KeepWorld, false));
	}
}

void URider_Component::SetIsMounted(bool TraceResult)
{
	// 타고있는 상태로 변환해준다.
	bIsMounted = TraceResult;
	// 그리고 IsMounted를 통해서 타고있는 상태를 만들어준다.
	IsMounted();
}

void URider_Component::EventMount()
{

	if (!Rider || bMountLock) return;

	bMountLock = true;

	SetMountLock(true);

	const bool bTraceResult = Trace();
	
	//만약 트레이스가 true값이 나왔다면
	SetIsMounted(bTraceResult);


}

void URider_Component::SetMovementType(EMovementStates MovStates)
{
	MovementStates = MovStates;
}




