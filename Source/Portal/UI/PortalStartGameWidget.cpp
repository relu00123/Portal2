// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PortalStartGameWidget.h"
#include "Components/Button.h" 
#include "Portal.h"
#include "Player/PortalLobbyPlayerController.h"

UPortalStartGameWidget::UPortalStartGameWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPortalStartGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Button = Cast<UButton>(GetWidgetFromName(TEXT("BtnStart")));
	ensure(Button);

	Button->OnClicked.AddDynamic(this, &ThisClass::HandleButtonClicked);

	//OnButtonClickedDelegate.AddUObject(this, &UPortalStartGameWidget::Test);

	// 상위계층 직접 참조라 아쉽긴한데 일단은.. 정석대로라면 interface으로 접근해야함. 
	//OnButtonClickedDelegate.AddUObject(Cast<APortalLobbyPlayerController>(GetOwningPlayer()), &APortalLobbyPlayerController::ClickOnPlayerControllerTest);
}

 
void UPortalStartGameWidget::HandleButtonClicked()
{
	// 바인딩된 모든 함수 실행 
	if (GetOwningPlayer()->HasAuthority())
	{
		//SCREEN_LOG(FColor::Blue, "Button Clicked On Server");
	}
	else
	{
		//SCREEN_LOG(FColor::Red, "Button Clicked on Client");
	}

	OnButtonClickedDelegate.Broadcast();
}

void UPortalStartGameWidget::Test()
{
	SCREEN_LOG(FColor::Green, "Test Function Called");
}
