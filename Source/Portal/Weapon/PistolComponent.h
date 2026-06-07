// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/WeaponComponent.h"
#include "PistolComponent.generated.h"


/**
 * 
 */
UCLASS()
class PORTAL_API UPistolComponent : public UWeaponComponent
{
	GENERATED_BODY()
	
	virtual void LeftClick() override;
	virtual void OnEquip() override;

private:
	// 머즐에서 재생할 파티클 FX (Cascade or Niagara)
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* MuzzleFlashFX;

	// 데미지 UI를 발동시킬 클래스
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber" , meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ADamageNumber> DamageNumberClass;

	void SpawnDamageNumber(FVector ImpactLocation, FVector ImpactNormal, float DamageAmount);


	// Damage 관련 변수들 
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float DamageVariance = 2.f; // ±2 만큼 랜덤 오차

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float CriticalChance = 0.2f; // 20% 확률로 크리티컬

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float CriticalMultiplier = 1.5f;


public:
	UPistolComponent();

	virtual void PlayMuzzleFlash() override;

	virtual void PlayImpactEffect(FVector Location, FVector Normal) override;

	virtual void HandleHit(const FHitResult& HitResult, const FVector& ShotDirection, EMouseClickType MouseClickType) override;

private:
	void CalculateFinalDamage(float& OutDamage, bool& OutWasCritical); //데미지 계산 함수 

};
