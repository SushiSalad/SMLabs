#pragma once
#define M_PI           3.14159265358979323846

struct MathUtil {

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

};

struct DebugUtil {

	static void Message(FString string, float time) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, time, FColor::Blue, string);
		}
	}

	static void Error(FString string, float time) {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, time, FColor::Red, string);
		}
	}

};