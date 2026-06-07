// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GrabInteractableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTAL_API UGrabInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabInteractableComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



public:
	UPROPERTY(Replicated)
	AActor* GrabbedBy;

	UFUNCTION(BlueprintCallable)
	bool CanBeGrabbedBy(AActor* Grabber) const;

	UFUNCTION(BlueprintCallable)
	void SetGrabbedBy(AActor* Grabber);

	UFUNCTION(BlueprintCallable)
	void ClearGrabbedBy();

		
};
