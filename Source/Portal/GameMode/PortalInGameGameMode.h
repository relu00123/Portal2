// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PortalInGameGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API APortalInGameGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	APortalInGameGameMode();
	
protected:
	virtual void BeginPlay();

public:
	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

};
