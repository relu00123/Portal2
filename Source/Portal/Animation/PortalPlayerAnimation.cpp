// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PortalPlayerAnimation.h"
#include "Character/PortalLobbyCharacterPlayer.h"

void UPortalPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (OwningPawn)
	{
		bIsAming = OwningPawn->bIsAiming;

		if (bIsAming)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Currently Aiming"));
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Currently Not Aiming"));
		}
	}
}

void UPortalPlayerAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	APortalLobbyCharacterPlayer* OwnerPawn = Cast<APortalLobbyCharacterPlayer>(TryGetPawnOwner());

	if (OwnerPawn)
	{
		OwningPawn = OwnerPawn;
	}


	bIsAming = false; 
}
