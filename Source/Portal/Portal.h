// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

// On-Screen Debug 메시지 매크로
#define SCREEN_LOG(Color, Format, ...) \
    if (GEngine) \
    { \
        GEngine->AddOnScreenDebugMessage( \
            -1, 10.f, Color, FString::Printf(TEXT(Format), ##__VA_ARGS__) \
        ); \
    }


#define LOG_NETMODEINFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT %d"), GPlayInEditorID) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER"))) 
#define DETAIL_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogTemp, Warning, TEXT("[%s] %s"), LOG_NETMODEINFO, *FString::Printf(Format, ##__VA_ARGS__))