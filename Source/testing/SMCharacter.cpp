// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SMCharacter.h"
#include "MathUtil.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
//#include "SMRope.h"
#include "CableComponent.h"
#include "BaseWeapon.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"

ASMCharacter::ASMCharacter()
{

	bReplicates = true;
	SetReplicateMovement(true);

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//First Person Camera
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->bUsePawnControlRotation = true;

	//FAttachmentTransformRules rules(EAttachmentRule::KeepRelative, false);
	
	//ssaFPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 500.0f + BaseEyeHeight));

	//Other Components
	SMCapsuleComponent = GetCapsuleComponent();
	SMCharacterMovementComponent = GetCharacterMovement();

	//Axis variables
	

	//Rope
	rope = CreateDefaultSubobject<UCableComponent>(TEXT("Rope"));
	//rope->AttachToComponent(this->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
	//rope->SetAttachEndTo(this, RootComponent->GetDefaultSceneRootVariableName());
	rope->SetVisibility(false);
	rope->CableLength = 0;
	rope->CableWidth = 6;
	rope->EndLocation = FVector(0, 0, 0);

	//Default values
	AirAcceleration = 20000;
	GroundAcceleration = 10000;
	MaxAirSpeedIncrease = MathUtil::ToUnrealUnits(30);
	MaxRopeDistance = 50000;
	RopePullSpeed = 1;
	WidowGrapple = true;
	TicksOnGround = 0;
	spaceHold = false;
	ropeFired = false;
}

// Called when the game starts or when spawned
void ASMCharacter::BeginPlay()
{
	Super::BeginPlay();

	//spawn and set weapons
	FActorSpawnParameters Sparam;
	handgun = GetWorld()->SpawnActor<ABaseWeapon>(HandgunClass, FTransform(FVector(0,0,0)), Sparam);
	handgun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");

	rocket_launcher = GetWorld()->SpawnActor<ABaseWeapon>(RocketClass, FTransform(FVector(0, 0, 0)), Sparam);
	rocket_launcher->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
	rocket_launcher->toggleVis();

	//starting weapon
	weapon = handgun;

	FPSCameraComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "headSocket");
	
}

// Called every frame
void ASMCharacter::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

	fAxis = AActor::GetInputAxisValue(FName("MoveForward"));
	rAxis = AActor::GetInputAxisValue(FName("MoveRight"));
	
	ReplicateMovementPlease_Implementation(DeltaTime, fAxis, rAxis);
	RopeStuff(DeltaTime);
}

// Called to bind functionality to input
void ASMCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Setup movement bindings
	PlayerInputComponent->BindAxis(FName("MoveForward"));
	PlayerInputComponent->BindAxis(FName("MoveRight"));
	//Setup mouse look bindings
	PlayerInputComponent->BindAxis("LookHorizontal", this, &ASMCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookVertical", this, &ASMCharacter::AddControllerPitchInput);
	//Setup jump bindings
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ASMCharacter::StartJump);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Released, this, &ASMCharacter::StopJump);
	//Setup swinging bindings
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Pressed, this, &ASMCharacter::FireRope);
	PlayerInputComponent->BindAction(FName("PullRope"), IE_Pressed, this, &ASMCharacter::PullRope);
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Released, this, &ASMCharacter::DetachRope);
	//Setup weapon fuinctionality
	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ASMCharacter::Fire);
	PlayerInputComponent->BindAction(FName("Reload"), IE_Pressed, this, &ASMCharacter::Reload);
	PlayerInputComponent->BindAction(FName("SwitchWeapon"), IE_Pressed, this, &ASMCharacter::SwitchWeapon);
}

void ASMCharacter::ReplicateMovementPlease_Implementation(float dTime, float _fAxis, float _rAxis) {
	//fAxis = _fAxis;
	//rAxis = _rAxis;
	
	FVector vel = MovementStuff(dTime, _fAxis, _rAxis);
	SetVelocity_Client(vel, dTime);
	MovementStuff_Client();
	SetVelocity_Server(vel, dTime);
	
}

//// WEAPON STUFF ///

void ASMCharacter::Fire() {
	if (weapon->ammo > 0) {
		weapon->Fire();
		this->PlayAnimMontage(weapon->montage[0]);
	}
}

void ASMCharacter::Reload() {
	if (weapon->ammo < weapon->maxAmmo) {
		weapon->Reload();
		this->PlayAnimMontage(weapon->montage[1]);
	}
}

void ASMCharacter::SwitchWeapon() {
	if (weapon == handgun) {
		rocket_launcher->toggleVis();
		handgun->toggleVis();
		weapon = rocket_launcher;
	}
	else if (weapon == rocket_launcher) {
		rocket_launcher->toggleVis();
		handgun->toggleVis();
		weapon = handgun;
	}
}

//// MOVEMENT ////
//The networked movement is probably weird because it keeps calcing vel with 0 initial vel


