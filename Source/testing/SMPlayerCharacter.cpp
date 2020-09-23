// Fill out your copyright notice in the Description page of Project Settings.

//Our imports
#include "SMPlayerCharacter.h"
#include "MathUtil.h"
#include "BaseWeapon.h"
#include "SMGameState.h"
#include "SMPlayerState.h"
#include "SMGameMode.h"
#include "SMCharacterMovementComponent.h"

//Plugin imports
#include "Character/PBPlayerMovement.h"

//Unreal imports
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

ASMPlayerCharacter::ASMPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UPBPlayerMovement>(ACharacter::CharacterMovementComponentName))
{
	//Unreal values
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	//FPS Camera Component
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->bUsePawnControlRotation = true;

	//Property defaults
	Health = MAX_HEALTH;
	Armor = MAX_ARMOR;
}

void ASMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMPlayerCharacter, Health);
	DOREPLIFETIME(ASMPlayerCharacter, Armor);
	DOREPLIFETIME(ASMPlayerCharacter, isHoldingWeapon);
	//DOREPLIFETIME(ASMPlayerCharacter, equippedWeaponID);
}

// Called when the game starts or when spawned
void ASMPlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	//spawn and set weapons
	FActorSpawnParameters Sparam;
	if (HandgunClass != NULL) {
		handgun = GetWorld()->SpawnActor<ABaseWeapon>(HandgunClass, FTransform(FVector(0, 0, 0)), Sparam);
		handgun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
	} else {
		DebugUtil::Error(FString::Printf(TEXT("There is no attached HandgunClass.")), 10);
	}

	if (RocketClass != NULL) {
		rocket_launcher = GetWorld()->SpawnActor<ABaseWeapon>(RocketClass, FTransform(FVector(0, 0, 0)), Sparam);
		rocket_launcher->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
		rocket_launcher->toggleVis();
	} else {
		DebugUtil::Error(FString::Printf(TEXT("There is no attached RocketClass.")), 10);
	}

	//starting weapon
	weapon = handgun;

	FPSCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "headSocket");
}

void ASMPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	DebugUtil::Message(FString::Printf(TEXT("Health: %.1f"), Health), DeltaTime);
}

void ASMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Setup movement bindings
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ASMPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &ASMPlayerCharacter::MoveRight);
	//Setup mouse look bindings
	PlayerInputComponent->BindAxis("LookHorizontal", this, &ASMPlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookVertical", this, &ASMPlayerCharacter::AddControllerPitchInput);
	//Setup jump bindings
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASMPlayerCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Released, this, &ASMPlayerCharacter::StopJumping);
	//Setup swinging bindings
//	PlayerInputComponent->BindAction(FName("FireRope"), IE_Pressed, this, &ASMPlayerCharacter::FireRope);
//	PlayerInputComponent->BindAction(FName("FireRope"), IE_Released, this, &ASMPlayerCharacter::DetachRope);
	//Setup weapon functionality
	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ASMPlayerCharacter::Fire);
	PlayerInputComponent->BindAction(FName("Reload"), IE_Pressed, this, &ASMPlayerCharacter::Reload);
	PlayerInputComponent->BindAction(FName("SwitchWeapon"), IE_Pressed, this, &ASMPlayerCharacter::SwitchWeapon);
	PlayerInputComponent->BindAction(FName("Holster"), IE_Pressed, this, &ASMPlayerCharacter::Holster);
}

//// MOVEMENT STUFF ////

void ASMPlayerCharacter::MoveForward(float value) {
	if (!FMath::IsNearlyZero(value)) {
		AddMovementInput(FQuatRotationMatrix(GetActorQuat()).GetScaledAxis(EAxis::X), value);
	}
}

void ASMPlayerCharacter::MoveRight(float value) {
	if (!FMath::IsNearlyZero(value)) {
		AddMovementInput(FQuatRotationMatrix(GetActorQuat()).GetScaledAxis(EAxis::Y), value);
	}
}

//// DAMAGE STUFF ////

//Armor system from overwatch (if dmg > 10, reduce dmg by 5, else reduce dmg by half)
//Deals damage to this actor after possible armor damage reduction
//Returns the actual damage dealt
float ASMPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) {
	float actualDamage;
	if (Armor > 0) {
		actualDamage = DamageAmount > 10 ? DamageAmount - 5 : FMath::RoundToDouble(DamageAmount / 2.f);
		float newArmor = Armor - actualDamage;
		if (newArmor <= 0) {
			Armor = 0;
			Health -= newArmor;
			//TODO (maybe) MakeNoise: armor destroy
			//TODO MakeNoise: health dmg
		} else {
			Armor = newArmor;
			//TODO MakeNoise: armor dmg
		}
	} else {
		actualDamage = DamageAmount;
		Health -= DamageAmount;
		if (Health <= 0) {
			Die(actualDamage, DamageEvent, EventInstigator, DamageCauser);
		} else {
			//TODO PlayHit (see ShooterCharacter.cpp:421)
		}
		//TODO MakeNoise: health dmg 
	}
	return actualDamage;
}

