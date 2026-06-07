// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PortalGameState.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API APortalGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override; 

public :
	void CheckAllPlayersReady();
	void StartGame();

	bool ServerMapTravel(FName MapName);

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastPlayerNameUpdate(); 
	
 

	virtual void PostInitializeComponents() override;

	UFUNCTION(Server, Reliable)
	void ServerClientJoined();

	UFUNCTION(NetMulticast, Reliable)
	void BroadcastClientJoined();

	
	 

 
};
