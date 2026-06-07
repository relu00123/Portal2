// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UDamageNumberWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetDamageText(float Damage, bool IsCritical);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayFadeAndDestroy(); // 애니메이션 끝나고 제거 
		 
};
