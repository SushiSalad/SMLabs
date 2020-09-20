#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SMGameState.generated.h"

UCLASS()
class TESTING_API ASMGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ASMGameState(const FObjectInitializer& ObjectInitializer);

	/** number of teams in current game (doesn't deprecate when no players are left in a team) */
	UPROPERTY(Transient, Replicated)
		int32 NumTeams;

	/** accumulated score per team */
	UPROPERTY(Transient, Replicated)
		TArray<int32> TeamScores;

	/** time left for warmup / match */
	UPROPERTY(Transient, Replicated)
		int32 RemainingTime;

	/** is timer paused? */
	UPROPERTY(Transient, Replicated)
		bool bTimerPaused;

	//Increments score to one of the teams
	//int32 IncrementScore(bool Team1Scored);


};
