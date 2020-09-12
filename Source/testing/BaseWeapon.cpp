// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"
#include "GameFramework/Character.h"

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

	//.onFire.AddDynamic(this, Fire());
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ABaseWeapon::Fire() {
	ammo--;
	onFire();
}


void ABaseWeapon::Reload() {
	ammo = maxAmmo;
	onReload();
}

//all weapon meshes must be a skeletal mesh for this to work properly.

void ABaseWeapon::toggleVis() {
	skeleMesh->ToggleVisibility();
}


