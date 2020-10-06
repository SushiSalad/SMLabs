// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "SMPlayerCharacter.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
//#include "ConsoleLog.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABaseWeapon::ABaseWeapon() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ammo = 0;
	maxAmmo = 0;
	bulletSpread = 0;
	fireRate = 0;
	damage = 0;
	reloadSpeed = 0;
	weight = 0;
	ammoPerShot = 0;

}

//////////////////////////////////////////////////////////////////////////
// Unreal utility

void ABaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseWeapon, MyPawn);
	DOREPLIFETIME_CONDITION(ABaseWeapon, ammo, COND_OwnerOnly);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

//////////////////////////////////////////////////////////////////////////
// Pawn Ownership stuff

// on update pawn, add/remove from pawns inventory
void ABaseWeapon::OnRep_MyPawn() {
	UE_LOG(LogTemp, Warning, TEXT("OnRep_MyPawn"));
	if (MyPawn) {
		//OnEnterInventory(MyPawn);
	} else {
		//OnLeaveInventory();
	}
}

void ABaseWeapon::SetOwningPawn(ASMPlayerCharacter* NewOwner) {
	if (MyPawn != NewOwner) {
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
	}
}



//////////////////////////////////////////////////////////////////////////
// Weapon usage

void ABaseWeapon::StartFire() {
	if (GetLocalRole() < ROLE_Authority) {
		ServerStartFire();
	}

	if (!bWantsToFire) {
		bWantsToFire = true;
		//DetermineWeaponState(); //TODO setup weapon states
		CurrentState = EWeaponState::Firing;
		HandleFiring();
	}
}
void ABaseWeapon::ServerStartFire_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("ServerStartFire"));
	StartFire();
}
bool ABaseWeapon::ServerStartFire_Validate() {
	return true;
}

void ABaseWeapon::StopFire() {
	//UE_LOG(LogTemp, Warning, TEXT("StopFire"));
	if (GetLocalRole() < ROLE_Authority) {
		ServerStopFire();
	}

	if (bWantsToFire) {
		//UE_LOG(LogTemp, Warning, TEXT("StopFire - bWantsToFire"));
		bWantsToFire = false;
		//DetermineWeaponState(); //TODO setup weapon states
		CurrentState = EWeaponState::Idle;
		//TODO stop anims
	}
}
void ABaseWeapon::ServerStopFire_Implementation() {
	StopFire();
}
bool ABaseWeapon::ServerStopFire_Validate() {
	return true;
}

void ABaseWeapon::HandleFiring() {
	//UE_LOG(LogTemp, Warning, TEXT("HandleFiring"));
	if (ammo > 0) {
		//if (GetNetMode() != NM_DedicatedServer) {
		//	SimulateWeaponFire();
		//}

		if (MyPawn && MyPawn->IsLocallyControlled()) {
			//UE_LOG(LogTemp, Warning, TEXT("HandleFiring - MyPawn"));
			FireWeapon();
			UseAmmo(ammoPerShot);
		}
	} else {
		Reload();
	}
}
void ABaseWeapon::ServerHandleFiring_Implementation(){
	UE_LOG(LogTemp, Warning, TEXT("ServerHandleFiring"));
	HandleFiring();
}
bool ABaseWeapon::ServerHandleFiring_Validate() {
	return true;
}

void ABaseWeapon::FireWeapon() {
	//UE_LOG(LogTemp, Warning, TEXT("FireWeapon"));
	onFire();
	//anim stuff here
	FCollisionQueryParams collisionParams;
	collisionParams.bTraceComplex = true;
	collisionParams.AddIgnoredActor(this);
	collisionParams.AddIgnoredActor(MyPawn);

	FVector cameraLoc;
	FRotator cameraRot;
	MyPawn->GetActorEyesViewPoint(cameraLoc, cameraRot);
	FVector end = cameraLoc + (cameraRot.Vector() * weaponRange);

	FHitResult fireTarget(ForceInit);
	DrawDebugLine(GetWorld(), GetActorLocation(), end, FColor::Red, false, 2, 0, 2);
	if (GetWorld()->LineTraceSingleByChannel(fireTarget, cameraLoc, end, ECC_Pawn, collisionParams)) {
		//UE_LOG(LogTemp, Warning, TEXT("FireWeapon - HitResult"));
		ASMPlayerCharacter* targetPlayer = Cast<ASMPlayerCharacter>(fireTarget.GetActor());
		if(targetPlayer && targetPlayer->Health > 0){
			//UE_LOG(LogTemp, Warning, TEXT("FireWeapon - HitResult - Health>0"));
			UGameplayStatics::ApplyDamage(targetPlayer, damage, targetPlayer->GetController(), MyPawn, NULL); //TODO setup damage types
		}
	}
}

int32 ABaseWeapon::UseAmmo(int32 ammoToUse) {
	//UE_LOG(LogTemp, Warning, TEXT("UseAmmo"));
	return ammo -= ammoToUse;
}

void ABaseWeapon::Reload() {
	UE_LOG(LogTemp, Warning, TEXT("Reload"));
	ammo = maxAmmo;
	onReload();
}

//all weapon meshes must be a skeletal mesh for this to work properly.
void ABaseWeapon::toggleVis() {
	skeleMesh->ToggleVisibility();
}


