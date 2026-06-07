// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup_Base.generated.h"

UCLASS()
class PORTAL_API APickup_Base : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup_Base();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 블루프린트에서 무기 종류를 지정할 변수 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite,  Category = "Pickup")
	TSubclassOf<class UWeaponComponent> WeaponComponentClass;


	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USphereComponent* CollisionComponent;  // 충돌 감지용

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class USkeletalMeshComponent* PickupMesh;  // 무기 모델

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult); 

	virtual bool GiveWeaponToPlayer(class APortalLobbyCharacterPlayer* PlayerCharacter);  // 무기 지급

	UPROPERTY(ReplicatedUsing = OnRep_IsDestroyed)
	bool bIsDestroyed = false;

	UFUNCTION()
	void OnRep_IsDestroyed();


	UFUNCTION(NetMulticast, Reliable)
	void Multicast_DestroyPickup();

};
