// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MapSpawnData.generated.h"

USTRUCT(BlueprintType)
struct FSpawnPointList
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTransform> SpawnLocations;
};

/**
 * 
 */
UCLASS(BlueprintType)
class PORTAL_API UMapSpawnData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FName, FSpawnPointList> SpawnPoints;
};
