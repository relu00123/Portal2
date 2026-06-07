// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerState/PortalPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "UI/PortalLobbyHUDWidget.h"
#include "Player/PortalLobbyPlayerController.h"
#include "GameState/PortalGameState.h"
#include "Portal/Portal.h"

void APortalPlayerState::BeginPlay()
{
	Super::BeginPlay();


}

APortalPlayerState::APortalPlayerState()
{
	//bIsReady = false;
	ReadyState = EReadyState::Empty;
}

void APortalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortalPlayerState, ReadyState);
	//DOREPLIFETIME(APortalPlayerState, PlayerName);
}

void APortalPlayerState::SetName(const FString& NewName)
{
	SetPlayerName(NewName);
	//PlayerName = NewName;

	//if (HasAuthority())
	//	OnRep_MyPlayerName();
	////PlayerName = NewName;
	////OnPlayerNameChanged.Broadcast(NewName);
	//if (HasAuthority())
	//{
	//	APortalGameState* GameState = GetWorld()->GetGameState<APortalGameState>();
	//	if (GameState)
	//	{
	//		GameState->BroadcastPlayerNameUpdate();
	//	}
	//}

}

void APortalPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	// 정확한 시점을 몰라서 1초뒤에 이름을 바꾸도록 하였음. 
	FTimerHandle TimerHandle_ClientJoin;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientJoin, this, &APortalPlayerState::ClientNameChanged, 1.f, false);
}

void APortalPlayerState::OnRep_ReadyState()
{
	//UE_LOG(LogTemp, Warning, TEXT("ReadyState Changed!!"));

	APortalLobbyPlayerController* pc = Cast<APortalLobbyPlayerController>(GetWorld()->GetFirstPlayerController());

	if (pc && pc->IsLocalPlayerController())
	{
		if (pc->GetLobbyHUDWidget() != nullptr)
			pc->GetLobbyHUDWidget()->SetReadyState(FName(*GetPlayerName()),  ReadyState);
	}
}

void APortalPlayerState::ClientNameChanged()
{
	if (GetPlayerController())
	{
		APortalGameState* GS = Cast<APortalGameState>(GetWorld()->GetGameState());
		if (GS)
		{
			GS->ServerClientJoined();
		}
	}
}

//void APortalPlayerState::OnRep_MyPlayerName()
//{
//	//APortalLobbyPlayerController* pc = Cast<APortalLobbyPlayerController>(GetPlayerController());
//	//if (pc && pc->GetLobbyHUDWidget())
//	//{
//	//	pc->GetLobbyHUDWidget()->UpdatePlayerName(PlayerName);
//	//}
//
//	 
//}



void APortalPlayerState::CopyProperties(APlayerState* NewPlayerState)
{
	Super::CopyProperties(NewPlayerState);

	//if (NewPlayerState != nullptr)
	//{

	//	UE_LOG(LogTemp, Log, TEXT("CopyPropertiesCalled!!!!!!!!!!"));
	//	UE_LOG(LogTemp, Log, TEXT("PlayerName : %s"), *GetPlayerName());


	//	// Called on old Player State. inComing Argument is New PlayerState
	//	NewPlayerState->SetPlayerName(GetPlayerName());
	//}
}

void APortalPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(LogTemp, Warning, TEXT("PlayerState PostInitializecomponents Called"));

	//APortalGameState* GS = Cast<APortalGameState>(GetWorld()->GetGameState());
	//ensure(GS);

	//UE_LOG(LogTemp, Warning, TEXT("Player Num Called From PlayerState : %d"), GS->PlayerArray.Num());

	//if (GetPlayerController()->IsLocalPlayerController())
	//{
	//	APortalLobbyPlayerController* LobbyPC = Cast<APortalLobbyPlayerController>(GetPlayerController());

	//	if (LobbyPC)
	//	{
	//		for (TObjectPtr<APlayerState> e : GS->PlayerArray)
	//		{
	//			LobbyPC->GetLobbyHUDWidget()->UpdatePlayerName(e->GetPlayerName());
	//		}
	//	}

	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("LobbyPC Doesnot Exist"));
	//	}
	//}

	 
	
}

 

//void APortalPlayerState::SetupLobbyHUDWidget(UPortalLobbyHUDWidget* InHUDWidget)
//{
//	/*if (InHUDWidget)
//	{
//		OnPlayerNameChanged.AddUObject(InHUDWidget, &UPortalLobbyHUDWidget::UpdatePlayerName);
//		InHUDWidget->UpdatePlayerName(GetName());
//	}*/
//}

 