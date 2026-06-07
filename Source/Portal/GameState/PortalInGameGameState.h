// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "PortalInGameGameState.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API APortalInGameGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void CheckPlayerNames();
	
};
