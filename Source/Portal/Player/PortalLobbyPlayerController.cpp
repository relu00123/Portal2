// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PortalLobbyPlayerController.h"
#include "UI/PortalLobbyHUDWidget.h"
#include "GameState/PortalGameState.h"
#include "PlayerState/PortalPlayerState.h"
#include "Portal.h"
#include "Portal/Portal.h"
#include "GameFramework/Character.h"
#include "Enum/EReadyState.h"

APortalLobbyPlayerController::APortalLobbyPlayerController()
{
	static ConstructorHelpers::FClassFinder<UPortalLobbyHUDWidget> PortalLobbyHUDWidgetRef(TEXT("/Game/Portal/UI/WBP_PortalLobbyHUD.WBP_PortalLobbyHUD_C"));

	if (PortalLobbyHUDWidgetRef.Class)
	{
		PortalLobbyHUDWidgetClass = PortalLobbyHUDWidgetRef.Class;
	}

}

void APortalLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		PortalLobbyHUDWidget = Cast<UPortalLobbyHUDWidget>(CreateWidget(this, PortalLobbyHUDWidgetClass));

		if (!PortalLobbyHUDWidget) return;

		PortalLobbyHUDWidget->AddToViewport();

		//PortalLobbyHUDWidget->GetStartOrReadyBtn()
		PortalLobbyHUDWidget->StartOrReadyBtnDelegate.AddUObject(this, &APortalLobbyPlayerController::HandleStartOrReadyButton);



		APortalGameState* GS =  Cast<APortalGameState>(GetWorld()->GetGameState());
		ensure(GS);
		//GS->testfunc(this);


		DETAIL_LOG(LogTemp, Warning, TEXT("LocalPlayerBeginPlay Called(HUD Attached)"));

		// 이곳에서 몇초뒤에 서버에게 요청하는 함수를 만들어보자..
		FTimerHandle TimerHandle_ClientJoin;
		 

		// 5초 뒤에 CheckPlayerNames() 함수 실행

	/*	if (IsLocalPlayerController())
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_ClientJoin, GS, &APortalGameState::ServerClientJoined, 5.0f, false);
		}*/

	}


	 
}

void APortalLobbyPlayerController::ServerSetReady_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("APortalLobbyPlayerController :  ServerSetReady_Implementation Called"));
	APortalGameState* GameState = GetWorld()->GetGameState<APortalGameState>();
	if (GameState)
	{
		APortalPlayerState* MyPlayerState = GetPlayerState<APortalPlayerState>();
		if (MyPlayerState)
		{
			if (MyPlayerState->ReadyState == EReadyState::Ready)
			{
				MyPlayerState->ReadyState = EReadyState::Waiting;

			}

			else
			{
				MyPlayerState->ReadyState = EReadyState::Ready;
			}


			if (HasAuthority())
			{
				MyPlayerState->OnRep_ReadyState();
			}
		}
	}
}

void APortalLobbyPlayerController::HandleStartOrReadyButton()
{
	if (HasAuthority())
	{
		APortalGameState* GameState = GetWorld()->GetGameState<APortalGameState>();
		if (GameState)
		{
			GameState->CheckAllPlayersReady(); // 모든 플레이어가 준비되었는지 확인 후 게임 시작
		}
	}

	else
	{
		ServerSetReady();  // 서버에게 준비완료 전송 
	}
}

void APortalLobbyPlayerController::ClickOnPlayerControllerTest()
{
	SCREEN_LOG(FColor::Blue, "Controller Log Test");
}

void APortalLobbyPlayerController::OnRep_PlayerState()
{
	// PlayerState의 Delegate에 Widget Bidning작업 시작
	//ensure(PortalLobbyHUDWidget);
	//GetPlayerState<APortalPlayerState>()->SetupLobbyHUDWidget(PortalLobbyHUDWidget);

	 
}

void APortalLobbyPlayerController::ClientChangeToGameModeHUD_Implementation()
{
	if (PortalLobbyHUDWidget)
	{
		PortalLobbyHUDWidget->ChangeToGameModeHUD();
	}
}

void APortalLobbyPlayerController::ClientAdjustCharacterPos_Implementation(FTransform FocusedTransform)
{


	ACharacter* FocusedCharacter = GetCharacter();
	ensure(FocusedCharacter);

	FocusedCharacter->SetActorTransform(FocusedTransform);

	FRotator InitControllerRotation = GetControlRotation();

	SetControlRotation(FRotator(InitControllerRotation.Pitch, FocusedTransform.Rotator().Yaw, InitControllerRotation.Roll));
	//FocusedCharacter->SetActorLocationAndRotation(FocusedTransform.GetLocation(), FocusedTransform.GetRotation());


	// 음... 몰루! 
	FTimerHandle Timer;
	GetWorldTimerManager().SetTimer(Timer, FTimerDelegate::CreateLambda([FocusedCharacter, FocusedTransform]()
		{
			if (FocusedCharacter) // nullptr 체크
			{
				FocusedCharacter->SetActorTransform(FocusedTransform);
				FocusedCharacter->SetActorRotation(FocusedTransform.Rotator());
			}
		}), 0.2f, false);
}
