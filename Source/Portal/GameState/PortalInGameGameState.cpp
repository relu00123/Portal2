// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/PortalInGameGameState.h"
#include "PlayerState/PortalPlayerState.h"
#include "Portal/Portal.h"

void APortalInGameGameState::BeginPlay()
{
	Super::BeginPlay();

	int32 TotalPlayers = PlayerArray.Num();

	FTimerHandle TimerHandle_CheckPlayerNames;

	// 5초 뒤에 CheckPlayerNames() 함수 실행

	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_CheckPlayerNames, this, &APortalInGameGameState::CheckPlayerNames, 20.0f, false);
	}
}

void APortalInGameGameState::CheckPlayerNames()
{
	int32 TotalPlayers = PlayerArray.Num();

	for (APlayerState* PlayerState : PlayerArray)
	{
		APortalPlayerState* MyPlayerState = Cast<APortalPlayerState>(PlayerState);
		if (MyPlayerState)
		{
			FString PlayerName = MyPlayerState->GetPlayerName();
			SCREEN_LOG(FColor::Black, "Player Name: %s", *PlayerName);
		}
	}


	
}