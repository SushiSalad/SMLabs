// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SMHUD.generated.h"

class AHUD;
class ASMPlayerCharacter;

UCLASS()
class TESTING_API ASMHUD : public AHUD
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTexture;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* HealthTexture;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* ArmourTexture;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* AmmoTexture;

	UPROPERTY(EditDefaultsOnly)
	UTexture2D* SpeedTexture;

	UPROPERTY(EditDefaultsOnly)
	float Offset; //General offset for HUD elements from window border.
	UPROPERTY(EditDefaultsOnly)
	float ScaleUI; //UI scaling factor for other resolutions than Full HD.

//// Font Specifics ////
	UPROPERTY()
	UFont* BigFont; //Large font - used for ammo display etc.
	UPROPERTY()
	UFont* NormalFont; //Normal font - used for death messages and such.
	FColor HUDLight; //light steel blue
	FColor HUDDark; //slate gray
	FFontRenderInfo ShadowedFont; //FontRenderInfo enabling casting shadows

public:
	ASMHUD(const FObjectInitializer& ObjectInitializer);

	//Primary draw call for the HUD.
	virtual void DrawHUD() override;

	//Draws the player's health and armour
	void DrawHealthHUD(ASMPlayerCharacter* MyPawn);

	//Draws the player's weapon image and ammo
	void DrawWeaponHUD(ASMPlayerCharacter* MyPawn);

	//Draws the player's health and armour
	void DrawCrosshairHUD();

	//Draws the hit indicator
	//void DrawHitMarkerHUD(ASMPlayerCharacter* MyPawn);

	//TODO(delle) setup player direction HUD
	//Draws the player's movement speed and direction
	void DrawMovementHUD(ASMPlayerCharacter* MyPawn);

};
