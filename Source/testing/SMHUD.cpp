// Fill out your copyright notice in the Description page of Project Settings.

#include "SMHUD.h"
#include "SMPlayerCharacter.h"

#include "Engine/Canvas.h" 

bool DEBUG_HUD = true;
#define LOG if(DEBUG_HUD) UE_LOG

ASMHUD::ASMHUD(const FObjectInitializer& ObjectInitializer) : AHUD(ObjectInitializer) {
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTextureOb(TEXT("/Game/UI/HUD/crosshair"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> HealthTextureOb(TEXT("/Game/UI/HUD/health"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ArmourTextureOb(TEXT("/Game/UI/HUD/shield"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> AmmoTextureOb(TEXT("/Game/UI/HUD/ammo"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> SpeedTextureOb(TEXT("/Game/UI/HUD/sprinting_man"));
	CrosshairTexture = CrosshairTextureOb.Object;
	HealthTexture = HealthTextureOb.Object;
	ArmourTexture = ArmourTextureOb.Object;
	AmmoTexture = AmmoTextureOb.Object;
	SpeedTexture= SpeedTextureOb.Object;

	static ConstructorHelpers::FObjectFinder<UFont> BigFontOb(TEXT("/Game/UI/Roboto51"));
	static ConstructorHelpers::FObjectFinder<UFont> NormalFontOb(TEXT("/Game/UI/Roboto18"));
	BigFont = BigFontOb.Object;
	NormalFont = NormalFontOb.Object;

	Offset = 25.f;
	HUDLight = FColor(175, 202, 213, 255); 
	HUDDark = FColor(110, 124, 131, 255); 
	ShadowedFont.bEnableShadow = true;
}

void ASMHUD::DrawHUD() {
	AHUD::DrawHUD();

	if (Canvas == nullptr) {
		LOG(LogTemp, Error, TEXT("SMHUD-ERROR: Missing Canvas"));
		return;
	}
	ScaleUI = Canvas->ClipY / 1080.f;

	ASMPlayerCharacter* MyPawn = Cast<ASMPlayerCharacter>(GetOwningPawn());
	if (MyPawn != nullptr) {
		DrawHealthHUD(MyPawn);
		DrawWeaponHUD(MyPawn);
		DrawCrosshairHUD();
		DrawMovementHUD(MyPawn);
	} else {
		LOG(LogTemp, Error, TEXT("SMHUD-ERROR: There was no owning pawn for %d"), this);
	}
}

void ASMHUD::DrawHealthHUD(ASMPlayerCharacter* MyPawn) {
	if (BigFont && HealthTexture && ArmourTexture) {
		Canvas->SetDrawColor(FColor::White);
		//armour icon
		FVector2D ArmourIconDrawPos(Canvas->ClipX - (Offset - ArmourTexture->GetSizeX()) * ScaleUI,
									Canvas->ClipY - (Offset - ArmourTexture->GetSizeY()) * ScaleUI);
		FCanvasTileItem ArmourItem(ArmourIconDrawPos, ArmourTexture->Resource, FLinearColor::White);
		ArmourItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(ArmourItem);

		//text
		FString healthText = " " + FString::FromInt((int)MyPawn->Health) + " | " + FString::FromInt((int)MyPawn->Armor) + " ";
		int32 HealthFontWidth, HealthFontHeight;
		BigFont->GetStringHeightAndWidth(healthText, HealthFontHeight, HealthFontWidth);
		FVector2D TextDrawPos(ArmourIconDrawPos.X - HealthFontWidth * ScaleUI, ArmourIconDrawPos.Y);
		FCanvasTextItem TextItem(TextDrawPos, FText::FromString(healthText), BigFont, HUDLight);
		TextItem.EnableShadow(FLinearColor::Black);
		//float TextScale = .5f;
		Canvas->DrawItem(TextItem);

		//health icon
		FVector2D HealthIconDrawPos(TextDrawPos.X - HealthTexture->GetSizeX() * ScaleUI, ArmourIconDrawPos.Y);
		FCanvasTileItem HealthItem(HealthIconDrawPos, HealthTexture->Resource, FLinearColor::White);
		HealthItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(HealthItem);
	} else { LOG(LogTemp, Error, TEXT("SMHUD-ERROR: Missing BigFont %d, HealthTexture %d, or ArmourTexture %d"), BigFont, HealthTexture, ArmourTexture); }
}

void ASMHUD::DrawWeaponHUD(ASMPlayerCharacter* MyPawn) {
	
}

void ASMHUD::DrawCrosshairHUD() {
	if (CrosshairTexture) {
		// Find the center of our canvas.
		FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

		// Offset by half of the texture's dimensions so that the center of the texture aligns with the center of the Canvas.
		FVector2D CrossHairDrawPosition(Center.X - (CrosshairTexture->GetSurfaceWidth() * 0.5f), Center.Y - (CrosshairTexture->GetSurfaceHeight() * 0.5f));

		// Draw the crosshair at the centerpoint.
		FCanvasTileItem TileItem(CrossHairDrawPosition, CrosshairTexture->Resource, FLinearColor::White);
		TileItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(TileItem);
	}
}

void ASMHUD::DrawMovementHUD(ASMPlayerCharacter* MyPawn) {
	
}