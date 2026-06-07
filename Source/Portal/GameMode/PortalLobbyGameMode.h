// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/GameMode.h"
#include "PortalLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API APortalLobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APortalLobbyGameMode();

private:
	int32 PlayerCount;

protected:
	virtual void BeginPlay();

	virtual void PostLogin(APlayerController* NewPlayer) override; 


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	TSubclassOf<APawn> LobbyPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	TSubclassOf<APlayerController> LobbyPawnController; 

	 
};
