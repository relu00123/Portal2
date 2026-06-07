#pragma once

#include "CoreMinimal.h"
#include "EReadyState.generated.h"

UENUM(BlueprintType)
enum class EReadyState : uint8
{
	Empty = 0,
	Waiting = 1,
	Ready = 2
};	