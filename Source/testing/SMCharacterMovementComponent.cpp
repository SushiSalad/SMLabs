// Fill out your copyright notice in the Description page of Project Settings.


#include "SMCharacterMovementComponent.h"
#include "MathUtil.h"

#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Engine/NetworkObjectList.h"

static TAutoConsoleVariable<int32> CVarShowPos(TEXT("cl.ShowPos"), 0, TEXT("Show position and movement information.\n"), ECVF_Default);

USMCharacterMovementComponent::USMCharacterMovementComponent() {
	//AirControl = 1.0f;
	//AirControlBoostMultiplier = 1.0f;
	//AirControlBoostVelocityThreshold = 0.0f;
	GroundAccelerationMultiplier = 100;
	AirAccelerationMultiplier = 200;
	AirSpeedCap = MathUtil::ToUnrealUnits(30);
	bShowPos = false;
}

void USMCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if ((bShowPos || CVarShowPos->GetInt() != 0) && CharacterOwner) {
		DebugUtil::Message(FString::Printf(TEXT("pos: %f %f %f"), CharacterOwner->GetActorLocation().X, 
			CharacterOwner->GetActorLocation().Y, CharacterOwner->GetActorLocation().Z), DeltaTime); //debug position
		DebugUtil::Message(FString::Printf(TEXT("ang: %f %f %f"), CharacterOwner->GetControlRotation().Yaw,
			CharacterOwner->GetControlRotation().Pitch, CharacterOwner->GetControlRotation().Roll), DeltaTime);
		float vel = MathUtil::Hypotenuse(Velocity.X, Velocity.Y);
		DebugUtil::Message(FString::Printf(TEXT("vel ue: %f\nvel hu: %f"), vel, MathUtil::ToHammerUnits(vel)), DeltaTime);
	}
}

void USMCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) {
	// Do not update velocity when using root motion or when SimulatedProxy and not simulating root motion - SimulatedProxy are repped their Velocity
	if (!HasValidData() || HasAnimRootMotion() || DeltaTime < MIN_TICK_TIME || (CharacterOwner && CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy && !bWasSimulatingRootMotion)) {
		return;
	}

	Friction = FMath::Max(0.f, Friction);
	const float MaxAccel = GetMaxAcceleration();
	float MaxSpeed = GetMaxSpeed();

	// Check if path following requested movement
	bool bZeroRequestedAcceleration = true;
	FVector RequestedAcceleration = FVector::ZeroVector;
	float RequestedSpeed = 0.0f;
	if (ApplyRequestedMove(DeltaTime, MaxAccel, MaxSpeed, Friction, BrakingDeceleration, RequestedAcceleration, RequestedSpeed)) {
		bZeroRequestedAcceleration = false;
	}

	if (bForceMaxAccel) {
		// Force acceleration at full speed.
		// In consideration order for direction: Acceleration, then Velocity, then Pawn's rotation.
		if (Acceleration.SizeSquared() > SMALL_NUMBER) {
			Acceleration = Acceleration.GetSafeNormal() * MaxAccel;
		} else {
			Acceleration = MaxAccel * (Velocity.SizeSquared() < SMALL_NUMBER ? UpdatedComponent->GetForwardVector() : Velocity.GetSafeNormal());
		}

		AnalogInputModifier = 1.f;
	}

	// Path following above didn't care about the analog modifier, but we do for everything else below, so get the fully modified value.
	// Use max of requested speed and max speed if we modified the speed in ApplyRequestedMove above.
	const float MaxInputSpeed = FMath::Max(MaxSpeed * AnalogInputModifier, GetMinAnalogSpeed());
	MaxSpeed = FMath::Max(RequestedSpeed, MaxInputSpeed);

	// Apply braking or deceleration
	const bool bZeroAcceleration = Acceleration.IsZero();
	const bool bVelocityOverMax = IsExceedingMaxSpeed(MaxSpeed);

	// Only apply braking if there is no acceleration, or we are over our max speed and need to slow down to it.
	if ((bZeroAcceleration && bZeroRequestedAcceleration) || bVelocityOverMax) {
		const FVector OldVelocity = Velocity;

		const float ActualBrakingFriction = (bUseSeparateBrakingFriction ? BrakingFriction : Friction);
		ApplyVelocityBraking(DeltaTime, ActualBrakingFriction, BrakingDeceleration);

		// Don't allow braking to lower us below max speed if we started above it.
		if (bVelocityOverMax && Velocity.SizeSquared() < FMath::Square(MaxSpeed) && FVector::DotProduct(Acceleration, OldVelocity) > 0.0f) {
			Velocity = OldVelocity.GetSafeNormal() * MaxSpeed;
		}
	} else if (!bZeroAcceleration) {
		// Friction affects our ability to change direction. This is only done for input acceleration, not path following.
		const FVector AccelDir = Acceleration.GetSafeNormal();
		const float VelSize = Velocity.Size();
		Velocity = Velocity - (Velocity - AccelDir * VelSize) * FMath::Min(DeltaTime * Friction, 1.f);
	}

	// Apply fluid friction
	if (bFluid) {
		Velocity = Velocity * (1.f - FMath::Min(Friction * DeltaTime, 1.f));
	}

	// Apply input acceleration
	if (!bZeroAcceleration) {
		//const float NewMaxInputSpeed = IsExceedingMaxSpeed(MaxInputSpeed) ? Velocity.Size() : MaxInputSpeed;
		//Velocity += Acceleration * DeltaTime;
		//Velocity = Velocity.GetClampedToMaxSize(NewMaxInputSpeed);

		float accelMultiplier = IsMovingOnGround() ? GroundAccelerationMultiplier : AirAccelerationMultiplier;
		Acceleration = MathUtil::CalculateAcceleration(Velocity, Acceleration, DeltaTime) * accelMultiplier;
		Velocity += Acceleration;
	}

	// Apply additional requested acceleration
	if (!bZeroRequestedAcceleration) {
		//const float NewMaxRequestedSpeed = IsExceedingMaxSpeed(RequestedSpeed) ? Velocity.Size() : RequestedSpeed;
		Velocity += RequestedAcceleration * DeltaTime;
		//Velocity = Velocity.GetClampedToMaxSize(NewMaxRequestedSpeed);
	}

	if (bUseRVOAvoidance) {
		CalcAvoidanceVelocity(DeltaTime);
	}
}