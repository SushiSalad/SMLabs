// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SMPlayerCharacter.h"
#include "RopeSMPlayerCharacter.generated.h"

/**
 *
 */
UCLASS()
class TESTING_API ARopeSMPlayerCharacter : public ASMPlayerCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:

	ARopeSMPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	float MaxRopeDistance;
	UPROPERTY(EditAnywhere)
	float RopePullSpeed;
	UPROPERTY(Replicated)
	bool ropeFired;
	UPROPERTY(Replicated)
	bool ropeAttached;
	UPROPERTY(Replicated)
	FHitResult ropeTarget;
	float coolDown;
	UPROPERTY(Replicated)
	float coolDownElapsed;
	UPROPERTY(Replicated)
	bool coolingDown;

	void FireRope();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFireRope();
	void ServerFireRope_Implementation();
	bool ServerFireRope_Validate();

	void RopeStuff(float DeltaTime);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRopeStuff(float DeltaTime);
	void ServerRopeStuff_Implementation(float DeltaTime);
	bool ServerRopeStuff_Validate(float DeltaTime);

	void DetachRope();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDetachRope();
	void ServerDetachRope_Implementation();
	bool ServerDetachRope_Validate();

	void StartSliding();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartSliding();
	void ServerStartSliding_Implementation();
	bool ServerStartSliding_Validate();

	void StopSliding();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopSliding();
	void ServerStopSliding_Implementation();
	bool ServerStopSliding_Validate();

	void UpdateCoolDown(float DeltaTime, bool cool = false);
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateCoolDown(float DeltaTime, bool cool = false);
	void ServerUpdateCoolDown_Implementation(float DeltaTime, bool cool = false);
	bool ServerUpdateCoolDown_Validate(float DeltaTime, bool cool = false);

};

