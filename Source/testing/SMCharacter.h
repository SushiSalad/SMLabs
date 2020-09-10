// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMCharacter.generated.h"

UCLASS()
class TESTING_API ASMCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASMCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* FPSCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* SMCapsuleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCharacterMovementComponent* SMCharacterMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = cable)
	class UCableComponent* rope;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//On hit, if space held, jump
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) override;

	//Jumping
	void StartJump();
	void StopJump();

	//Source Movement
	UPROPERTY(EditAnywhere)
	float AirAcceleration;
	UPROPERTY(EditAnywhere)
	float GroundAcceleration;
	UPROPERTY(EditAnywhere)
	float MaxAirSpeedIncrease;
	int TicksOnGround;
	bool spaceHold;

	void MovementStuff(float DeltaTime);
	FVector CreateAccelerationVector();
	FVector GetNextFrameVelocity(FVector AccelVector, float DeltaTime);

	//Swinging
	UPROPERTY(EditAnywhere)
	float MaxRopeDistance;
	UPROPERTY(EditAnywhere)
	float RopePullSpeed;
	UPROPERTY(EditAnywhere)
	bool WidowGrapple;
	bool ropeFired;
	bool ropeAttached;
	FHitResult ropeTarget;
	//ASMRope rope;

	void RopeStuff(float DeltaTime);
	void FireRope();
	void PullRope();
	void DetachRope();
};
