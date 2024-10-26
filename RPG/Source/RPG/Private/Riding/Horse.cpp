
#include "Riding/Horse.h"
#include "Engine/World.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"


AHorse::AHorse()
{
	PrimaryActorTick.bCanEverTick = true;


	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.0f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	FrontBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("FrontBoxCollision"));
	FrontBoxCollision->SetupAttachment(GetMesh());

	LeftBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftBoxCollision"));
	LeftBoxCollision->SetupAttachment(GetMesh());

	RightBoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightBoxCollision"));
	RightBoxCollision->SetupAttachment(GetMesh());

	RiderLocation = CreateDefaultSubobject<USceneComponent>(TEXT("RiderLocation"));
	RiderLocation->SetupAttachment(GetMesh());

	RidingSystem = CreateDefaultSubobject<URiding_System>(TEXT("RidingSystem"));

	MaxWalkSpeed = 1600.f;
	
	bIsMoving = false;
	bIsBoosting = false;
}

float AHorse::SetRiderSpeed()
{
	return ForwardWalkSpeed;
}

void AHorse::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHorse::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		bIsMoving = true;

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);


		if (Value < 0.f)
		{
			GetCharacterMovement()->MaxWalkSpeed = BackWalkSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else if (Value > 0.f)
		{
			GetCharacterMovement()->MaxWalkSpeed = ForwardWalkSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}

	}

	else if (Value == 0.f)
	{
		bIsMoving = false;
		ResetWalkSpeed();
	}
}

void AHorse::MoveRight(float Value)
{
	if (Controller && (Value != 0.f))
	{
		bIsMoving = true;

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		if (Value < 0.f)
		{
			GetCharacterMovement()->MaxWalkSpeed = RightWalkSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else if (Value > 0.f)
		{
			GetCharacterMovement()->MaxWalkSpeed = RightWalkSpeed;
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
	}

	else if (Value == 0.f)
	{
		bIsMoving = false;
	}
}

void AHorse::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AHorse::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AHorse::IncreaseMotions()
{
	if (RidingSystem)
	{
		if (ForwardWalkSpeed < MaxWalkSpeed)
		{
			ForwardWalkSpeed += 400.f;
		}
	}
}

void AHorse::ResetWalkSpeed()
{
	RightWalkSpeed = 300.f;
	BackWalkSpeed = 300.f;
	ForwardWalkSpeed = 300.f;
}


void AHorse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHorse::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AHorse::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AHorse::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AHorse::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AHorse::LookUp);

	PlayerInputComponent->BindAction(FName("IncreaseMotions"), IE_Pressed, this, &AHorse::IncreaseMotions);
}

