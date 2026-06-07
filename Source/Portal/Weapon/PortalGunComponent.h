// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponComponent.h"
#include "PortalGunComponent.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API UPortalGunComponent : public UWeaponComponent
{
	GENERATED_BODY()
	

	virtual void LeftClick() override; 


	virtual void OnEquip() override;

public:
	virtual void HandleHit(const FHitResult& HitResult, const FVector& ShotDirection, EMouseClickType MouseClickType) override;

	 
};

