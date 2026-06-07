// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/PortalThirdPlayerAnimation.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/PortalLobbyCharacterPlayer.h"

void UPortalThirdPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (OwningPawn != nullptr)
    {
        FTransform transform = OwningPawn->GetLeftHandIKTransform();
        if (transform.Equals(FTransform::Identity))
        {
            // 로그 찍거나 디폴트로 설정
            //UE_LOG(LogTemp, Warning, TEXT("Effect Transform Not Applied"));
            EffectTransform = FTransform::Identity;
            return;
        }

        // 문제 없으면 캐싱
        // UE_LOG(LogTemp, Warning, TEXT("Effect Transform Applied"));
        EffectTransform = transform;

        AimPitch = OwningPawn->GetAimPitch();
        //UE_LOG(LogTemp, Warning, TEXT("AimOffset From AnimInstance : %f"), AimPitch);
    }
}

void UPortalThirdPlayerAnimation::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    APortalLobbyCharacterPlayer* OwnerPawn = Cast<APortalLobbyCharacterPlayer>(TryGetPawnOwner());

    if (OwnerPawn)
    {
        OwningPawn = OwnerPawn;
    }
}
