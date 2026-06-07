// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PortalUserWidget.h"
#include "Data/ReadyStateColorData.h"
#include "PortalLobbyPlayerListWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UPortalLobbyPlayerListWidget : public UPortalUserWidget
{
	GENERATED_BODY()
	
public:
	UPortalLobbyPlayerListWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	class UReadyStateColorData* ReadyStateColorData;

private:
	UPROPERTY()
	TMap<FName, class UTextBlock*> PlayerNameMap; 


	UPROPERTY()
	TMap<FName, class UImage*> PlayerStateMap;


public:
	bool SetPlayerName(FName PlayerName);

	bool SetPlayerState(FName PlayerName, EReadyState PlayerState);
};
