// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PortalLobbyHUDWidget.h"
#include "PortalStartGameWidget.h"
#include "Interface/PortalCharacterHUDInterface.h"
#include "Portal.h"
#include "UI/PortalStartGameWidget.h"
#include "UI/PortalLobbyPlayerListWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "DamageNumberWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

UPortalLobbyHUDWidget::UPortalLobbyHUDWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPortalLobbyHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartOrReadyBtn = Cast<UPortalStartGameWidget>(GetWidgetFromName(TEXT("WidgetButton")));
	ensure(StartOrReadyBtn);

	PlayerListWidget = Cast<UPortalLobbyPlayerListWidget>(GetWidgetFromName(TEXT("UserList")));
	ensure(PlayerListWidget);

	/*IPortalCharacterHUDInterface* HUDPawn = Cast<IPortalCharacterHUDInterface>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
	else
	{
		SCREEN_LOG(FColor::Blue, "Hud Setup Failed");
	}*/

	 
	// Bind Widget 사용시 자동 바인딩이라고 함.. 
	ensure(DamageCanvasPanel);

	StartOrReadyBtn->OnButtonClickedDelegate.AddUObject(this, &UPortalLobbyHUDWidget::HandleStartOrReadyBtnClicked);
}

void UPortalLobbyHUDWidget::ShowDamageTextAtLocation(const FVector& WorldLocation, float Damage, bool IsCritical)
{
	if (!DamageNumberWidgetClass || !DamageCanvasPanel)
		return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	FVector OffsetWorldLocation = WorldLocation + FVector(0.f, 0.f, 600.f);

	FVector2D ScreenPosition;
	const bool bProjected = UGameplayStatics::ProjectWorldToScreen(PC, OffsetWorldLocation, ScreenPosition);

	if (bProjected)
	{
		UDamageNumberWidget* DamageWidget = CreateWidget<UDamageNumberWidget>(PC, DamageNumberWidgetClass);
		if (DamageWidget)
		{
			DamageCanvasPanel->AddChildToCanvas(DamageWidget);
			DamageWidget->SetDamageText(Damage, IsCritical); // BP에서 구현된 함수 
			DamageWidget->PlayFadeAndDestroy();


			float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
			FVector2D AdjustedPosition = ScreenPosition / ViewportScale; // float 나눗셈 OK

			const float MaxOffsetRadius = 80.0f; // 조정 가능
			const float AngleRad = FMath::RandRange(0.f, 2 * PI);
			const float Radius = FMath::FRandRange(0.f, MaxOffsetRadius);
			FVector2D Offset = FVector2D(FMath::Cos(AngleRad), FMath::Sin(AngleRad)) * Radius;

			FVector2D FinalPosition = AdjustedPosition + Offset;

			if (UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(DamageWidget->Slot))
			{
				PanelSlot->SetAutoSize(true); // 텍스트 크기에 맞게 조절
				PanelSlot->SetAlignment(FVector2D(0.5f, 0.5f)); // 중심 기준
				PanelSlot->SetPosition(FinalPosition);

				//DrawDebugPoint(GetWorld(), WorldLocation, 10, FColor::Red, false, 2.0f);
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, FString::Printf(TEXT("ScreenPos: %s"), *AdjustedPosition.ToString()));
			}

			// 애니메이션에서 RemoveFromParent() 호출해도 되고,
			 // 혹은 아래처럼 일정 시간 후 제거도 가능
			 // FTimerHandle TimerHandle;
			// GetWorld()->GetTimerManager().SetTimer(TimerHandle, [DamageWidget]() {
			 //     if (DamageWidget) DamageWidget->RemoveFromParent();
			  // }, 1.0f, false);
		}
	}




}

void UPortalLobbyHUDWidget::UpdatePlayerName(const FString& Name)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerName Updated : %s"), *Name);

	if (PlayerListWidget)
		bool UpdateResult = PlayerListWidget->SetPlayerName(FName(*Name));
}

void UPortalLobbyHUDWidget::HandleStartOrReadyBtnClicked()
{
	if (StartOrReadyBtnDelegate.IsBound())
	{
		StartOrReadyBtnDelegate.Broadcast();
	}
}

void UPortalLobbyHUDWidget::SetReadyState(FName PlayerName, EReadyState PlayerReadyState)
{
	//UE_LOG(LogTemp, Warning, TEXT("READY STATE CHANGED IN UI"));

	if (PlayerListWidget != nullptr)
		PlayerListWidget->SetPlayerState(PlayerName, PlayerReadyState);

}

void UPortalLobbyHUDWidget::ChangeToGameModeHUD()
{
	if (StartOrReadyBtn)
		StartOrReadyBtn->SetVisibility(ESlateVisibility::Collapsed);

	if (PlayerListWidget)
		PlayerListWidget->SetVisibility(ESlateVisibility::Collapsed);
}

 