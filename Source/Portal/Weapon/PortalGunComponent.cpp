// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/PortalGunComponent.h"
#include "Character/PortalLobbyCharacterPlayer.h"
#include "Prop/PortalActor.h"
#include "Enum/PortalEnums.h"
#include "Components/BoxComponent.h"

void UPortalGunComponent::LeftClick()
{
}

void UPortalGunComponent::OnEquip()
{
}

// 이부분 로직은 Server에서 실행된다. 
void UPortalGunComponent::HandleHit(const FHitResult& HitResult, const FVector& ShotDirection, EMouseClickType MouseClickType)
{
	Super::HandleHit(HitResult, ShotDirection, MouseClickType);

	EDoorType DoorType = (MouseClickType == EMouseClickType::MOUSE_LEFT) ? EDoorType::BLUE_DOOR : EDoorType::ORANGE_DOOR;


	// 포탈 생성 로직 일단 임시. (나중에는 맞은위치가 아니라 벽이 포탈 생성한 벽인지도 확인해야하고, 위치조절도 해야함)

	APortalLobbyCharacterPlayer* Player = Cast<APortalLobbyCharacterPlayer>(GetOwner());
	if (!Player) return;


	// 아직 생성된 Portal이 없으면 생성하거나, 기존 것을 가져옴
	APortalActor* Portal = Player->GetOrCreatePortalActor();
	if (!Portal) return;


	// 임시: 그냥 충돌 지점에 위치시켜 보기
	FVector ForwardOffset = HitResult.ImpactNormal * 1.0f; // 밀어낼 거리
	FVector SpawnLocation = HitResult.ImpactPoint + ForwardOffset;
	FRotator SpawnRotation = (-HitResult.ImpactNormal).Rotation();

 
	// 설치가능 여부 확인 
	FVector BoxHalfExtent = Portal->BlueDoorCollision->GetScaledBoxExtent();
	FQuat BoxRotation = SpawnRotation.Quaternion();
	FCollisionShape CollisionBox = FCollisionShape::MakeBox(FVector(BoxHalfExtent.Z, BoxHalfExtent.X, BoxHalfExtent.Y));
	 

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Player);

	bool bCanPlacePortal = !GetWorld()->OverlapAnyTestByChannel(
		SpawnLocation,
		BoxRotation,
		COLLISION_PORTAL_TRACE, // 박스가 겹치는지를 확인할 채널
		CollisionBox,
		Params
	);

	// 디버깅 용도
	if (!bCanPlacePortal)
	{
		DrawDebugBox(
			GetWorld(),
			SpawnLocation,
			CollisionBox.GetExtent(),
			BoxRotation,
			FColor::Red,
			false,
			0.3f,
			0,
			2.0f
		);

		UE_LOG(LogTemp, Warning, TEXT("포탈 설치 불가! 충돌 감지됨."));
		return;
	}
	else
	{
		DrawDebugBox(
			GetWorld(),
			SpawnLocation,
			CollisionBox.GetExtent(),
			BoxRotation,
			FColor::Green,
			false,
			0.3f,
			0,
			2.0f
		);
	}


	Portal->HideDoor(DoorType);
	Portal->SetDoorDeferred(DoorType, SpawnLocation, SpawnRotation);

	UE_LOG(LogTemp, Warning, TEXT("Portal placed at: %s"), *SpawnLocation.ToString());
}

 