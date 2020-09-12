// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class TESTING_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite);
	
	UFUNCTION(BlueprintCallable) void Fire();

	//UPROPERTY(EditAnywhere, BlueprintReadWrite);
	UFUNCTION(BlueprintCallable) void Reload();

	UFUNCTION(BlueprintCallable) void toggleVis();

	UFUNCTION(BlueprintImplementableEvent) void onFire();

	UFUNCTION(BlueprintImplementableEvent) void onReload();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int ammo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int maxAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int bulletSpread;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int fireRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int damage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int reloadSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	int weight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	USkeletalMeshComponent* skeleMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite);
	TArray<UAnimMontage*> montage;
};
