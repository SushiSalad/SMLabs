
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

	/** set the weapon's owning pawn */
	void SetOwningPawn(ASMPlayerCharacter* NewOwner);

	//////////////////////////////////////////////////////////////////////////
	// Weapon usage

	/** [local + server] start weapon fire */
	virtual void StartFire();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerStopFire();

	/** [local + server] handle weapon fire */
	void HandleFiring();
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHandleFiring();

	UFUNCTION(BlueprintCallable)
	void Reload();

	UFUNCTION(BlueprintCallable)
	void toggleVis();

	UFUNCTION(BlueprintImplementableEvent)
	void onFire();

	UFUNCTION(BlueprintImplementableEvent) 
	void onReload();

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon();

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
	int weaponID;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int weaponRange;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite);
	int weight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	UDamageType* damageType;
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
