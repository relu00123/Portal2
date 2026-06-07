// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PortalLobbyGameMode.h"
#include "GameFramework/Pawn.h"
#include "PlayerState/PortalPlayerState.h"
#include "GameState/PortalGameState.h"
#include "Portal/Portal.h"

APortalLobbyGameMode::APortalLobbyGameMode()
{
	bUseSeamlessTravel = true; 

	PlayerCount = 0;

	DefaultPawnClass = LobbyPawn;
	PlayerControllerClass = LobbyPawnController;


	static ConstructorHelpers::FClassFinder<APawn> DefaultPawnClassRef(TEXT("/Script/Engine.Blueprint'/Game/Portal/Character/BP_PortalLobbyCharacter.BP_PortalLobbyCharacter_C'"));
	if (DefaultPawnClassRef.Class)
	{
		DefaultPawnClass = DefaultPawnClassRef.Class;
	}

	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerClassRef(TEXT("/Script/Engine.Blueprint'/Game/Portal/PlayerController/BP_PortalLobbyPlayerController.BP_PortalLobbyPlayerController_C'"));
	if (PlayerControllerClassRef.Class)
	{
		PlayerControllerClass = PlayerControllerClassRef.Class;
	}

	PlayerStateClass = APortalPlayerState::StaticClass();
	GameStateClass = APortalGameState::StaticClass();
	 
}

void APortalLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine)
	{
		GEngine->Exec(GetWorld(), TEXT("net.AllowPIESeamlessTravel 1"));
	}

	/*if (LobbyPawn)
		DefaultPawnClass = LobbyPawn;

	if (PlayerControllerClass)
		PlayerControllerClass = LobbyPawnController;*/
}

void APortalLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	bUseSeamlessTravel = true;

	if (GEngine)
	{
		GEngine->Exec(GetWorld(), TEXT("net.AllowPIESeamlessTravel 1"));
	}


	APortalPlayerState* PlayerState = NewPlayer->GetPlayerState<APortalPlayerState>();
	if (PlayerState)
	{
		PlayerCount++;
		PlayerState->SetName(FString::Printf(TEXT("Player_%d"), PlayerCount));
		//PlayerState->SetPlayerName(FString::Printf(TEXT("Player_%d"), PlayerCount));

		//FString PlayerName = PlayerState->GetPlayerName();
		//SCREEN_LOG(FColor::Black, "Player Name: %s", *PlayerName);
	}

	 
}

 