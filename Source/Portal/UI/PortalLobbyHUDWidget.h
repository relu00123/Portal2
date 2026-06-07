// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/ReadyStateColorData.h"
#include "PortalLobbyHUDWidget.generated.h"

/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FStartOrReadyBtnClicked);

UCLASS()
class PORTAL_API UPortalLobbyHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPortalLobbyHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	TObjectPtr<class UPortalStartGameWidget> StartOrReadyBtn;

	UPROPERTY()
	TObjectPtr<class UPortalLobbyPlayerListWidget> PlayerListWidget;


	// Damage UI
protected:
	// 데미지 숫자들을 붙일 캔버스
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* DamageCanvasPanel;

	// 데미지 위젯 클래스 (WBP_DamageNumber)
	UPROPERTY(EditDefaultsOnly, Category = "DamageUI")
	TSubclassOf<class UDamageNumberWidget> DamageNumberWidgetClass;

public: 
	UFUNCTION(BlueprintCallable)
	void ShowDamageTextAtLocation(const FVector& WorldLocation, float Damage, bool IsCritical);


public:
	FStartOrReadyBtnClicked StartOrReadyBtnDelegate; 

	void UpdatePlayerName(const FString& Name);

	UFUNCTION()
	void HandleStartOrReadyBtnClicked();


	void SetReadyState(FName PlayerName, EReadyState PlayerReadyState);

	void ChangeToGameModeHUD();
};
