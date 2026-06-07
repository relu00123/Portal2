#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/DamageType.h"
#include "DamageTypes.generated.h" 

USTRUCT()
struct FExtendPointDamageEvent : public FPointDamageEvent
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool bWasCritical = false;
};