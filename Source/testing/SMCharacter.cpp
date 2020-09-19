// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SMCharacter.h"
#include "MathUtil.h"
#include "BaseWeapon.h"
#include "SMGameState.h"
#include "SMPlayerState.h"
#include "SMGameMode.h"
#include "SMCharacterMovementComponent.h"

#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

//Constructor
ASMCharacter::ASMCharacter(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer.SetDefaultSubobjectClass<USMCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	//Unreal values
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	SetReplicateMovement(true);

	//FPS Camera Component
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->bUsePawnControlRotation = true;
	//Other Components
	SMCapsuleComponent = GetCapsuleComponent();
	//SMCharacterMovementComponent = GetMovementComponent();
	

	//Default values
	AirAcceleration = 20000;
	GroundAcceleration = 10000;
	MaxAirSpeedIncrease = MathUtil::ToUnrealUnits(30);
	MaxRopeDistance = 50000;
	RopePullSpeed = 1;
	spaceHold = false;
	ropeFired = false;
}

void ASMCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(ASMCharacter, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASMCharacter, Armor, COND_OwnerOnly);
}

// Called when the game starts or when spawned
void ASMCharacter::BeginPlay()
{
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

// Called every frame
void ASMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//MovementStuff(DeltaTime);
	//Srv_MovementStuff(DeltaTime);
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
	PlayerInputComponent->BindAction(FName("FireRope"), IE_Released, this, &ASMCharacter::DetachRope);
	//Setup weapon functionality
	PlayerInputComponent->BindAction(FName("Fire"), IE_Pressed, this, &ASMCharacter::Fire);
	PlayerInputComponent->BindAction(FName("Reload"), IE_Pressed, this, &ASMCharacter::Reload);
	PlayerInputComponent->BindAction(FName("SwitchWeapon"), IE_Pressed, this, &ASMCharacter::SwitchWeapon);
}

void ASMCharacter::PostInitializeComponents() {
	Super::PostInitializeComponents();

	SMCharacterMovementComponent = Cast<USMCharacterMovementComponent>(Super::GetMovementComponent());
}

//// DAMAGE STUFF ////



//// WEAPON STUFF ////

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

void ASMCharacter::Srv_MovementStuff_Implementation(float DeltaTime) {
	//ASMGameState* GameState = Cast<ASMGameState>(UGameplayStatics::GetGameState(GetWorld()));

	MovementStuff(DeltaTime);
}
bool ASMCharacter::Srv_MovementStuff_Validate(float DeltaTime) {
	return true;
}

void ASMCharacter::MovementStuff(float DeltaTime) {
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
	GetMovementComponent()->Velocity += GetNextFrameVelocity(CreateAccelerationVector(), DeltaTime);
	DebugUtil::Message(FString::Printf(TEXT("%.2f u/s"),
		MathUtil::ToHammerUnits(MathUtil::Hypotenuse(GetMovementComponent()->Velocity.X, GetMovementComponent()->Velocity.Y))), DeltaTime);
}

//Auto bhop.
void ASMCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) {
	if (spaceHold) {
		Jump();
	} else {
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

//creates the acceleration vector for the next frame
FVector ASMCharacter::CreateAccelerationVector() {
	FVector accel;
	accel = GetActorForwardVector() * GetInputAxisValue(FName("MoveForward"));
	accel += GetActorRightVector() * GetInputAxisValue(FName("MoveRight"));
	//accel = GetLastMovementInputVector();
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

//// SWINGING ////

void ASMCharacter::RopeStuff(float DeltaTime) {
	if (ropeAttached) {
		DrawDebugLine(GetWorld(), GetActorLocation(), ropeTarget.ImpactPoint, FColor::Red, false, -1.0F, 0, 2);
		this->LaunchCharacter((ropeTarget.ImpactPoint - GetActorLocation()) * RopePullSpeed * DeltaTime, false, false);
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
		}
	}
}

void ASMCharacter::DetachRope() {
	ropeFired = false;
	ropeAttached = false;
	ropeTarget.Reset();
}