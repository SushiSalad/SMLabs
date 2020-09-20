// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SMCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class TESTING_API USMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

	virtual void PerformMovement(float DeltaTime) override;

	virtual void ReplicateMoveToServer(float DeltaTime, const FVector& NewAcceleration) override;

	virtual bool HandlePendingLaunch() override;

	FVector CreateAccelerationVector();

	FVector GetNextFrameVelocity(FVector AccelVector, float DeltaTime);
	
};
