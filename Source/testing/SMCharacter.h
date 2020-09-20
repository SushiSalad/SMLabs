// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SMCharacter.generated.h"

UCLASS()
class TESTING_API ASMCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Sets default values for this character's properties
	ASMCharacter(const FObjectInitializer& ObjectInitializer);

	////Components////

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* FPSCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* SMCapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USMCharacterMovementComponent* SMCharacterMovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ABaseWeapon* weapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> HandgunClass;
	class ABaseWeapon* handgun;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> RocketClass;
	class ABaseWeapon* rocket_launcher;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	FORCEINLINE class USMCharacterMovementComponent* GetSMMovementComponent() const { return SMCharacterMovementComponent; }

	//Properties
	UPROPERTY(Replicated, BlueprintReadWrite)
	float Health;
	UPROPERTY(Replicated, BlueprintReadWrite)
	float Armor;

	////Unreal Functions or Events////

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//On hit, if space held, jump (auto bhop)
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp,
		bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) override;
	//Cast movement component to custom one
	virtual void PostInitializeComponents() override;
	//Replicate variables
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	////Weapon Stuff////

	void SwitchWeapon();

	void Fire();

	void Reload();

	////Movement Stuff////

	void StartJump();
	void StopJump();

	bool spaceHold;

	//Swinging
	UPROPERTY(EditAnywhere)
	float MaxRopeDistance;
	UPROPERTY(EditAnywhere)
	float RopePullSpeed;
	bool ropeFired;
	bool ropeAttached;
	FHitResult ropeTarget;

	void RopeStuff(float DeltaTime);
	void FireRope();
	void DetachRope();

	//Testing
	void MoveForward(float value);
	void MoveRight(float value);

};
