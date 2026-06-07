// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PortalCharacterBase.h"
#include "InputActionValue.h"
#include "Enum/PortalEnums.h"
#include "PortalLobbyCharacterPlayer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponMeshChanged, UMeshComponent*, NewWeaponMesh);


/**
 * 
 */
UCLASS()
class PORTAL_API APortalLobbyCharacterPlayer : public APortalCharacterBase
{
	GENERATED_BODY()

public:
	APortalLobbyCharacterPlayer();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override; 


// Input Section 
protected :
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	class UInputMappingContext* GameplayMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* FireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* RightFireAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchWeaponToPortalgunAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchWeaponToRifleAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* GrabAction;


	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* SwitchMaterialAction;


// Grab Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grab", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UPhysicsHandleComponent> PhysicsHandle;

	// 현재 잡고 있는 컴포넌트를 저장하는 변수  (PhysicsHandle은 Component를 집는다) 
	UPROPERTY(Replicated, Transient)
	TObjectPtr<class UPrimitiveComponent> HeldComponent;

	void GrabObject(const FInputActionValue& Value);

	void SwitchMaterial(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerGrabObject();

	UFUNCTION(Server, Reliable)
	void ServerReleaseObject();

	void TryGrabObject();
	void UpdateGrabbedObjectLocation();

// Character Section 
protected :
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "AimOffset")
	float AimPitch;

public :
	FORCEINLINE float GetAimPitch() const { return AimPitch;  }

	void Look(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void ServerUpdateAimPitch(float NewPitch);
	
// Weapon Section
public :
	UFUNCTION()
	bool EquipWeapon(class UWeaponComponent* NewWeapon); // 무기 장착함수

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<class UWeaponComponent> CurrentWeapon;

	UPROPERTY()
	TObjectPtr<class USkeletalMeshComponent> MirrorWeaponMesh;

	UFUNCTION()
	void OnRep_CurrentWeapon();


// Delegate Section
public :
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnWeaponMeshChanged OnWeaponMeshChanged;


public:
	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<class UWeaponComponent> PistolWeapon;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Weapon")
	TObjectPtr<class UWeaponComponent> PortalGunWeapon;

	void SwitchToRifle(const FInputActionValue& Value);

	void SwitchToPortalGun(const FInputActionValue& Value);


	void SwitchWeapon(EWeaponType weaponType);

	UFUNCTION(Server, Reliable)
	void ServerSwitchWeapon(EWeaponType weaponType);

	void HideWeapon(EWeaponType weaponType);


// Weapon Aiming
	UPROPERTY(Replicated)
	bool bIsAiming; 

	UFUNCTION(Server, Reliable)
	void ServerSetIsAiming(bool bNewAiming);

	void StartAiming(const FInputActionValue& Value);
	void StopAiming(const FInputActionValue& Value);

// Weapon Fire Common
private:
	void HandleWeaponFire(EMouseClickType ClickType); // 서버에서만 호출되는 함수 


// Weapon Fire
	UFUNCTION(Server, Reliable)
	void ServerFireRifle();

	void FireRifle();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFireRifle();
	 

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireMontage_FirstPerson;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireMontage_ThirdPerson;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayImpactEffect(FVector ImpactPoint, FVector ImpactNormal);


// Weapon Right Fire (Orange Portal)
	UFUNCTION(Server, Reliable)
	void ServerFireRightRifle();

	void FireRightRifle();


// Mesh Section 
public :
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> FirstPersonMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> ThirdPersonMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UCameraComponent> FirstPersonCamera;


// IK
	UFUNCTION(BlueprintCallable)
	FTransform GetLeftHandIKTransform();



// Portal 
public :
	UPROPERTY(Replicated)
	TObjectPtr<class APortalActor> OwningPortal; 

	UPROPERTY(EditDefaultsOnly, Category = "Portal")
	TSubclassOf<class APortalActor> PortalActorClass;


	class APortalActor* GetOrCreatePortalActor();


// TravellerComponent
public : 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UTravellerComponent> TravellerComponent;


	UFUNCTION(Client, Reliable)
	void Client_SetControlRotation(const FRotator& NewControlRot);

// Portal LerP
	UPROPERTY()
	FRotator InitialRotation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	float BlendAlpha = 0.f;

	UPROPERTY()
	bool bShouldBlendRotation = false;



};
