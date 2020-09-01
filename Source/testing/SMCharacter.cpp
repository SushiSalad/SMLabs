// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacter.h"
#include "MathUtil.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"

ASMCharacter::ASMCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//First Person Camera
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FPSCameraComponent->SetupAttachment((USceneComponent*)GetCapsuleComponent());
	//FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	FPSCameraComponent->bUsePawnControlRotation = true;

	//Other Components
	SMCapsuleComponent = GetCapsuleComponent();
	SMCharacterMovementComponent = GetCharacterMovement();

	//Default values
	AirAcceleration = 20000;
	GroundAcceleration = 10000;
	AirSpeedIncreaseLimit = 50;
	TicksOnGround = 0;
	spaceHold = false;
	ropeFired = false;
}

// Called when the game starts or when spawned
void ASMCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MovementStuff(DeltaTime);
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
}

//// MOVEMENT ////

void ASMCharacter::MovementStuff(float DeltaTime) {
	if (spaceHold) {
		SMCharacterMovementComponent->GroundFriction = 0;
		SMCharacterMovementComponent->BrakingDecelerationWalking = 0;
		SMCharacterMovementComponent->BrakingDecelerationFlying = 0;
		GroundAcceleration = 10000;
	} else {
		SMCharacterMovementComponent->GroundFriction = 8;
		GroundAcceleration = 10000;
	}
	GetMovementComponent()->Velocity += GetNextFrameVelocity(CreateAccelerationVector(), DeltaTime);
	DebugUtil::Message(FString::Printf(TEXT("%.2f u/s"), 
		MathUtil::Hypotenuse(GetMovementComponent()->Velocity.X, GetMovementComponent()->Velocity.Y)), DeltaTime);
}

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
	accel = AActor::GetActorForwardVector() * AActor::GetInputAxisValue(FName("MoveForward"));
	accel += AActor::GetActorRightVector() * AActor::GetInputAxisValue(FName("MoveRight"));
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
		if (magVprojA < (AirSpeedIncreaseLimit - magAxT)) {
			return AccelVector * DeltaTime;
		} else if (magVprojA < AirSpeedIncreaseLimit) {
			return (AirSpeedIncreaseLimit - magVprojA) * (AccelVector / AccelVector.Size());
		} else {
			return FVector(0, 0, 0);
		}
	} else {
		return AccelVector * DeltaTime;
	}
}

//// SWINGING ////

void ASMCharacter::RopeStuff(float DeltaTime) {
	
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
		FVector end = cameraLoc + (cameraRot.Vector() * 5000.f);

		DrawDebugLine(GetWorld(), cameraLoc, end, FColor::Green, false, 1, 0, 1);
		if (GetWorld()->LineTraceSingleByChannel(ropeTarget, cameraLoc, end, ECC_WorldStatic, collisionParams)) {
			DrawDebugLine(GetWorld(), cameraLoc, ropeTarget.ImpactPoint, FColor::Red, false, 1, 0, 5);
			DebugUtil::Message(FString::Printf(TEXT("Collision on %s at %s" ), *ropeTarget.GetActor()->GetActorLabel(), 
				*ropeTarget.ImpactPoint.ToString()), 10);
			ropeAttached = true;
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
	ropeTarget.Reset();
}