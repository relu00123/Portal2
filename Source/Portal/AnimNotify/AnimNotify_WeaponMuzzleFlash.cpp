// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_WeaponMuzzleFlash.h"
#include "Portal/Character/PortalLobbyCharacterPlayer.h"
#include "Portal/Weapon/WeaponComponent.h" // ¶Ç´Â AWeaponBase µî

void UAnimNotify_WeaponMuzzleFlash::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	UE_LOG(LogTemp, Warning, TEXT("Notify Called!"));

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	APortalLobbyCharacterPlayer* Character = Cast<APortalLobbyCharacterPlayer>(Owner);
	if (!Character) return;

	UWeaponComponent* Weapon = Character->CurrentWeapon;
	if (!Weapon) return;


	UE_LOG(LogTemp, Warning, TEXT("Trying To Fire Muzzle FX"));
	Weapon->PlayMuzzleFlash();
}