FVector ASMCharacter::MovementStuff(float DeltaTime, float _fAxis, float _rAxis) {

	//Don't apply friction while jumping prevent losing speed while bhopping
	if (spaceHold) {
		SMCharacterMovementComponent->GroundFriction = 0;
		SMCharacterMovementComponent->BrakingDecelerationWalking = 0;
		SMCharacterMovementComponent->BrakingDecelerationFlying = 0;
		GroundAcceleration = 10000;
	} else {
		SMCharacterMovementComponent->GroundFriction = 8;
		SMCharacterMovementComponent->BrakingDecelerationWalking = 2048.0;
		GroundAcceleration = 10000;
	}

	

	//Actually calculate next velocity.
	FVector nextVel = GetNextFrameVelocity(CreateAccelerationVector(_fAxis, _rAxis), DeltaTime);
	return nextVel;
	//ReplicateMovementPlease_Implementation(DeltaTime);
	
}

void ASMCharacter::MovementStuff_Client_Implementation(){
	
	if (spaceHold) {
		SMCharacterMovementComponent->GroundFriction = 0;
		SMCharacterMovementComponent->BrakingDecelerationWalking = 0;
		SMCharacterMovementComponent->BrakingDecelerationFlying = 0;
		GroundAcceleration = 10000;
	}
	else {
		SMCharacterMovementComponent->GroundFriction = 8;
		SMCharacterMovementComponent->BrakingDecelerationWalking = 2048.0;
		GroundAcceleration = 10000;
	}

	//DebugUtil::Message(FString::Printf(TEXT("%.2f u/s"),
		//MathUtil::ToHammerUnits(MathUtil::Hypotenuse(GetMovementComponent()->Velocity.X, GetMovementComponent()->Velocity.Y))), DeltaTime);

}

//creates the acceleration vector for the next frame
FVector ASMCharacter::CreateAccelerationVector(float _fAxis, float _rAxis) {
	FVector accel;
	//accel = AActor::GetActorForwardVector() * AActor::GetInputAxisValue(FName("MoveForward"));
	//accel += AActor::GetActorRightVector() * AActor::GetInputAxisValue(FName("MoveRight"));
	accel = AActor::GetActorForwardVector() * _fAxis;
	accel += AActor::GetActorRightVector() * _rAxis;

	accel.Normalize(0.0001f);
	accel *= (GetMovementComponent()->IsFalling() || bPressedJump) ? AirAcceleration : GroundAcceleration;
	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Blue, FString::Printf(TEXT("%f, %f, %f"), accel.X, accel.Y, accel.Z));
	return accel;
}

//use the acceleration if the the magnitude of the next frame's velocity is less than the limit
FVector ASMCharacter::GetNextFrameVelocity(FVector AccelVector, float DeltaTime) {
	float magVprojA = GetMovementComponent()->Velocity.CosineAngle2D(AccelVector)*GetMovementComponent()->Velocity.Size();
	float magAxT = (AccelVector * DeltaTime).Size();
	if (GetMovementComponent()->IsFalling() || bPressedJump) {
		if (magVprojA < (MaxAirSpeedIncrease - magAxT)) {
			return AccelVector * DeltaTime;
		} else if (magVprojA < MaxAirSpeedIncrease) {
			return (MaxAirSpeedIncrease - magVprojA) * (AccelVector / AccelVector.Size());
		} else {
			return FVector(0, 0, 0);
		}
	} else {
		return AccelVector * DeltaTime;
	}
}

void ASMCharacter::SetVelocity_Server_Implementation(FVector vel, float DeltaTime) {

	DebugUtil::Message(FString::Printf(TEXT("Server: %.2f u/s"),
		MathUtil::ToHammerUnits(GetMovementComponent()->Velocity.Size())), DeltaTime);

	
	GetMovementComponent()->Velocity += vel;
}

void ASMCharacter::SetVelocity_Client_Implementation(FVector vel, float DeltaTime) {

	DebugUtil::Message(FString::Printf(TEXT("Client: %.2f u/s"),
		MathUtil::ToHammerUnits(GetMovementComponent()->Velocity.Size())), DeltaTime);

	GetMovementComponent()->Velocity += vel;
}

//Auto bhop.
void ASMCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) {
	if (spaceHold) {
		Jump();
	}
	else {
		StopJumping();
	}
}

void ASMCharacter::StartJump() {
	bPressedJump = true;
	spaceHold = true;
}

void ASMCharacter::StopJump() {
	bPressedJump = false;
	spaceHold = false;
}

//// SWINGING ////

void ASMCharacter::RopeStuff(float DeltaTime) {
	if (WidowGrapple) {
		if (ropeAttached) {
			DrawDebugLine(GetWorld(), GetActorLocation(), ropeTarget.ImpactPoint, FColor::Red, false, -1.0F, 0, 2);
			this->LaunchCharacter((ropeTarget.ImpactPoint - GetActorLocation()) * RopePullSpeed * DeltaTime, false, false);
		}
	} else {
		
	}
}

void ASMCharacter::FireRope() {
	if (!ropeFired) {
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
			if (!WidowGrapple) {
				//rope = ASMRope(cameraLoc, end);

			}
		}
	}
}

void ASMCharacter::PullRope() {
	if (ropeFired) {
		if (ropeAttached) {
			
		} else {

		}
	}
}

void ASMCharacter::DetachRope() {
	ropeFired = false;
	ropeAttached = false;
	rope->SetWorldLocation(FVector(0,0,0));
	rope->SetVisibility(false);
	ropeTarget.Reset();
}