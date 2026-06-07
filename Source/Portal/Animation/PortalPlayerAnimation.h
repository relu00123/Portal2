// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enum/PortalEnums.h"
#include "PortalPlayerAnimation.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UPortalPlayerAnimation : public UAnimInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
	bool IsMoving;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
	bool IsInAir;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
	bool HasRifle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
	class APortalLobbyCharacterPlayer* FirstPersonCharacter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
	EWeaponType WeaponType;

	UPROPERTY(BlueprintReadWrite, Category = "Reference")
	class APortalLobbyCharacterPlayer* OwningPawn;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsAming;

public:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	virtual void NativeInitializeAnimation() override;
};
