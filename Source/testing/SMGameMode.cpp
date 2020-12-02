// Fill out your copyright notice in the Description page of Project Settings.

#include "SMGameMode.h"
#include "SMHUD.h"
#include "SMPlayerController.h"

ASMGameMode::ASMGameMode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnOb(TEXT("/Game/Blueprints/Characters/BP_SMCharacter"));
	DefaultPawnClass = PlayerPawnOb.Class;

	HUDClass = ASMHUD::StaticClass();
	PlayerControllerClass = ASMPlayerController::StaticClass();
}

//bool ASMGameMode::ReadyToStartMatch_Implementation() {
//	return Super::ReadyToStartMatch();
//	//return MaxNumPlayers == NumPlayers;
//}


void ASMGameMode::PostLogin(APlayerController* NewPlayer) {
	Super::PostLogin(NewPlayer);

	PlayerControllerList.Add(NewPlayer);
}



//void ASMGameMode::BeginPlay() {
//	Super::BeginPlay();
//	// 'FCString::Atoi' converts 'FString' to 'int32' and we use the static 'ParseOption' function of the
//	// 'UGameplayStatics' Class to get the correct Key from the 'OptionsString'
//	//MaxNumPlayers = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, “MaxNumPlayers”)));
//}
