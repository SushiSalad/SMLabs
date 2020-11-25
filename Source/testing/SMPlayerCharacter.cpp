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
#include "EngineUtils.h"
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
}

void ASMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMPlayerCharacter, Health);
	DOREPLIFETIME(ASMPlayerCharacter, Armor);
	DOREPLIFETIME(ASMPlayerCharacter, isHoldingWeapon);
	DOREPLIFETIME(ASMPlayerCharacter, CurrentWeapon);
}

// Called when the game starts or when spawned
void ASMPlayerCharacter::BeginPlay() {
	Super::BeginPlay();

	//set health values
	Health = MAX_HEALTH;
	Armor = MAX_ARMOR;

	//spawn and set weapons
	FActorSpawnParameters Sparam;
	if (HandgunClass != NULL) {
		handgun = GetWorld()->SpawnActor<ABaseWeapon>(HandgunClass, FTransform(FVector(0, 0, 0)), Sparam);
		handgun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
		//handgun->SetOwningPawn(this);
	} else {
		DebugUtil::Error(FString::Printf(TEXT("There is no attached HandgunClass.")), 10);
	}

	if (RocketClass != NULL) {
		rocket_launcher = GetWorld()->SpawnActor<ABaseWeapon>(RocketClass, FTransform(FVector(0, 0, 0)), Sparam);
		rocket_launcher->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
		rocket_launcher->toggleVis();
		//rocket_launcher->SetOwningPawn(this);
	} else {
		DebugUtil::Error(FString::Printf(TEXT("There is no attached RocketClass.")), 10);
	}

	FPSCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "headSocket");
	
	//starting weapon
	if (GetLocalRole() < ROLE_Authority) {
		return;
	}
	SetCurrentWeapon(handgun);
}

void ASMPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (GetLocalRole() < ROLE_Authority) {
		DebugUtil::Message(FString::Printf(TEXT("Client - Health: %.1f, Armor: %.1f"), Health, Armor), DeltaTime);
	} else {
		DebugUtil::Message(FString::Printf(TEXT("Server - Health: %.1f, Armor: %.1f"), Health, Armor), DeltaTime);
	}
	
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
	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ASMPlayerCharacter::StartWeaponFire);
	PlayerInputComponent->BindAction(FName("Fire"), IE_Released, this, &ASMPlayerCharacter::StopWeaponFire);
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
	UE_LOG(LogTemp, Warning, TEXT("Take Damage | Damage: %f, Damaged:%s, Causer: %s"), DamageAmount, *this->GetName(), *DamageCauser->GetName());
	float actualDamage;
	if (Armor > 0) {
		actualDamage = DamageAmount > 10 ? DamageAmount - 5 : FMath::RoundToDouble(DamageAmount / 2.f);
		float newArmor = Armor - actualDamage;
		if (newArmor <= 0) {
			//UE_LOG(LogTemp, Warning, TEXT("Take Armor Damage"));
			Armor = 0;
			Health += newArmor; //will be negative, so i add it
			//TODO (maybe) MakeNoise: armor destroy
			//TODO MakeNoise: health dmg
		} else {
			//UE_LOG(LogTemp, Warning, TEXT("Take Health Damage after armor"));
			Armor = newArmor;
			//TODO MakeNoise: armor dmg
		}
	} else {
		//UE_LOG(LogTemp, Warning, TEXT("Take Health Damage"));
		actualDamage = DamageAmount;
		Health -= DamageAmount;
		if (Health <= 0) {
			UE_LOG(LogTemp, Warning, TEXT("Player should have died"));
			Die(actualDamage, DamageEvent, EventInstigator, DamageCauser);
		} else {
			//TODO PlayHit (see ShooterCharacter.cpp:421)
		}
		//TODO MakeNoise: health dmg 
	}
	return actualDamage;
}

//TODO(delle) add gamemode kill functionality and scoring
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
	if (GetLocalRole() == ROLE_Authority) {
		Respawn(); //TODO get team from playerstate/gamestate
	}

	return true;
}

