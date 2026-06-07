// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PistolComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"            // UParticleSystem
#include "Particles/ParticleSystemComponent.h"   // UParticleSystemComponent
#include "UIActor/DamageNumber.h"
#include "Structs/DamageTypes.h"

void UPistolComponent::LeftClick()
{
}

void UPistolComponent::OnEquip()
{
}

void UPistolComponent::SpawnDamageNumber(FVector ImpactLocation, FVector ImpactNormal, float DamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawn Damage Number Called (PistolComponent.cpp)"));

	// 랜덤 원 생성 
	float Radius = 30.0f;
	float Angle = FMath::RandRange(0.f, 2 * PI);
	float Dist = FMath::RandRange(0.f, Radius);

	FVector LocalOffset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * Dist;
	FQuat AlignToSurface = FQuat::FindBetweenNormals(FVector::UpVector, ImpactNormal);
	FVector FinalOffset = AlignToSurface.RotateVector(LocalOffset);


	// 1. UI가 향할 방향은 ImpactNormal의 반대
	FVector UIForward = -ImpactNormal;
	FVector UIUp = FVector::UpVector; // 위 방향은 보통 월드 Up

	// 2. 회전 계산 (Forward, Up 기반으로)
	FMatrix RotationMatrix = FRotationMatrix::MakeFromXZ(UIForward, UIUp);
	FRotator WidgetRotation = RotationMatrix.Rotator();


	//FVector SpawnLocation = ImpactLocation + FinalOffset;
	FVector SpawnLocation = ImpactLocation + FinalOffset + ImpactNormal * 100.0f;

 

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (DamageNumberClass)
	{
	/*	ADamageNumber* DamageActor = GetWorld()->SpawnActor<ADamageNumber>(
			DamageNumberClass, SpawnLocation, FRotator::ZeroRotator, Params);*/

		ADamageNumber* DamageActor = GetWorld()->SpawnActor<ADamageNumber>(
			DamageNumberClass, SpawnLocation, WidgetRotation, Params);

		if (DamageActor)
		{
			DamageActor->InitDamage(DamageAmount);
		}
	}
}


UPistolComponent::UPistolComponent()
{


	FireCooldown = 0.1f;
}

void UPistolComponent::CalculateFinalDamage(float& OutDamage, bool& OutWasCritical)
{
	float RandomOffset = FMath::FRandRange(-DamageVariance, DamageVariance);
	float FinalDamage = BaseDamage + RandomOffset;

	OutWasCritical = FMath::FRand() < CriticalChance;

	if (OutWasCritical)
	{
		FinalDamage *= CriticalMultiplier;
		 
		UE_LOG(LogTemp, Warning, TEXT("Critical Hit!"));
	}

	OutDamage = FMath::RoundToInt(FinalDamage); // 정수로 변환
}


void UPistolComponent::PlayMuzzleFlash()
{
	Super::PlayMuzzleFlash();

	if (!MuzzleFlashFX) return;

	FVector Location = GetSocketLocation("MuzzleFlash");
	FRotator Rotation = GetSocketRotation("MuzzleFlash");

	//UParticleSystemComponent* FX =  UGameplayStatics::SpawnEmitterAttached(
	//	MuzzleFlashFX,
	//	this,
	//	"MuzzleFlash",  // Attach to socket
	//	FVector::ZeroVector,
	//	FRotator::ZeroRotator,
	//	EAttachLocation::SnapToTarget,
	//	true
	//);

	//if (FX)
	//{
	//	FX->SetIsReplicated(false);
	//}


	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		MuzzleFlashFX,
		Location,
		Rotation,
		true  // bAutoDestroy
	);

	// UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlashFX, Location, Rotation);
}

void UPistolComponent::PlayImpactEffect(FVector Location, FVector Normal)
{
	Super::PlayImpactEffect(Location, Normal);

	// SpawnDamageNumber(Location, Normal, 10);
}

void UPistolComponent::HandleHit(const FHitResult& HitResult, const FVector& ShotDirection, EMouseClickType MouseClickType)
{
	Super::HandleHit(HitResult, ShotDirection, MouseClickType);

	AActor* HitActor = HitResult.GetActor();
 
	if (HitActor)
	{
		float FinalDamage = 0.f;
		bool bWasCritical = false;

		CalculateFinalDamage(FinalDamage, bWasCritical);

		// 확장된 데미지 이벤트 생성
		FExtendPointDamageEvent DamageEvent;
		DamageEvent.Damage = FinalDamage;
		DamageEvent.ShotDirection = ShotDirection;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.bWasCritical = bWasCritical;

		HitActor->TakeDamage(
			FinalDamage,
			DamageEvent,
			GetOwner()->GetInstigatorController(),
			GetOwner()
		);
	}
}
