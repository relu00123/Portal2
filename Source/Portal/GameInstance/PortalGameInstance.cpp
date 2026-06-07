// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/PortalGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Data/MapSpawnData.h"
#include "Portal.h"

void UPortalGameInstance::Init()
{
	Super::Init();
	/*if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Blue,
			FString(TEXT("Portal GameInstance Created! "))
		);
	}*/

	//if (GetWorld() && GetWorld()->GetAuthGameMode())
	//	SCREEN_LOG(FColor::Red, "Portal GameInstance Created");

	// 맵 이름 출력해보기 
	FString MapName = TEXT("Unknown");
	if (GetWorld())
	{
		MapName = UGameplayStatics::GetCurrentLevelName(GetWorld()); // 맵이름 가져오기 
	}
	 

	SCREEN_LOG(FColor::Blue, "Current Map : %s", *MapName);


	FString AssetPath = TEXT("/Game/Portal/Data/MapData/BP_MapSpawnData.BP_MapSpawnData");
	UMapSpawnData* LoadedData = LoadObject<UMapSpawnData>(nullptr, *AssetPath);

	if (LoadedData)
	{
		SpawnData = LoadedData;
		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded MapSpawnData from runtime!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load MapSpawnData! Check the path: %s"), *AssetPath);
	}
}

void UPortalGameInstance::OnStart()
{
	Super::OnStart();
	 
}

TArray<FTransform> UPortalGameInstance::GetSpawnLocationsForMap(FName MapName)
{
	if (!SpawnData) return {};

	if (const FSpawnPointList* SpawnList = SpawnData->SpawnPoints.Find(MapName))
	{
		return SpawnList->SpawnLocations; // 배열반환 
	}


	return TArray<FTransform>();
}

 
