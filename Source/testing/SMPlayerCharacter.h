// Inherits from PBPlayerCharacter from Project Borealis

#pragma once

#include "CoreMinimal.h"
#include "Character/PBPlayerCharacter.h"
#include "SMPlayerCharacter.generated.h"

UCLASS()
class TESTING_API ASMPlayerCharacter : public APBPlayerCharacter
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	////Components////
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class UCameraComponent* FPSCameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ABaseWeapon* weapon;
	UPROPERTY(EditAnywhere)
		TSubclassOf<ABaseWeapon> HandgunClass;
		class ABaseWeapon* handgun;
	UPROPERTY(EditAnywhere)
		TSubclassOf<ABaseWeapon> RocketClass;
		class ABaseWeapon* rocket_launcher;

	////Properties////
	UPROPERTY(Replicated, BlueprintReadWrite)
		float Health;
	UPROPERTY(Replicated, BlueprintReadWrite)
		float Armor;
	UPROPERTY(Replicated, BlueprintReadWrite)
		bool isHoldingWeapon;

	////Unreal Functions or Events////
	ASMPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	//Replicate variables on server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	////Movement Stuff////
	void MoveForward(float value);
	void MoveRight(float value);

	////Weapon Stuff////
	void SwitchWeapon();
	void Fire();
	void Reload();

};
