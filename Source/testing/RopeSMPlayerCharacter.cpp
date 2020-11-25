// Fill out your copyright notice in the Description page of Project Settings.

#include "RopeSMPlayerCharacter.h"

//Our imports
#include "SMGameMode.h"
#include "MathUtil.h"

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

ARopeSMPlayerCharacter::ARopeSMPlayerCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<UPBPlayerMovement>(ACharacter::CharacterMovementComponentName)) {

	MaxRopeDistance = 50000;
	RopePullSpeed = 10;
	ropeFired = false;
	coolDown = 3;
}

void ARopeSMPlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	FPSCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "headSocket");

}

void ARopeSMPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ARopeSMPlayerCharacter, coolingDown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ARopeSMPlayerCharacter, coolDownElapsed, COND_OwnerOnly);
	//DOREPLIFETIME(ARopeSMPlayerCharacter, equippedWeaponID);
}

void ARopeSMPlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	RopeStuff(DeltaTime);
	UpdateCoolDown(DeltaTime);
}

void ARopeSMPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Setup swinging bindings
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Pressed, this, &ARopeSMPlayerCharacter::FireRope);
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Released, this, &ARopeSMPlayerCharacter::DetachRope);
	//Sliding
	PlayerInputComponent->BindAction(FName("StartSliding"), IE_Pressed, this, &ARopeSMPlayerCharacter::StartSliding);
	PlayerInputComponent->BindAction(FName("StartSliding"), IE_Released, this, &ARopeSMPlayerCharacter::StopSliding);
}

//// Rope Stuff ////

void ARopeSMPlayerCharacter::FireRope() {
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

void ARopeSMPlayerCharacter::ServerFireRope_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("FireRope_Server"));
	FireRope();
}

bool ARopeSMPlayerCharacter::ServerFireRope_Validate() {
	//will need actual validation later
	return true;
}

void ARopeSMPlayerCharacter::RopeStuff(float DeltaTime) {
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
		} else {
			m->bServerAcceptClientAuthoritativePosition = 0;
			m->bIgnoreClientMovementErrorChecksAndCorrection = 0;
			DetachRope();
		}
	}
}

void ARopeSMPlayerCharacter::ServerRopeStuff_Implementation(float DeltaTime) {
	UE_LOG(LogTemp, Warning, TEXT("RopeStuff_Server"));
	RopeStuff(DeltaTime);
}

bool ARopeSMPlayerCharacter::ServerRopeStuff_Validate(float DeltaTime) {
	return true;
}

void ARopeSMPlayerCharacter::DetachRope() {


	if (GetLocalRole() < ROLE_Authority) {
		UE_LOG(LogTemp, Warning, TEXT("~~~~~~~~~~~~DetachRope_Local~~~~~~~~~~~~~~~"));
		ServerDetachRope();
	}

	ropeFired = false;
	ropeAttached = false;
	ropeTarget.Reset();
}

void ARopeSMPlayerCharacter::ServerDetachRope_Implementation() {
	UE_LOG(LogTemp, Warning, TEXT("DetachRope_Server"));
	DetachRope();
}
bool ARopeSMPlayerCharacter::ServerDetachRope_Validate() {
	return true;
}

void ARopeSMPlayerCharacter::StartSliding() {
	if (GetLocalRole() < ROLE_Authority) {
		ServerStartSliding();
	}

	UPBPlayerMovement* m = Cast<UPBPlayerMovement>(this->GetMovementComponent());
	m->GroundFriction = 0;

}

void ARopeSMPlayerCharacter::ServerStartSliding_Implementation() {
	StartSliding();
}

bool ARopeSMPlayerCharacter::ServerStartSliding_Validate() {
	return true;
}

void ARopeSMPlayerCharacter::StopSliding() {
	if (GetLocalRole() < ROLE_Authority) {
		ServerStopSliding();
	}

	UPBPlayerMovement* m = Cast<UPBPlayerMovement>(this->GetMovementComponent());
	m->GroundFriction = 4;

}

void ARopeSMPlayerCharacter::ServerStopSliding_Implementation() {
	StopSliding();
}

bool ARopeSMPlayerCharacter::ServerStopSliding_Validate() {
	return true;
}

void ARopeSMPlayerCharacter::UpdateCoolDown(float DeltaTime, bool cool) {
	if (coolingDown) {
		if (GetLocalRole() < ROLE_Authority) {
			ServerUpdateCoolDown(DeltaTime);
		}
	}
}

void ARopeSMPlayerCharacter::ServerUpdateCoolDown_Implementation(float DeltaTime, bool cool) {
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

bool ARopeSMPlayerCharacter::ServerUpdateCoolDown_Validate(float DeltaTime, bool cool) {
	return true;
}

