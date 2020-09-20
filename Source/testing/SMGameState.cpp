// Fill out your copyright notice in the Description page of Project Settings.


#include "SMGameState.h"
#include "Net/UnrealNetwork.h"

ASMGameState::ASMGameState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NumTeams = 0;
	RemainingTime = 0;
	bTimerPaused = false;
}

void ASMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASMGameState, NumTeams);
	DOREPLIFETIME(ASMGameState, TeamScores);
	DOREPLIFETIME(ASMGameState, bTimerPaused);
	DOREPLIFETIME(ASMGameState, RemainingTime);
}

//int32 ASMGameState::IncrementScore(bool Team1Scored) {
//	if (Team1Scored) {
//		return Team1Score++;
//	} else {
//		return Team2Score++;
//	}
//}
