// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/SphereComponent.h"

#include "Item.h"
#include "Items/Weapon.h"
#include "items/Shield.h"
#include "Items/Treasure.h"
#include "Items/Soul.h"

#include "Animation/AnimMontage.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"




ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.0f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 350.0f;
	
	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	DodgeHelper = CreateDefaultSubobject<USphereComponent>(TEXT("DodgeHelper"));
	DodgeHelper->SetupAttachment(GetRootComponent());
	FVector DodgeHelpersetLocation = { 550.f, 0.f, 0.f };
	DodgeHelper->SetWorldLocation(DodgeHelpersetLocation);

	SprintSpeedMultiplier = 3.0f;

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASlashCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASlashCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ASlashCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ASlashCharacter::LookUp);


	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASlashCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &ASlashCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &ASlashCharacter::Attack);
	PlayerInputComponent->BindAction(FName("Dodge"), IE_Pressed, this, &ASlashCharacter::Dodge);
	//PlayerInputComponent->BindAction(FName("Crouch"), IE_Pressed, this, &ASlashCharacter::Crouch);
	//PlayerInputComponent->BindAction(FName("Sprint"), IE_Pressed, this, &ASlashCharacter::Sprint);
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);
	if (Attributes && Attributes->GetHealthPercent() > 0.f)
	{
		ActionState = EActionState::EAS_HitReaction;
	}
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddSouls(Soul->GetSouls());
		SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && SlashOverlay)
	{
		Attributes->AddGold(Treasure->GetGold());
		SlashOverlay->SetGold(Attributes->GetGold());
	}
}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("PlayerTeam"));
	InitializeSlashOverlay();
}

void ASlashCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Attacking) return;
	if (Controller && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied || ActionState == EActionState::EAS_Attacking) return;
	if (Controller && (Value != 0.f))
	{
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void ASlashCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ASlashCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon)
	{
		if (IsWeapon)
		{
			EquippedWeapon->Destroy();
		}
		EquipWeapon(OverlappingWeapon);
	}
	else
	{
		if (IsShield)
		{
			if (CanArm())
			{
				PlayEquipMontage(FName("Equip"));
				IsWeapon = true;
				IsWeaponState();
				ActionState = EActionState::EAS_EquippingWeapon;
			}
			else if (CanDisarm())
			{
				PlayEquipMontage(FName("Unequip"));
				IsWeapon = false;
				IsWeaponState();
				ActionState = EActionState::EAS_EquippingWeapon;
			}
		}
		if (!IsShield)
		{
			if (CanDisarm())
			{
				PlayEquipMontage(FName("Unequip"));
				IsWeapon = false;
				IsWeaponState();
				ActionState = EActionState::EAS_EquippingWeapon;
			}
			else if (CanArm())
			{
				PlayEquipMontage(FName("Equip"));
				IsWeapon = true;
				IsWeaponState();
				ActionState = EActionState::EAS_EquippingWeapon;
			}
		}
	}
	AShield* OverlappingShield = Cast<AShield>(OverlappingItem);
	if (OverlappingShield && OverlappingShield->PickedUp())
	{
		OverlappingShield->Equip(GetMesh(), FName("LeftHand_Socket"));
		IsShield = true;
		IsWeaponState();
		OverlappingShield = nullptr;
		EquippedShield = OverlappingShield;
	}
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	if (OverlappingWeapon && OverlappingWeapon->PickedUp())
	{
		Weapon->Equip(GetMesh(), FName("RightHand_Socket"), this, this);
		CharacterEquipState = ECharacterEquipState::ECS_EquippedOneHandeWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = Weapon;
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Attributes && SlashOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		PPlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool ASlashCharacter::CanAttack()
{
	return 	ActionState == EActionState::EAS_Unoccupied &&
		CharacterEquipState != ECharacterEquipState::ECS_Unequipped;
}

void ASlashCharacter::Dodge()
{
	if (IsOccupied() && !HasEnoughStamina()) return;
		
	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;
	if (Attributes && SlashOverlay)
	{
		Attributes->UseStamina(Attributes->GetDodgeCost());
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
	}
}

void ASlashCharacter::DodgeEnd()
{
	Super::DodgeEnd();
;
	//SetActorLocation(DodgeHelper->GetComponentLocation());

	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::Crouch()
{

}

void ASlashCharacter::Sprint()
{

}


void ASlashCharacter::PPlayAttackMontage()
{
	//Super::PlayAttackMontage();
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		int32 Selection = currentATK;
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			//UE_LOG(LogTemp, Log, TEXT("%d"), currentATK);
			SectionName = FName("Attack1");
			currentATK++;
			break;
		case 1:
			SectionName = FName("Attack2");
			currentATK++;
			break;
		case 2:
			SectionName = FName("Attack3");
			currentATK++;
			break;
		case 3:
			SectionName = FName("Attack4");
			currentATK = 0;
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

bool ASlashCharacter::CanDisarm()
{
	if (ActionState == EActionState::EAS_Unoccupied &&
		CharacterEquipState != ECharacterEquipState::ECS_Unequipped)
	{
		return true;
	}
	else if (ActionState == EActionState::EAS_Unoccupied &&
		CharacterEquipState != ECharacterEquipState::ECS_EquippedTwoHandedWeapon
		&& IsShield)
	{
		return true;
	}
	return false;
}

bool ASlashCharacter::CanArm()
{
	if (ActionState == EActionState::EAS_Unoccupied &&
		CharacterEquipState == ECharacterEquipState::ECS_Unequipped &&
		EquippedWeapon)
	{
		return true;
	}
	else if (ActionState == EActionState::EAS_Unoccupied &&
		CharacterEquipState == ECharacterEquipState::ECS_EquippedOneHandeWeapon &&
		EquippedWeapon)
	{
		return true;
	}
	return false;
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("ClavicleSocket"));
	}
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHand_Socket"));
	}
}

void ASlashCharacter::ShieldPickUp()
{
}

void ASlashCharacter::ShieldBack()
{
}

void ASlashCharacter::HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool ASlashCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ASlashCharacter::InitializeSlashOverlay()
{
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		TObjectPtr<ASlashHUD> SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD());
		if (SlashHUD)
		{
			SlashOverlay = SlashHUD->GetSlashOverlay();
			if (SlashOverlay && Attributes)
			{
				SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
				SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
				SlashOverlay->SetGold(0);
				SlashOverlay->SetSouls(0);
			}
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
	{
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
	}
}

void ASlashCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::IsWeaponState()
{
	if (IsWeapon && IsShield)
	{
		CharacterEquipState = ECharacterEquipState::ECS_EquippedTwoHandedWeapon;
	}
	else if ((IsWeapon && !IsShield) || (!IsWeapon && IsShield))
	{
		CharacterEquipState = ECharacterEquipState::ECS_EquippedOneHandeWeapon;
	}
	else if (!IsWeapon && !IsShield)
	{
		CharacterEquipState = ECharacterEquipState::ECS_Unequipped;
	}
}

void ASlashCharacter::IsRidingState()
{

}

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool ASlashCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->GetStamina() > Attributes->GetDodgeCost();
}

bool ASlashCharacter::IsOccupied()
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}




