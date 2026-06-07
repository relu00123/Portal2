// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/PortalUserWidget.h"
#include "PortalStartGameWidget.generated.h"

/**
 * 
 */

DECLARE_MULTICAST_DELEGATE(FOnButtonClicked);

UCLASS()
class PORTAL_API UPortalStartGameWidget : public UPortalUserWidget
{
	GENERATED_BODY()

public:
	UPortalStartGameWidget(const FObjectInitializer& ObjectInitializer);


	FOnButtonClicked OnButtonClickedDelegate;


protected:
	virtual void NativeConstruct() override;


protected:
	UPROPERTY()
	TObjectPtr<class UButton> Button;

	UFUNCTION()
	void HandleButtonClicked();

	void Test();
};