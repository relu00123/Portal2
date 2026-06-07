// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Enum/PortalEnums.h"
#include "PortalThirdPlayerAnimation.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UPortalThirdPlayerAnimation : public UAnimInstance
{
	GENERATED_BODY()

 
public:
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    FVector Velocity;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float GroundSpeed;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float AimPitch;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    bool bShouldMove;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    bool bIsFalling;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
    bool HasRifle;

    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AnimVariables")
    EWeaponType WeaponType;

    UPROPERTY(BlueprintReadWrite, Category = "Movement")
    float Direction;


    UPROPERTY(BlueprintReadWrite, Category = "AnimVariables")
    FTransform EffectTransform;


    UPROPERTY(BlueprintReadWrite, Category = "Reference")
    class APortalLobbyCharacterPlayer* OwningPawn;

protected :
    virtual void NativeInitializeAnimation() override;
	
};
