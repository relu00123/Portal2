// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PortalTurret.generated.h"

UCLASS()
class PORTAL_API APortalTurret : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APortalTurret();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastTakeDamageEffect(float DamageAmount, class APlayerState* InstigatorPlayerState, bool IsCritical);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> TurretMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootSceneComp;
	 

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UHitReactionComponent> HitReactionComponent;
};