//TODO(delle) check for preferred/optimal spawn points
bool ASMPlayerCharacter::Respawn() {
	UE_LOG(LogTemp, Warning, TEXT("Player should have respawned"));
	TArray<APlayerStart*> FoundActors;
	for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It) {
		APlayerStart* Spawn = *It;
		FoundActors.Add(Spawn);
	}
	UE_LOG(LogTemp, Warning, TEXT("Found %d places to respawn"), FoundActors.Num());
	if (FoundActors.Num() > 0) {
		FVector spawnLocation = FoundActors[FMath::FRandRange(0, FoundActors.Num())]->GetActorLocation();
		FRotator spawnRotation = FoundActors[FMath::FRandRange(0, FoundActors.Num())]->GetActorRotation();
		Health = MAX_HEALTH;
		Armor = 0.f;
		UE_LOG(LogTemp, Warning, TEXT("Player should be teleported"));
		return TeleportTo(spawnLocation, spawnRotation);
	}
	return false;
}

//// WEAPON STUFF ////

void ASMPlayerCharacter::StartWeaponFire() {
	if (!bWantsToFire) {
		bWantsToFire = true;
		if (CurrentWeapon) {
			UE_LOG(LogTemp, Warning, TEXT("StartFire from %s as role %d"), *this->GetName(), this->GetLocalRole());
			CurrentWeapon->StartFire();
			this->PlayAnimMontage(CurrentWeapon->montage[0]);
		}
	}
}

void ASMPlayerCharacter::StopWeaponFire() {
	if (bWantsToFire) {
		bWantsToFire = false;
		if (CurrentWeapon) {
			CurrentWeapon->StopFire();
		}
	}
}

void ASMPlayerCharacter::Reload() {
	if (CurrentWeapon->ammo < CurrentWeapon->maxAmmo) {
		CurrentWeapon->Reload();
		this->PlayAnimMontage(CurrentWeapon->montage[1]);
	}
}

void ASMPlayerCharacter::SwitchWeapon() {
	if (CurrentWeapon == handgun) {
		rocket_launcher->toggleVis();
		handgun->toggleVis();
		CurrentWeapon = rocket_launcher;
	} else if (CurrentWeapon == rocket_launcher) {
		rocket_launcher->toggleVis();
		handgun->toggleVis();
		CurrentWeapon = handgun;
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
		UGameplayStatics::ApplyDamage(target.GetActor(), CurrentWeapon->damage, GetController(), this, UDamageType::StaticClass());
	}
}
bool ASMPlayerCharacter::ShootWeapon_Validate() {
	return true;
}

void ASMPlayerCharacter::PlayWeaponFireAnimation_Implementation(int8 playerIndex) {
	if (IsLocallyControlled()) {
		if (GetController() != UGameplayStatics::GetPlayerController(GetWorld(), playerIndex)) {
			this->PlayAnimMontage(CurrentWeapon->montage[0]);
		}
	}
}
bool ASMPlayerCharacter::PlayWeaponFireAnimation_Validate(int8 playerIndex) {
	return true;
}

//// Inventory Stuff ////

void ASMPlayerCharacter::OnRep_CurrentWeapon(ABaseWeapon* LastWeapon){
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void ASMPlayerCharacter::SetCurrentWeapon(ABaseWeapon* NewWeapon, ABaseWeapon* LastWeapon){
	if (NewWeapon) {
		ABaseWeapon* LocalLastWeapon = nullptr;
		if (LastWeapon != NULL)
		{
			LocalLastWeapon = LastWeapon;
		} else if (NewWeapon != CurrentWeapon)
		{
			LocalLastWeapon = CurrentWeapon;
		}

		// unequip previous
		if (LocalLastWeapon)
		{
			//LocalLastWeapon->OnUnEquip();
		}

		CurrentWeapon = NewWeapon;

		// equip new one
		NewWeapon->SetOwningPawn(this);	// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		//NewWeapon->OnEquip(LastWeapon);
	}
}

void ASMPlayerCharacter::EquipWeapon(ABaseWeapon* Weapon){
	if (Weapon){
		if (GetLocalRole() == ROLE_Authority){
			SetCurrentWeapon(Weapon, CurrentWeapon);
		} else {
			ServerEquipWeapon(Weapon);
		}
	}
}
void ASMPlayerCharacter::ServerEquipWeapon_Implementation(ABaseWeapon* Weapon){
	EquipWeapon(Weapon);
}
bool ASMPlayerCharacter::ServerEquipWeapon_Validate(ABaseWeapon* Weapon){
	return true;
}