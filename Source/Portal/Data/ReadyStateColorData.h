// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enum/EReadyState.h"
#include "ReadyStateColorData.generated.h"


//UENUM(BlueprintType)
//enum class EReadyState : uint8
//{
//	Empty = 0,
//	Waiting = 1,
//	Ready = 2
//};

/**
 * 
 */
UCLASS()
class PORTAL_API UReadyStateColorData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="State Color")
	TMap<EReadyState, FLinearColor> CharacterStateColorData; 

	 
	FLinearColor GetColor(EReadyState State) const;
	
};
