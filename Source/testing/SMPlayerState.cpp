#include "SMPlayerState.h"
#include "SMGameState.h"
#include "Net/UnrealNetwork.h"

ASMPlayerState::ASMPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	TeamNumber = 0;
	NumKills = 0;
	NumDeaths = 0;
	bQuitGame = false;
}

void ASMPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASMPlayerState, TeamNumber);
	DOREPLIFETIME(ASMPlayerState, NumKills);
	DOREPLIFETIME(ASMPlayerState, NumDeaths);
	DOREPLIFETIME(ASMPlayerState, bQuitGame);
}

void ASMPlayerState::CopyProperties(class APlayerState* PlayerState) {
	Super::CopyProperties(PlayerState);

	if (PlayerState) {
		ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(PlayerState);

		if (SMPlayerState) {
			SMPlayerState->TeamNumber = TeamNumber;
		}
	}
}

void ASMPlayerState::OverrideWith(class APlayerState* PlayerState) {
	Super::OverrideWith(PlayerState);

	if (PlayerState) {
		ASMPlayerState* SMPlayerState = Cast<ASMPlayerState>(PlayerState);

		if (SMPlayerState) {
			TeamNumber = SMPlayerState->TeamNumber;
		}
	}
}

int32 ASMPlayerState::GetTeamNumber() const {
	return TeamNumber;
}

void ASMPlayerState::SetTeamNumber(int32 inTeamNumber) {
	TeamNumber = inTeamNumber;
}

int32 ASMPlayerState::GetKills() const {
	return NumKills;
}

void ASMPlayerState::ScoreKill(ASMPlayerState* victim, int32 points) {
	NumKills++;
	ScorePoints(points);
}

int32 ASMPlayerState::GetDeaths() const {
	return NumDeaths;
}

void ASMPlayerState::ScoreDeath(ASMPlayerState* killedBy, int32 points) {
	NumDeaths++;
	ScorePoints(points);
}

bool ASMPlayerState::IsQuitGame() const {
	return bQuitGame;
}

void ASMPlayerState::SetQuitGame(bool bInQuitGame) {
	bQuitGame = bInQuitGame;
}

void ASMPlayerState::ScorePoints(int32 Points) {
	ASMGameState* const MyGameState = GetWorld()->GetGameState<ASMGameState>();
	if (MyGameState && TeamNumber >= 0) {
		if (TeamNumber >= MyGameState->TeamScores.Num()) {
			MyGameState->TeamScores.AddZeroed(TeamNumber - MyGameState->TeamScores.Num() + 1);
		}

		MyGameState->TeamScores[TeamNumber] += Points;
	}

	SetScore(GetScore() + Points);
}