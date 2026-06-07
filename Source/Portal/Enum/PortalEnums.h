#pragma once

#include "CoreMinimal.h"
#include "PortalEnums.generated.h"   

#define COLLISION_PORTAL_TRACE ECC_GameTraceChannel1
#define COLLISION_PORTAL_RAY ECC_GameTraceChannel2
#define COLLISION_PORTAL_SURFACE ECC_GameTraceChannel3

#define COLLISION_PORTAL_BLOCKER ECC_WorldDynamic
#define COLLISION_IGNORE_PORTAL_RAY ECC_WorldDynamic

UENUM(BlueprintType)
enum class EDoorType : uint8
{
	BLUE_DOOR,
	ORANGE_DOOR,
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EMPTY,
	PORTAL_GUN,
	RIFLE,
};

UENUM(BlueprintType)
enum class EMouseClickType : uint8
{
	MOUSE_NONE,
	MOUSE_LEFT,
	MOUSE_RIGHT
};

 