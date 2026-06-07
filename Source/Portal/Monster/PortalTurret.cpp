// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/PortalTurret.h"
#include "Components/HitReactionComponent.h"
#include "Player/PortalLobbyPlayerController.h"
#include "UI/PortalLobbyHUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Structs/DamageTypes.h"
 

APortalTurret::APortalTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComp;

	TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	TurretMesh->SetupAttachment(RootSceneComp); // 이제 Transform 전부 조절 가능


	// 루트 컴포넌트 생성
	//TurretMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TurretMesh"));
	//RootComponent = TurretMesh;

	// HitReactionComponent 생성 및 연결
	HitReactionComponent = CreateDefaultSubobject<UHitReactionComponent>(TEXT("HitReactionComponent"));

	HitReactionComponent->TargetMeshComponent = TurretMesh;
}

void APortalTurret::BeginPlay()
{
	Super::BeginPlay();

	// 테스트용 
	//if (HitReactionComponent)
	//{
	//	// 테스트용: 5초 후 Flash 트리거
	//	FTimerHandle TempTimerHandle;
	//	GetWorld()->GetTimerManager().SetTimer(
	//		TempTimerHandle,
	//		[this]()
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("TriggerFlash() called!"));
	//			HitReactionComponent->TriggerFlash();
	//		},
	//		5.0f, // 시간 (초)
	//		false // 반복 여부 (false: 1회)
	//	);
	//}
	
}

void APortalTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalTurret::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float APortalTurret::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Portal Turret TakeDamage Function Called!"));

	// 리펙토링 전 
	//// Took Damage 나오는 것 확인 
	//UE_LOG(LogTemp, Warning, TEXT("Took Damage : %f !"), DamageAmount);

	//// 반짝이는 효과 테스트 (Material 하얀색으로 잠깐 변화) 
	//if (HitReactionComponent)
	//{
	//	HitReactionComponent->TriggerFlash();
	//}

	//// 서버 공통 이펙트: Multicast 호출
	//APlayerState* InstigatorPlayerState = EventInstigator ? EventInstigator->PlayerState : nullptr;
	//
	//MulticastTakeDamageEffect(DamageAmount, InstigatorPlayerState, true );
	//

	//// 체력 감소나 죽음 처리 로직 여기에 추가 가능
	//return DamageAmount;



	// Refactoring 중.. 
	bool bWasCritical = false; 

	if (DamageEvent.IsOfType(FExtendPointDamageEvent::ClassID))
	{
		const FExtendPointDamageEvent& ExtendedEvent = static_cast<const FExtendPointDamageEvent&>(DamageEvent);
		bWasCritical = ExtendedEvent.bWasCritical;

		// 반짝이는 효과 테스트 (Material 하얀색으로 잠깐 변화) 
		if (HitReactionComponent)
		{
			HitReactionComponent->TriggerFlash();
		}

		// 서버 공통 이펙트: Multicast 호출
		APlayerState* InstigatorPlayerState = EventInstigator ? EventInstigator->PlayerState : nullptr;

		MulticastTakeDamageEffect(DamageAmount, InstigatorPlayerState, bWasCritical);
	}

	return DamageAmount;

}

void APortalTurret::MulticastTakeDamageEffect_Implementation(float DamageAmount, APlayerState* InstigatorPlayerState, bool IsCritical)
{
	// 반짝이는 효과 테스트 (공통 이팩트)
	if (HitReactionComponent)
	{
		HitReactionComponent->TriggerFlash();
	}

	if (APlayerController* LocalPC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (LocalPC->PlayerState == InstigatorPlayerState)
		{
			if (APortalLobbyPlayerController* PC = Cast<APortalLobbyPlayerController>(LocalPC))
			{
				UPortalLobbyHUDWidget* HUD =  PC->GetLobbyHUDWidget();

				if (HUD)
				{
					HUD->ShowDamageTextAtLocation(GetActorLocation(), DamageAmount, IsCritical);
				}
			}
		}
	}
}

