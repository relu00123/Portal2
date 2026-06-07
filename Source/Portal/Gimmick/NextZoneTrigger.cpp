// Fill out your copyright notice in the Description page of Project Settings.


#include "Gimmick/NextZoneTrigger.h"
#include "Components/BoxComponent.h"
#include "GameFramework/GameState.h"
#include "GameState/PortalGameState.h"
#include "GameFramework/PlayerState.h"
#include "Character/PortalLobbyCharacterPlayer.h"
#include "Prop/PortalActor.h"

// Sets default values
ANextZoneTrigger::ANextZoneTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;


	// **Collision 설정 추가**
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 충돌 감지 활성화
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);           // 월드 오브젝트로 설정
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);      // 기본적으로 모든 채널 무시
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // 플레이어(Pawn)와 충돌 감지


	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ANextZoneTrigger::OnTriggerEnter);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ANextZoneTrigger::OnTriggerExit);


}

void ANextZoneTrigger::OnTriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("Trigger Entered!"));

		APlayerController* PlayerController = Cast<APlayerController>(OtherActor->GetInstigatorController());
		if (PlayerController && !PlayersInside.Contains(PlayerController))
		{
			PlayersInside.Add(PlayerController);
			CheckAllPlayersInside();
		}
	}
	
}

void ANextZoneTrigger::OnTriggerExit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (HasAuthority())
	{

		UE_LOG(LogTemp, Warning, TEXT("Trigger Exited!"));

		APlayerController* PlayerController = Cast<APlayerController>(OtherActor->GetInstigatorController());
		if (PlayerController)
		{
			PlayersInside.Remove(PlayerController);
		}
	}
}

void ANextZoneTrigger::CheckAllPlayersInside()
{
	APortalGameState* GameState =  GetWorld()->GetGameState<APortalGameState>();
	//AGameStateBase* GameState = GetWorld()->GetGameState<AGameStateBase>();
	if (!GameState) return; // GameState가 없으면 함수 종료

	int32 TotalPlayerCount = GameState->PlayerArray.Num(); // 현재 접속 중인 모든 플레이어 수

	if (PlayersInside.Num() == TotalPlayerCount) // 모든 플레이어가 트리거 안에 있는지 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("All Players Inside"));
		  
		DeleteExistingPortals();
		GameState->ServerMapTravel(MapName);
		PlayersInside.Empty();
	}
}

void ANextZoneTrigger::DeleteExistingPortals()
{
	APortalGameState* GameState = GetWorld()->GetGameState<APortalGameState>();
	if (!GameState) return;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		AController* Controller = PlayerState->GetOwner<AController>();
		if (!Controller) continue;

		APawn* Pawn = Controller->GetPawn();
		if (!Pawn) continue;

		// 너의 캐릭터 클래스인 APortalLobbyCharacterPlayer* 로 캐스팅
		APortalLobbyCharacterPlayer* Character = Cast<APortalLobbyCharacterPlayer>(Pawn);
		if (!Character) continue;

		// 포탈 삭제
		if (Character->OwningPortal)
		{
			Character->OwningPortal->Destroy();
			Character->OwningPortal = nullptr;

			UE_LOG(LogTemp, Warning, TEXT("Destroyed portal for player: %s"), *Character->GetName());
		}
	}
}

// Called when the game starts or when spawned
void ANextZoneTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerBox)
	{
		FVector BoxExtent = TriggerBox->GetScaledBoxExtent();
		FVector BoxLocation = TriggerBox->GetComponentLocation();
		FQuat BoxRotation = TriggerBox->GetComponentQuat(); // **회전 정보 가져오기**

		// **회전을 적용한 상태로 디버그 박스 그리기**
		DrawDebugBox(GetWorld(), BoxLocation, BoxExtent, BoxRotation, FColor::Green, true, -1.0f, 0, 5.0f);
	}
	
}
 

