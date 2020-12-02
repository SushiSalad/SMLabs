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

	bool bWantsToFire = false;

public:
////Components////
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* FPSCameraComponent;
	/** currently equipped weapon */
	UPROPERTY(Replicated, Transient, VisibleAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentWeapon)
	class ABaseWeapon* CurrentWeapon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> HandgunClass;
	class ABaseWeapon* handgun;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseWeapon> RocketClass;
	class ABaseWeapon* rocket_launcher;

////Properties////
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Health)
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	float MAX_HEALTH = 100.f;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Health)
	float Armor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Health)
	float MAX_ARMOR = 50.f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = Weapon)
	bool isHoldingWeapon = true;

////Unreal Functions or Events////
	ASMPlayerCharacter(const FObjectInitializer& ObjectInitializer);
	//Replicate variables on server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	//Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

////Movement Stuff////
	void MoveForward(float value);
	void MoveRight(float value);

////Damage Stuff////
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;
	//Taken from ShooterGame UE example project
	//Kills pawn.  Server / authority only.
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);
	/** notification when killed, for both the server and client. */
	//virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);
	
	/** [server] respawns the player at a spawn point */
	void ServerRespawn();

////Weapon Stuff////
	void SwitchWeapon();
	void StartWeaponFire();
	void StopWeaponFire();
	void Reload();
	void Holster();

	/** [server + local] plays the equipped weapon's fire animation */
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void PlayWeaponFireAnimation(int8 playerIndex);
	void PlayWeaponFireAnimation_Implementation(int8 playerIndex);
	bool PlayWeaponFireAnimation_Validate(int8 playerIndex);

//// Inventory Stuff ////
	/** [server + local] equips weapon from inventory */
	void EquipWeapon(class ABaseWeapon* Weapon);
	UFUNCTION(reliable, server, WithValidation)
	void ServerEquipWeapon(class ABaseWeapon* NewWeapon);
	void ServerEquipWeapon_Implementation(class ABaseWeapon* NewWeapon);
	bool ServerEquipWeapon_Validate(class ABaseWeapon* NewWeapon);

	/** current weapon rep handler */
	UFUNCTION()
	void OnRep_CurrentWeapon(class ABaseWeapon* LastWeapon);

	/** updates current weapon */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetCurrentWeapon(ABaseWeapon* NewWeapon, ABaseWeapon* LastWeapon = NULL);

	
};
