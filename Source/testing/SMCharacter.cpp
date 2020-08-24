// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacter.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ASMCharacter::ASMCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Create a first person camera component.
	FPSCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	// Attach the camera component to our capsule component.
	FPSCameraComponent->SetupAttachment((USceneComponent*)GetCapsuleComponent());
	// Position the camera slightly above the eyes.
	FPSCameraComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f + BaseEyeHeight));
	// Allow the pawn to control camera rotation.
	FPSCameraComponent->bUsePawnControlRotation = true;

	SMCapsuleComponent = GetCapsuleComponent();
	SMCharacterMovementComponent = GetCharacterMovement();

	AirAcceleration = 10000;
	GroundAcceleration = 10000;
	AirSpeedIncreaseLimit = 30;
	TicksOnGround = 0;
	spaceHold = false;
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
}

void ASMCharacter::NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse,
	const FHitResult & Hit) {
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