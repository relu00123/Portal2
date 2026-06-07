// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 내부 데이터 복제 허용 
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UWeaponComponent::CanFire() const
{
	return (GetWorld()->TimeSeconds - LastFireTime) >= FireCooldown;
}

bool UWeaponComponent::Fire()
{
	if (!CanFire()) return false;

	LastFireTime = GetWorld()->TimeSeconds;

	return true;
	 
}

void UWeaponComponent::HandleHit(const FHitResult& HitResult, const FVector& ShotDirection, EMouseClickType MouseClickType)
{
}

void UWeaponComponent::PlayImpactEffect(FVector Location, FVector Normal)
{
	if (!ImpactFX) return;

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ImpactFX,
		Location,
		Normal.Rotation()
	);
}

void UWeaponComponent::PlayMuzzleFlash()
{
}

 
