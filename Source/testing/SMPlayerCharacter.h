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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class ABaseWeapon* weapon;
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



	////Damage Stuff////
	UFUNCTION(BlueprintCallable, Category = Health)
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser) override;
	//Taken from ShooterGame UE example project
	//Kills pawn.  Server / authority only.
	virtual bool Die(float KillingDamage, struct FDamageEvent const& DamageEvent, class AController* Killer, class AActor* DamageCauser);
	/** notification when killed, for both the server and client. */
//	virtual void OnDeath(float KillingDamage, struct FDamageEvent const& DamageEvent, class APawn* InstigatingPawn, class AActor* DamageCauser);
	//Respawns the player to a random PlayerStart with given tag
	virtual bool Respawn(FName playerStartTag);

	////Weapon Stuff////
	void SwitchWeapon();
	void StartWeaponFire();
	void StopWeaponFire();
	void Reload();
	void Holster();
	UFUNCTION(Server, Reliable, WithValidation)
	void ShootWeapon();
	void ShootWeapon_Implementation();
	bool ShootWeapon_Validate();
	UFUNCTION(NetMulticast, Unreliable, WithValidation)
	void PlayWeaponFireAnimation(int8 playerIndex);
	void PlayWeaponFireAnimation_Implementation(int8 playerIndex);
	bool PlayWeaponFireAnimation_Validate(int8 playerIndex);

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
