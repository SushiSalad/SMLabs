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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ABaseWeapon* weapon;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> HandgunClass;
	class ABaseWeapon* handgun;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> RocketClass;
	class ABaseWeapon* rocket_launcher;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//On hit, if space held, jump
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) override;

	

	void SwitchWeapon();

	void Fire();

	void Reload();
		
	void RopeStuff(float DeltaTime);
	void FireRope();
	void PullRope();
	void DetachRope();

	//Jumping
	void StartJump();
	void StopJump();

	//Input
	UPROPERTY(EditAnywhere)
	float fAxis;
	UPROPERTY(EditAnywhere)
	float rAxis;

	//Source Movement
	UPROPERTY(EditAnywhere)
	float AirAcceleration;
	UPROPERTY(EditAnywhere)
	float GroundAcceleration;
	UPROPERTY(EditAnywhere)
	float MaxAirSpeedIncrease;
	int TicksOnGround;	
	bool spaceHold;
	
	//Client functions
	FVector ClientMovementStuff(float DeltaTime, float _fAxis, float _rAxis);
	FVector ClientCreateAccelerationVector(float _fAxis, float _rAxis);
	FVector ClientGetNextFrameVelocity(FVector AccelVector, float DeltaTime);
	void ClientSetVelocity(FVector vel, float DeltaTime);

	//Server functions
	UFUNCTION(Server, Reliable)
	void ReplicateMovementPlease(float dTime, float _fAxis, float _rAxis, FVector iniVel);
	FVector ServerMovementStuff(float DeltaTime, float _fAxis, float _rAxis);
	FVector ServerCreateAccelerationVector(float _fAxis, float _rAxis);
	FVector ServerGetNextFrameVelocity(FVector AccelVector, float DeltaTime);
	void ServerSetVelocity(FVector vel, float DeltaTime, FVector iniVel);

	//Input
	float fAxis;
	float rAxis;
	
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

	
};
