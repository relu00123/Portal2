// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HitReactionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTAL_API UHitReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHitReactionComponent();

	// Material 플래시를 트리거함
	UFUNCTION(BlueprintCallable, Category = "HitFlash")
	void TriggerFlash();

	UPROPERTY(EditAnywhere, Category = "HitFlash")
	UMaterialInterface* HitFlashMaterial;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


// HitFlash Section
public:
	UPROPERTY(EditAnywhere, Category = "HitFlash")
	UPrimitiveComponent* TargetMeshComponent = nullptr; // 어떤 Mesh를 타겟으로 할지

private:
	// 동적 머티리얼 인스턴스 (플래시 효과)
	UPROPERTY(Transient)
	UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	// 원본 머티리얼 (플래시 후 복원할 때 사용)
	UPROPERTY(Transient)
	UMaterialInterface* OriginalMaterial = nullptr;

	// 타이머 핸들 (시간 지나면 원복)
	FTimerHandle FlashTimerHandle;

	// 기본 파라미터
	UPROPERTY(EditAnywhere, Category = "Flash")
	FName FlashParamName = "FlashStrength";

	UPROPERTY(EditAnywhere, Category = "Flash")
	float FlashValue = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Flash")
	float FlashDuration = 0.03f;

	void ResetFlash();

		
};
