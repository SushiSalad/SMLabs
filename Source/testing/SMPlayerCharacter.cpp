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

	MaxRopeDistance = 50000;
	RopePullSpeed = 10;
	ropeFired = false;
	coolDown = 3;
}

void ASMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMPlayerCharacter, Health);
	DOREPLIFETIME(ASMPlayerCharacter, Armor);
	DOREPLIFETIME(ASMPlayerCharacter, isHoldingWeapon);
	DOREPLIFETIME_CONDITION(ASMPlayerCharacter, coolingDown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASMPlayerCharacter, coolDownElapsed, COND_OwnerOnly);
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
		handgun->SetOwningPawn(this);
	} else {
		DebugUtil::Error(FString::Printf(TEXT("There is no attached HandgunClass.")), 10);
	}

	if (RocketClass != NULL) {
		rocket_launcher = GetWorld()->SpawnActor<ABaseWeapon>(RocketClass, FTransform(FVector(0, 0, 0)), Sparam);
		rocket_launcher->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
		rocket_launcher->toggleVis();
		rocket_launcher->SetOwningPawn(this);
	} else {
		DebugUtil::Error(FString::Printf(TEXT("There is no attached RocketClass.")), 10);
	}

	//starting weapon
	weapon = handgun;

	FPSCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "headSocket");
}

void ASMPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	RopeStuff(DeltaTime);
	UpdateCoolDown(DeltaTime);
	//if (coolingDown) { UE_LOG(LogTemp, Warning, TEXT("cooling down on %s"), *this->GetName()); }
	DebugUtil::Message(FString::Printf(TEXT("Health: %.1f, Armor: %.1f"), Health, Armor), DeltaTime);
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
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Pressed, this, &ASMPlayerCharacter::FireRope);
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Released, this, &ASMPlayerCharacter::DetachRope);
	//Sliding
	PlayerInputComponent->BindAction(FName("StartSliding"), IE_Pressed, this, &ASMPlayerCharacter::StartSliding);
	PlayerInputComponent->BindAction(FName("StartSliding"), IE_Released, this, &ASMPlayerCharacter::StopSliding);
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

void ASMPlayerCharacter::StartWeaponFire() {
	UE_LOG(LogTemp, Warning, TEXT("fire"));
	if (!bWantsToFire) {
		bWantsToFire = true;
		if (weapon) {
			weapon->StartFire();
			this->PlayAnimMontage(weapon->montage[0]);
		}
	}
}

void ASMPlayerCharacter::StopWeaponFire() {
	if (bWantsToFire) {
		bWantsToFire = false;
		if (weapon) {
			weapon->StopFire();
		}
	}
}

//void ASMPlayerCharacter::Fire() {
//	if (weapon->ammo > 0) {
//		weapon->Fire();
//		DebugUtil::Message(FString::Printf(TEXT("Ammo: %d"), weapon->ammo), 2);
//		//this->PlayAnimMontage(weapon->montage[0]);
//		ShootWeapon();
//	} else {
//		//TODO makenoise 
//		Reload();
//	}
//}

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
		UGameplayStatics::ApplyDamage(target.GetActor(), weapon->damage, GetController(), this, UDamageType::StaticClass());
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

//// Rope Stuff ////

void ASMPlayerCharacter::FireRope() {
	if (GetLocalRole() < ROLE_Authority) {
		UE_LOG(LogTemp, Warning, TEXT("FireRope_Local"));
		ServerFireRope();
	}

	if (!ropeFired && !coolingDown) {
		ropeFired = true;
		FCollisionQueryParams collisionParams;
		collisionParams.bTraceComplex = true;
		collisionParams.AddIgnoredActor(this);

		FVector cameraLoc;
		FRotator cameraRot;
		GetActorEyesViewPoint(cameraLoc, cameraRot);
		FVector end = cameraLoc + (cameraRot.Vector() * MaxRopeDistance);

		if (GetWorld()->LineTraceSingleByChannel(ropeTarget, cameraLoc, end, ECC_WorldStatic, collisionParams)) {
			//DebugUtil::Message(FString::Printf(TEXT("Collision on %s at %s" ), *ropeTarget.GetActor()->GetActorLabel(), *ropeTarget.ImpactPoint.ToString()), 10);
			ropeAttached = true;
			coolingDown = true;
			UE_LOG(LogTemp, Warning, TEXT("RopeAttached."));
		}
	}
}

