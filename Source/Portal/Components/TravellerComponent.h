// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TravellerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTAL_API UTravellerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTravellerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Teleport(const FTransform& FromPortal, const FTransform& ToPortal);
	void UpdateCloneTransform(const FTransform& MirrorTransform);

	UStaticMeshComponent* GetGraphicsComponent();

	// Slice용
	void SetSliceParams(float Offset, FVector Normal, bool bForClone);

	FVector GetPreviousOffset() const;
	void SetPreviousOffset(const FVector& Offset);

	UPROPERTY()
	AActor* CloneActor = nullptr;

	void CreateClone();
	void UpdateCloneTransform(const FTransform& FromPortal, const FTransform& ToPortal);
	void SetSliceParams(const FVector& SliceOrigin, const FVector& SliceNormal, bool bIsClone);

	bool HasClone() const { return CloneActor != nullptr; }
	void DestoryClone();


	// 잡힌 상태인지 확인. 잡힌상태는 서버에서만 판단하므로 Replicate필요없음. 
	UPROPERTY(BlueprintReadWrite, Category = "Portal")
	bool bIsGrabbed;


	UFUNCTION()
	FORCEINLINE bool IsTravellerGrabbed() { return bIsGrabbed; }

	UFUNCTION()
	FORCEINLINE void SetTravellerGrabbed(bool state) { bIsGrabbed = state;}


	
private:
	UStaticMeshComponent* GraphicsComponent;
	
	FVector PreviousOffset;

	// Material slice 관련 정보
	TArray<UMaterialInstanceDynamic*> OriginalMaterials;
	TArray<UMaterialInstanceDynamic*> CloneMaterials;


public : 
	UPROPERTY()
	float TeleportIgnoreDuration; // 0.1초간 무시

	float IgnoreTimeRemaining; // 남은 시간
	bool IsIgnoringOverlap() const;
};
