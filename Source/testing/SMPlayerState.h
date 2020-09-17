#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SMPlayerState.generated.h"

UCLASS()
class TESTING_API ASMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ASMPlayerState(const FObjectInitializer& ObjectInitializer);

	// Used to copy properties from the current PlayerState to the passed one
	virtual void CopyProperties(class APlayerState* PlayerState) override;
	// Used to override the current PlayerState with the properties of the passed one
	virtual void OverrideWith(class APlayerState* PlayerState) override;

	int32 GetTeamNumber() const;
	void SetTeamNumber(int32 inTeamNumber);

	int32 GetKills() const;
	void ScoreKill(ASMPlayerState* victim, int32 points);

	int32 GetDeaths() const;
	void ScoreDeath(ASMPlayerState* killedBy, int32 points);

	bool IsQuitGame() const;
	void SetQuitGame(bool bInQuitGame);

protected:
	UPROPERTY(Transient, Replicated)
		int32 TeamNumber;

	UPROPERTY(Transient, Replicated)
		int32 NumKills;

	UPROPERTY(Transient, Replicated)
		int32 NumDeaths;

	/** whether the user quit the match */
	UPROPERTY(Transient, Replicated)
		bool bQuitGame;

	void ScorePoints(int32 points);

};
