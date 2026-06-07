// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalBlockingActor.generated.h"

UCLASS()
class PORTAL_API APortalBlockingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalBlockingActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	UPROPERTY(VisibleAnywhere)
	class UBoxComponent* BlockingBox;

	//UPROPERTY(VisibleAnywhere)
	//UStaticMeshComponent* DebugMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
