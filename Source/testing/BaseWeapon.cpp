

#include "BaseWeapon.h"
#include "SMPlayerCharacter.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
//#include "ConsoleLog.h"
#include "DrawDebugHelpers.h"

bool DEBUG_WEAPON = false;
#define LOG if(DEBUG_WEAPON) UE_LOG

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
	maxRange = 0;
	weaponID = 0;

	CurrentState = EWeaponState::Idle;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
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
	LOG(LogTemp, Warning, TEXT("OnRep_MyPawn"));
	if (MyPawn) {
		OnEnterInventory(MyPawn);
	} else {
		OnLeaveInventory();
	}
}

void ABaseWeapon::SetOwningPawn(ASMPlayerCharacter* NewOwner) {
	if (MyPawn != NewOwner) {
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		// net owner for RPC calls
		SetOwner(NewOwner);
		LOG(LogTemp, Warning, TEXT("Setting owner of %s to %s on role %d"), *this->GetName(), *NewOwner->GetName(), this->GetLocalRole());
	}
}

void ABaseWeapon::OnEnterInventory(ASMPlayerCharacter* NewOwner)
{
	SetOwningPawn(NewOwner);
}

void ABaseWeapon::OnLeaveInventory()
{
	/*
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}
	*/
	if (GetLocalRole() == ROLE_Authority)
	{
		SetOwningPawn(NULL);
	}
}

//////////////////////////////////////////////////////////////////////////
// Weapon usage

void ABaseWeapon::StartFire() {
	LOG(LogTemp, Warning, TEXT("StartFire on %s as role %d"), *this->GetName(), this->GetLocalRole());
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
	LOG(LogTemp, Warning, TEXT("ServerStartFire"));
	StartFire();
}
bool ABaseWeapon::ServerStartFire_Validate() {
	return true;
}

void ABaseWeapon::StopFire() {
	LOG(LogTemp, Warning, TEXT("StopFire"));
	if (GetLocalRole() < ROLE_Authority) {
		ServerStopFire();
	}

	if (bWantsToFire) {
		LOG(LogTemp, Warning, TEXT("StopFire - bWantsToFire"));
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
	LOG(LogTemp, Warning, TEXT("HandleFiring"));
	if (ammo > 0) {
		//if (GetNetMode() != NM_DedicatedServer) {
		//	SimulateWeaponFire();
		//}

		if (MyPawn && MyPawn->IsLocallyControlled()) {
			LOG(LogTemp, Warning, TEXT("HandleFiring - MyPawn"));
			FireWeapon();
			UseAmmo(ammoPerShot);
		}
	} else {
		Reload();
	}
}
void ABaseWeapon::ServerHandleFiring_Implementation(){
	LOG(LogTemp, Warning, TEXT("ServerHandleFiring"));
	HandleFiring();
}
bool ABaseWeapon::ServerHandleFiring_Validate() {
	return true;
}

void ABaseWeapon::FireWeapon() {
	LOG(LogTemp, Warning, TEXT("FireWeapon"));
	onFire(); //anim stuff here
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
		LOG(LogTemp, Warning, TEXT("FireWeapon - HitResult"));
		// if we're a client and we've hit something that is being controlled by the server
		if (MyPawn && MyPawn->IsLocallyControlled() && GetNetMode() == NM_Client) {
			LOG(LogTemp, Warning, TEXT("FireWeapon - HitResult - OnClient"));
			if (fireTarget.GetActor() && fireTarget.GetActor()->GetRemoteRole() == ROLE_Authority) {
				LOG(LogTemp, Warning, TEXT("FireWeapon - HitResult - OnClient - TargetOnServer"));
				ServerNotifyHit(fireTarget);
			}
		}

		LOG(LogTemp, Warning, TEXT("HitConfirmed after server"));
		HitConfirmed(fireTarget);
	}
}

//TODO actually verify the hit and corresponding hit box
void ABaseWeapon::ServerNotifyHit_Implementation(const FHitResult& fireTarget) {
	LOG(LogTemp, Warning, TEXT("HitConfirmed on server"));
	HitConfirmed(fireTarget);
}
bool ABaseWeapon::ServerNotifyHit_Validate(const FHitResult& fireTarget) {
	return true;
}

void ABaseWeapon::HitConfirmed(const FHitResult& fireTarget) {
	LOG(LogTemp, Warning, TEXT("HitConfirmed"));
	// if we're an actor on the server, or the actor's role is authoritative, we should register damage
	//ASMPlayerCharacter* targetPlayer = Cast<ASMPlayerCharacter>(fireTarget.GetActor()
	if (fireTarget.GetActor() /*targetPlayer && targetPlayer->Health > 0*/) {
		LOG(LogTemp, Warning, TEXT("HitConfirmed - Health>0, NetMode: %d, targetLocalRole: %d"), GetNetMode(), fireTarget.GetActor()->GetLocalRole());
		if (GetNetMode() != NM_Client || fireTarget.GetActor()->GetLocalRole() == ROLE_Authority) {
			LOG(LogTemp, Warning, TEXT("HitConfirmed - Health>0 - OnServer - TargetOnServer"));
			//UGameplayStatics::ApplyDamage(targetPlayer, damage, targetPlayer->GetController(), MyPawn, NULL); //TODO setup damage types
			FPointDamageEvent PointDmg;
			PointDmg.HitInfo = fireTarget;
			PointDmg.Damage = damage;
			fireTarget.GetActor()->TakeDamage(damage, PointDmg, MyPawn->Controller, this);
		}
	}
}

int32 ABaseWeapon::UseAmmo(int32 ammoToUse) {
	LOG(LogTemp, Warning, TEXT("UseAmmo"));
	return ammo -= ammoToUse;
}

void ABaseWeapon::Reload() {
	LOG(LogTemp, Warning, TEXT("Reload"));
	ammo = maxAmmo;
	onReload();
}

//all weapon meshes must be a skeletal mesh for this to work properly.
void ABaseWeapon::toggleVis() {
	skeleMesh->ToggleVisibility();
}
