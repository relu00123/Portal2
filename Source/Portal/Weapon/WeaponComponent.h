// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Enum/PortalEnums.h"
#include "WeaponComponent.generated.h"


UCLASS( Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTAL_API UWeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;

public:
	virtual bool CanFire() const;
	virtual bool Fire();

	virtual void HandleHit(const FHitResult& HitResult, const FVector& ShotDirection, EMouseClickType MouseClickType);

protected:
	UPROPERTY(EditDefaultsOnly)
	float FireCooldown = 0.5f;

	float LastFireTime = -999.0f;


	// Hit Effect
public:
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystem* ImpactFX;

	virtual void PlayImpactEffect(FVector Location, FVector Normal);
	

public:
	virtual void LeftClick() {};

	virtual void OnEquip() {};

	virtual void PlayMuzzleFlash();
};
