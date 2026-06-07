// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PortalGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UPortalGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

	virtual void OnStart() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "Data")
	class  UMapSpawnData* SpawnData; // 맵 스폰 데이터를 저장

	TArray<FTransform> GetSpawnLocationsForMap(FName MapName);
};
