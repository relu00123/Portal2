// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PortalLobbyPlayerListWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

 

UPortalLobbyPlayerListWidget::UPortalLobbyPlayerListWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPortalLobbyPlayerListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ReadyStateColorData = LoadObject<UReadyStateColorData>(nullptr, TEXT("/Game/Portal/UI/BP_ReadyStateColorData.BP_ReadyStateColorData"));
	ensure(ReadyStateColorData);


	for (int i = 1; i <= 4; ++i)
	{
		FString PlayerNameString = FString::Printf(TEXT("Player_%d"), i); // 문자열조합
		FString PlayerStateString = FString::Printf(TEXT("PlayerState_%d"), i);

		FName PlayerName = FName(*PlayerNameString);
		FName PlayerState = FName(*PlayerStateString);

		UTextBlock* PlayerTextBlock = Cast<UTextBlock>(GetWidgetFromName(PlayerName));
		ensure(PlayerTextBlock);
		PlayerNameMap.Add(PlayerName, PlayerTextBlock);

		UImage* PlayerStateImage = Cast<UImage>(GetWidgetFromName(PlayerState));
		ensure(PlayerStateImage);
		PlayerStateMap.Add(PlayerName, PlayerStateImage);
	}
}

bool UPortalLobbyPlayerListWidget::SetPlayerName(FName PlayerName)
{
	UTextBlock** TextBlockptr = PlayerNameMap.Find(PlayerName);

	if (TextBlockptr)
	{
		(*TextBlockptr)->SetText(FText::FromString(PlayerName.ToString()));
		return true;
	}

	else
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock not found UPortalLobbyPlayerListWidget") );
	}

	return false;
}

bool UPortalLobbyPlayerListWidget::SetPlayerState(FName PlayerName, EReadyState PlayerState)
{
	UImage** ImageBlockptr = PlayerStateMap.Find(PlayerName);

	if (ImageBlockptr)
	{
		(*ImageBlockptr)->SetColorAndOpacity(ReadyStateColorData->GetColor(PlayerState));
	}

	return false;
}
