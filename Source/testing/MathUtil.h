#pragma once
#define M_PI           3.14159265358979323846

struct MathUtil {

	//Returns SQRT(a^2 + b^2)
	static float Hypotenuse(float a, float b) {
		return sqrt(a*a + b*b);
	}

	//1 Hammer Unit is 1.905 Unreal Units.
	static float ToHammerUnits(float a) {
		return a / 1.905;
	}

	static float ToUnrealUnits(float a) {
		return a * 1.905;
	}

	//DEPRECATED - Returns an acceleration vector similar to source movement
	static FVector CalculateAcceleration(FVector Velocity, FVector Acceleration, float DeltaTime) {
		float magVprojA = Velocity.CosineAngle2D(Acceleration)*Velocity.Size();
		float magAxT = (Acceleration * DeltaTime).Size();
		if (magVprojA < (ToUnrealUnits(30) - magAxT)) {
			return Acceleration * DeltaTime;
		} else if (magVprojA < ToUnrealUnits(30)) {
			return (ToUnrealUnits(30) - magVprojA) * (Acceleration / Acceleration.Size());
		} else {
			return FVector(0, 0, 0);
		}
	}

};

struct DebugUtil {

	//Adds a blue debug message to the top-left of the screen
	static void Message(FString string, float time) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, time, FColor::Blue, string);
		}
	}

	//Adds a red debug message to the top-left of the screen
	static void Error(FString string, float time) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, time, FColor::Red, string);
		}
	}

};