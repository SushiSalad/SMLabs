#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SMGameMode.generated.h"

UCLASS()
class TESTING_API ASMGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ASMGameMode(const FObjectInitializer& ObjectInitializer);

	// Maximum Number of Players needed/allowed during this Match
	int32 MaxNumPlayers;
	// List of PlayerControllers
	TArray<class APlayerController*> PlayerControllerList;

	// Override Implementation of ReadyToStartMatch
	//virtual bool ReadyToStartMatch_Implementation() override;

	// Overriding the PostLogin function
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Override BeginPlay, since we need that to recreate the BP version
	//virtual void BeginPlay() override;


};