bool ASMPlayerCharacter::Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser) {
	//if (!CanDie(KillingDamage, DamageEvent, Killer, DamageCauser)) {
	//	return false;
	//}

	//Health = FMath::Min(0.0f, Health);

	//// if this is an environmental death then refer to the previous killer so that they receive credit (knocked into lava pits, etc)
	//UDamageType const* const DamageType = DamageEvent.DamageTypeClass ? DamageEvent.DamageTypeClass->GetDefaultObject<UDamageType>() : GetDefault<UDamageType>();
	//Killer = GetDamageInstigator(Killer, *DamageType);

	//AController* const KilledPlayer = (Controller != NULL) ? Controller : Cast<AController>(GetOwner());
	//GetWorld()->GetAuthGameMode<ASMGameMode>()->Killed(Killer, KilledPlayer, this, DamageType);

	////NetUpdateFrequency = GetDefault<ASMGameMode>()->NetUpdateFrequency;
	//NetUpdateFrequency = GetDefault<AGameMode>()->NetUpdateFrequency;
	//GetCharacterMovement()->ForceReplicationUpdate();

	//OnDeath(KillingDamage, DamageEvent, Killer ? Killer->GetPawn() : NULL, DamageCauser);
	 
	if (IsLocallyControlled()) {
		FString deathMessage = FString::Printf(TEXT("You have been killed."));
		DebugUtil::Message(deathMessage, 5);
	}

	if (GetLocalRole() == ROLE_Authority) {
		FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), Health);
		DebugUtil::Message(healthMessage, 3);
	}

	Respawn(FName("Team1Spawn")); //TODO get team from playerstate/gamestate
	
	return true;
}

bool ASMPlayerCharacter::Respawn(FName playerStartTag) {
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APlayerStart::StaticClass(), playerStartTag, FoundActors);

	if (FoundActors.Num() > 0) {
		FVector spawnLocation = FoundActors[FMath::FRandRange(0, FoundActors.Num())]->GetActorLocation();
		FRotator spawnRotation = FoundActors[FMath::FRandRange(0, FoundActors.Num())]->GetActorRotation();
		Health = MAX_HEALTH;
		Armor = 0.f;
		return TeleportTo(spawnLocation, spawnRotation);
	}
	return false;
}

//// WEAPON STUFF ////

void ASMPlayerCharacter::Fire() {
	if (weapon->ammo > 0) {
		weapon->Fire();
		DebugUtil::Message(FString::Printf(TEXT("Ammo: %d"), weapon->ammo), 2);
		//this->PlayAnimMontage(weapon->montage[0]);
		ShootWeapon();
	} else {
		//TODO makenoise 
		Reload();
	}
}

void ASMPlayerCharacter::Reload() {
	if (weapon->ammo < weapon->maxAmmo) {
		weapon->Reload();
		this->PlayAnimMontage(weapon->montage[1]);
	}
}

void ASMPlayerCharacter::SwitchWeapon() {
	if (weapon == handgun) {
		rocket_launcher->toggleVis();
		handgun->toggleVis();
		weapon = rocket_launcher;
	} else if (weapon == rocket_launcher) {
		rocket_launcher->toggleVis();
		handgun->toggleVis();
		weapon = handgun;
	}
}

void ASMPlayerCharacter::Holster() {
	if (isHoldingWeapon) {
		isHoldingWeapon = false;
	} else {
		isHoldingWeapon = true;
	}
}

void ASMPlayerCharacter::ShootWeapon_Implementation() {
	PlayWeaponFireAnimation(0); //TODO get playerIndex from player state
	FCollisionQueryParams collisionParams;
	collisionParams.bTraceComplex = false;
	collisionParams.AddIgnoredActor(this);
	FVector cameraLoc;
	FRotator cameraRot;
	GetActorEyesViewPoint(cameraLoc, cameraRot);
	FVector endLoc = cameraLoc + (cameraRot.Vector() * 5000); //TODO get max weapon range from weapon
	FHitResult target;

	if (GetWorld()->LineTraceSingleByChannel(target, cameraLoc, endLoc, ECC_Pawn, collisionParams)) {
		//UGameplayStatics::ApplyDamage(target.GetActor(), weapon->damage, GetController(), this, UDamageType::StaticClass());
		UGameplayStatics::ApplyDamage(target.GetActor(), weapon->damage, GetController(), this, UDamageType::StaticClass()); //TODO get dmg and dmg type from weapon
	}
}
bool ASMPlayerCharacter::ShootWeapon_Validate() {
	return true;
}

void ASMPlayerCharacter::PlayWeaponFireAnimation_Implementation(int8 playerIndex) {
	if (IsLocallyControlled()) {
		if (GetController() != UGameplayStatics::GetPlayerController(GetWorld(), playerIndex)) {
			this->PlayAnimMontage(weapon->montage[0]);
		}
	}
}
bool ASMPlayerCharacter::PlayWeaponFireAnimation_Validate(int8 playerIndex) {
	return true;
}
