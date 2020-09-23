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
	Health = 100;
	Armor = 0;
}

void ASMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMPlayerCharacter, Health);
	DOREPLIFETIME(ASMPlayerCharacter, Armor);
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
}

//// DAMAGE STUFF ////



//// WEAPON STUFF ////

void ASMPlayerCharacter::Fire() {
	if (weapon->ammo > 0) {
		weapon->Fire();
		this->PlayAnimMontage(weapon->montage[0]);
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
