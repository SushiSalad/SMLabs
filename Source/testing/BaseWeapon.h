
//The actual weapons themselves are set up in blueprint form, 
//as it is easier to manipulate their meshes there and
//they are simple enough to be kept as blueprints.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM()
enum DamageType {

	//placeholder values for now.

	Physical UMETA(DisplayName = "Physical"),
	Electric UMETA(DisplayName = "Electric"),
	Burn UMETA(DisplayName = "Burn"),
};

namespace EWeaponState
{
	enum Type
	{
		Idle,
		Firing,
		Reloading,
		Equipping,
	};
}

UCLASS()
class TESTING_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** current weapon state */
	EWeaponState::Type CurrentState;

	/** is weapon fire active? */
	bool bWantsToFire = false;

	/** pawn owner */
	UPROPERTY(Transient, ReplicatedUsing = OnRep_MyPawn)
	class ASMPlayerCharacter* MyPawn;
	UFUNCTION()
	void OnRep_MyPawn();

public:	
	// Sets default values for this actor's properties
	ABaseWeapon();
	virtual void Tick(float DeltaTime) override;

	//Replicate variables on server
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	//////////////////////////////////////////////////////////////////////////
	// Pawn Ownership stuff

	/** set the weapon's owning pawn */
	void SetOwningPawn(ASMPlayerCharacter* NewOwner);

	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(ASMPlayerCharacter* NewOwner);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();


	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local + server] start weapon fire */
	virtual void StartFire();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();
	void ServerStartFire_Implementation();
	bool ServerStartFire_Validate();

	/** [local + server] stop weapon fire */
	virtual void StopFire();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();
	void ServerStopFire_Implementation();
	bool ServerStopFire_Validate();

	/** [local + server] handle weapon fire */
	void HandleFiring();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandleFiring();
	void ServerHandleFiring_Implementation();
	bool ServerHandleFiring_Validate();

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon();

	/** server notified of hit from client to verify */
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerNotifyHit(const FHitResult& fireTarget);
	void ServerNotifyHit_Implementation(const FHitResult& fireTarget);
	bool ServerNotifyHit_Validate(const FHitResult& fireTarget);

	/** actually deals the damage to target */
	void HitConfirmed(const FHitResult& fireTarget);

	UFUNCTION(BlueprintCallable)
	void Reload();

	UFUNCTION(BlueprintCallable)
	void toggleVis();

	UFUNCTION(BlueprintImplementableEvent)
	void onFire();

	UFUNCTION(BlueprintImplementableEvent) 
	void onReload();

	/** consume a number of bullets */
	int32 UseAmmo(int32 ammoToUse);

	//////////////////////////////////////////////////////////////////////////
	// Weapon properties

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite);
	int ammo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int ammoPerShot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int maxAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int bulletSpread;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int fireRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int reloadSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int weaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int weight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int maxRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	uint8 weaponID;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	TEnumAsByte<DamageType> damageType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	USkeletalMeshComponent* skeleMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation);
	TArray<UAnimMontage*> montage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Animation);
	UAnimMontage* FireAnim;
};
