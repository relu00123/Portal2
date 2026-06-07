// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HitReactionComponent.h"

// Sets default values for this component's properties
UHitReactionComponent::UHitReactionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...

	DynamicMaterial = nullptr;
}

void UHitReactionComponent::TriggerFlash()
{
	//if (!IsValid(TargetMeshComponent) || !IsValid(DynamicMaterial)) return;

	//if (GetWorld() == nullptr) return;

	//// 이미 플래시 중이면 다시 실행하지 않음
	//if (GetWorld()->GetTimerManager().IsTimerActive(FlashTimerHandle))
	//{
	//	return;
	//}

	//// 원래 머티리얼 저장 (처음 1회만)
	//if (!OriginalMaterial && TargetMeshComponent->GetNumMaterials() > 0)
	//{
	//	OriginalMaterial = TargetMeshComponent->GetMaterial(0);
	//}

	//// Create DynamicMaterialInstance if not already
	//if (!DynamicMaterial)
	//{
	//	DynamicMaterial = UMaterialInstanceDynamic::Create(HitFlashMaterial, this);
	//}

	//// 머티리얼을 Flash용으로 교체하고 파라미터 설정
	// 
	//if (TargetMeshComponent->GetNumMaterials() > 0)
	//{
	//	TargetMeshComponent->SetMaterial(0, DynamicMaterial);
	//	DynamicMaterial->SetScalarParameterValue(FlashParamName, FlashValue);
	//}
 //

	//// 일정 시간 뒤 원래 머티리얼로 복원
	//GetWorld()->GetTimerManager().SetTimer(
	//	FlashTimerHandle,
	//	this,
	//	&UHitReactionComponent::ResetFlash,
	//	FlashDuration,
	//	false
	//);

	// 먼저 메쉬 컴포넌트와 플래시에 쓸 머티리얼(HitFlashMaterial)이 유효한지 확인
	if (!IsValid(TargetMeshComponent) || !IsValid(HitFlashMaterial))
	{
		return;
	}

	if (GetWorld() == nullptr)
	{
		return;
	}

	// 이미 플래시 중이면 다시 실행하지 않음
	if (GetWorld()->GetTimerManager().IsTimerActive(FlashTimerHandle))
	{
		return;
	}

	// 원래 머티리얼 저장 (처음 1회만)
	if (!OriginalMaterial && TargetMeshComponent->GetNumMaterials() > 0)
	{
		OriginalMaterial = TargetMeshComponent->GetMaterial(0);
	}

	// DynamicMaterial이 아직 만들어지지 않았다면 여기서 생성
	if (!DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(HitFlashMaterial, this);
	}

	// 플래시 머티리얼로 교체하고 파라미터 적용
	if (DynamicMaterial && TargetMeshComponent->GetNumMaterials() > 0)
	{
		TargetMeshComponent->SetMaterial(0, DynamicMaterial);
		DynamicMaterial->SetScalarParameterValue(FlashParamName, FlashValue);
	}

	// 일정 시간 뒤 원래 머티리얼로 복원
	GetWorld()->GetTimerManager().SetTimer(
		FlashTimerHandle,
		this,
		&UHitReactionComponent::ResetFlash,
		FlashDuration,
		false
	);
}


void UHitReactionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!TargetMeshComponent)
	{
		TargetMeshComponent = GetOwner()->FindComponentByClass<UStaticMeshComponent>();
		if (!TargetMeshComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("HitReactionComponent: Could not find StaticMeshComponent!"));
			return;
		}
	}


	if (!OriginalMaterial && TargetMeshComponent->GetNumMaterials() > 0)
	{
		OriginalMaterial = TargetMeshComponent->GetMaterial(0);
	}

	if (HitFlashMaterial && !DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(HitFlashMaterial, this);
	}
}

// Called every frame
void UHitReactionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHitReactionComponent::ResetFlash()
{
	if (TargetMeshComponent && OriginalMaterial)
	{
		TargetMeshComponent->SetMaterial(0, OriginalMaterial);
	}

	GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);
}

