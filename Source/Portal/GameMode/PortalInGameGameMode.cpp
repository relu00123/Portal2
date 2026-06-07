// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/PortalInGameGameMode.h"
#include "PlayerState/PortalPlayerState.h"
#include "GameState/PortalInGameGameState.h"

APortalInGameGameMode::APortalInGameGameMode()
{
	bUseSeamlessTravel = true;

	PlayerStateClass = APortalPlayerState::StaticClass();
	GameStateClass = APortalInGameGameState::StaticClass();
}

void APortalInGameGameMode::BeginPlay()
{
	Super::BeginPlay();

    if (GEngine)
    {
        GEngine->Exec(GetWorld(), TEXT("net.AllowPIESeamlessTravel 1"));
    }


}

void APortalInGameGameMode::HandleSeamlessTravelPlayer(AController*& controller)
{
    UE_LOG(LogTemp, Log, TEXT("HandleSeamlessTravelPlayer() was called!"));

   /* APlayerController* PC = Cast<APlayerController>(controller);
    if (PC)
    {
        APortalPlayerState* OldPlayerState = PC->GetPlayerState<APortalPlayerState>();

        if (OldPlayerState)
        {
            UE_LOG(LogTemp, Log, TEXT("SeamlessTravel: Keeping PlayerState for %s"), *OldPlayerState->GetPlayerName());
        }
    }*/

    Super::HandleSeamlessTravelPlayer(controller);
}
