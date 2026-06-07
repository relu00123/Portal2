// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageNumber.generated.h"

UCLASS(Blueprintable)
class PORTAL_API ADamageNumber : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageNumber();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitDamage(float DamageAmount);

private :
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* WidgetComponent;

	float ElapsedTime = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float LifeTime = 1.0f;

	UPROPERTY(EditDefaultsOnly)
	float FloatSpeed = 30.0f;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> DamageNumberWidgetClass;

	class UDamageNumberWidget* WidgetInstance;

	void UpdateFaceToCamera();

};
