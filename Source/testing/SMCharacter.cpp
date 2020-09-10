// Fill out your copyright notice in the Description page of Project Settings.


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

ASMCharacter::ASMCharacter()
{
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

	FPSCameraComponent->AttachTo(GetMesh(), "headSocket", EAttachLocation::SnapToTarget);
	
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
		SMCharacterMovementComponent->BrakingDecelerationWalking = 2048.0;
		GroundAcceleration = 10000;
	}
	GetMovementComponent()->Velocity += GetNextFrameVelocity(CreateAccelerationVector(), DeltaTime);
	DebugUtil::Message(FString::Printf(TEXT("%.2f u/s"), 
		MathUtil::ToHammerUnits(MathUtil::Hypotenuse(GetMovementComponent()->Velocity.X, GetMovementComponent()->Velocity.Y))), DeltaTime);
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