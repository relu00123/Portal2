// Fill out your copyright notice in the Description page of Project Settings.


#include "GameState/PortalGameState.h"
#include "PlayerState/PortalPlayerState.h"
#include "Player/PortalLobbyPlayerController.h"
#include "UI/PortalLobbyHUDWidget.h"
#include "GameInstance/PortalGameInstance.h"
#include "GameFramework/Character.h"
#include "Portal.h"

void APortalGameState::BeginPlay()
{
	Super::BeginPlay();

	//for (APlayerState* PlayerState : PlayerArray)
	//{
	//	APortalPlayerState* MyPlayerState = Cast<APortalPlayerState>(PlayerState);
	//	
	//	if (MyPlayerState)
	//	{
	//		APortalLobbyPlayerController* MyplayerController = Cast<APortalLobbyPlayerController>(MyPlayerState->GetPlayerController());

	//		// PlayerController의 BeginPlay에서.. 
	//		if (MyplayerController && MyplayerController->GetLobbyHUDWidget())
	//		{
	//			//MyPlayerState->SetupLobbyHUDWidget(MyplayerController->GetLobbyHUDWidget());
	//		}
	//	}
	//}


}

void APortalGameState::CheckAllPlayersReady()
{
	int32 ReadyCount = 0;
	int32 TotalPlayers = PlayerArray.Num();

	for (APlayerState* PlayerState : PlayerArray)
	{
		APortalPlayerState* MyPlayerState = Cast<APortalPlayerState>(PlayerState);
		if (MyPlayerState && MyPlayerState->ReadyState == EReadyState::Ready)
		{
			ReadyCount++;
		}
	}

	// Host제외 모든 플레이어가 Ready상태일때
	if (ReadyCount == TotalPlayers - 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Trying to start game!!"));
		StartGame();
	}
}

void APortalGameState::StartGame()
{
	bool isSuccess = false;

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Start!!"));

		isSuccess = ServerMapTravel(FName("Puzzle1"));

		if (isSuccess) // 클라이언트의 Lobby에서 쓴 Widget들 삭제  
		{
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				APortalLobbyPlayerController* PC = Cast<APortalLobbyPlayerController>(It->Get());
				ensure(PC);
				PC->ClientChangeToGameModeHUD(); 
			}
		}
	}
}

bool APortalGameState::ServerMapTravel(FName MapName)
{
	UE_LOG(LogTemp, Warning, TEXT("APortalGameState : ServerMapTravel Called"));

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("APortalGameState : ServerMapTravel - HasAuthority()"));


		UPortalGameInstance* GS = Cast<UPortalGameInstance>(GetWorld()->GetGameInstance());
		ensure(GS);

		UE_LOG(LogTemp, Warning, TEXT("APortalGameState : ServerMapTravel - GameInstanceFound"));

		TArray<FTransform> locations = GS->GetSpawnLocationsForMap(MapName);

		int i = 0;
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It, ++i)
		{

			UE_LOG(LogTemp, Warning, TEXT("APortalGameState : ServerMapTravel - For loop Calling, locations count : %d"), locations.Num());

			if (i >= locations.Num()) break;

			APortalLobbyPlayerController* PC = Cast<APortalLobbyPlayerController>(It->Get());
			ensure(PC);

			UE_LOG(LogTemp, Warning, TEXT("APortalGameState : ServerMapTravel - PC Found"));

			FTransform FocusedTransform = locations[i];

			ACharacter* FocusedCharacter = It->Get()->GetCharacter();

			ensure(FocusedCharacter);

			UE_LOG(LogTemp, Warning, TEXT("APortalGameState : ServerMapTravel - Character Found "));

			FocusedCharacter->SetActorTransform(FocusedTransform);

			FRotator InitControllerRotation = PC->GetControlRotation();

			PC->SetControlRotation(FRotator(InitControllerRotation.Pitch, FocusedTransform.Rotator().Yaw, InitControllerRotation.Roll));
			//FocusedCharacter->SetActorLocationAndRotation(FocusedTransform.GetLocation(), FocusedTransform.GetRotation());

			//FocusedCharacter->ClientAdjustCharacterPos()

			PC->ClientAdjustCharacterPos(FocusedTransform);

		}

		return true;
	}

	return false;
}



void APortalGameState::BroadcastPlayerNameUpdate_Implementation()
{
	/*for (APlayerState* PS : PlayerArray)
	{
		APortalPlayerState* PortalPS = Cast<APortalPlayerState>(PS);
		if (PortalPS)
		{
			for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
			{
				APortalLobbyPlayerController* controller = Cast<APortalLobbyPlayerController>(It->Get());

				UE_LOG(LogTemp, Warning, TEXT("Trying to Update PlayerName   %s"), *PortalPS->GetPlayerName());
					
				if (controller && controller->GetLobbyHUDWidget())
				{
					controller->GetLobbyHUDWidget()->UpdatePlayerName(PortalPS->GetPlayerName());
				}
			}
		}
	}*/
}

void APortalGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

 
 

void APortalGameState::BroadcastClientJoined_Implementation()
{
	 

	APortalLobbyPlayerController* pc = Cast<APortalLobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	ensure(pc);

	if (pc->IsLocalPlayerController())
	{

		UPortalLobbyHUDWidget* hud = pc->GetLobbyHUDWidget();
		ensure(hud);

		for (auto e : GetWorld()->GetGameState()->PlayerArray)
		{
			hud->UpdatePlayerName(e->GetPlayerName());
		}
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AJDFKLJASDLKFJASLDKFJasldkf"));
	}
}

void APortalGameState::ServerClientJoined_Implementation()
{
	BroadcastClientJoined();

}
 