void ASMPlayerCharacter::ServerFireRope_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("FireRope_Server"));
	FireRope();
}

bool ASMPlayerCharacter::ServerFireRope_Validate() {
	//will need actual validation later
	return true;
}

void ASMPlayerCharacter::RopeStuff(float DeltaTime) {
	if (ropeAttached && coolDownElapsed < 0.5) {
		UPBPlayerMovement* m = Cast<UPBPlayerMovement>(this->GetMovementComponent());
		UE_LOG(LogTemp, Warning, TEXT("RopeStuff_Local"));
		if (GetLocalRole() < ROLE_Authority) {
			ServerRopeStuff(DeltaTime);
			//DebugUtil::Message(FString::Printf(TEXT("COOLDOWN ON CLIENT: %f"), coolDownElapsed), DeltaTime);
		}
		if (coolDownElapsed < 0.5) {
			m->bServerAcceptClientAuthoritativePosition = 1;
			m->bIgnoreClientMovementErrorChecksAndCorrection = 1;
			DrawDebugLine(GetWorld(), GetActorLocation(), ropeTarget.ImpactPoint, FColor::Red, false, -1.0F, 0, 2);
			this->LaunchCharacter((ropeTarget.ImpactPoint - GetActorLocation()).GetSafeNormal() * RopePullSpeed, false, false);
		}
		else {
			m->bServerAcceptClientAuthoritativePosition = 0;
			m->bIgnoreClientMovementErrorChecksAndCorrection = 0;
			DetachRope();
		}
	}
}

void ASMPlayerCharacter::ServerRopeStuff_Implementation(float DeltaTime) {
	UE_LOG(LogTemp, Warning, TEXT("RopeStuff_Server"));
	RopeStuff(DeltaTime);
}

bool ASMPlayerCharacter::ServerRopeStuff_Validate(float DeltaTime) {
	return true;
}

void ASMPlayerCharacter::DetachRope() {
	

	if (GetLocalRole() < ROLE_Authority) {
		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~DetachRope_Local~~~~~~~~~~~~~~~"));
		ServerDetachRope();
	}

	ropeFired = false;
	ropeAttached = false;
	ropeTarget.Reset();
}

void ASMPlayerCharacter::ServerDetachRope_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("DetachRope_Server"));
	DetachRope();
}
bool ASMPlayerCharacter::ServerDetachRope_Validate() {
	return true;
}

void ASMPlayerCharacter::StartSliding() {
	if(GetLocalRole() < ROLE_Authority) {
		ServerStartSliding();
	}

	UPBPlayerMovement* m = Cast<UPBPlayerMovement>(this->GetMovementComponent());
	m->GroundFriction = 0;

}

void ASMPlayerCharacter::ServerStartSliding_Implementation() {
	StartSliding();
}

bool ASMPlayerCharacter::ServerStartSliding_Validate() {
	return true;
}

void ASMPlayerCharacter::StopSliding() {
	if (GetLocalRole() < ROLE_Authority) {
		ServerStopSliding();
	}

	UPBPlayerMovement* m = Cast<UPBPlayerMovement>(this->GetMovementComponent());
	m->GroundFriction = 4;

}

void ASMPlayerCharacter::ServerStopSliding_Implementation() {
	StopSliding();
}

bool ASMPlayerCharacter::ServerStopSliding_Validate() {
	return true;
}

void ASMPlayerCharacter::UpdateCoolDown(float DeltaTime, bool cool) {
	if (coolingDown) {
		if (GetLocalRole() < ROLE_Authority) {
			ServerUpdateCoolDown(DeltaTime);
		}
	}
}

void ASMPlayerCharacter::ServerUpdateCoolDown_Implementation(float DeltaTime, bool cool) {
	if (coolingDown) {
		DebugUtil::Message(FString::Printf(TEXT("Cooldown: %f on %s"), coolDownElapsed, *this->GetName()), DeltaTime);
		coolDownElapsed += DeltaTime;
		if (coolDownElapsed > coolDown) { 
			coolingDown = false;
			coolDownElapsed = 0;
		}
		//UpdateCoolDown(DeltaTime);
	}
	
}

bool ASMPlayerCharacter::ServerUpdateCoolDown_Validate(float DeltaTime, bool cool) {
	return true;
}

