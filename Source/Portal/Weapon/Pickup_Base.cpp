// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Pickup_Base.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Character/PortalLobbyCharacterPlayer.h"
#include "Enum/PortalEnums.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/WeaponComponent.h"

// Sets default values
APickup_Base::APickup_Base()
{
	bReplicates = true; // 네트워크 복제 활성화

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// 무기 모델 
	PickupMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;

	// 충돌 컴포넌트
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetupAttachment(RootComponent);

	// 충돌 이벤트 바인딩
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &APickup_Base::OnOverlap);

	bIsDestroyed = false; 
}

void APickup_Base::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APickup_Base, bIsDestroyed);
}


// Called when the game starts or when spawned
void APickup_Base::BeginPlay()
{
	Super::BeginPlay();
}

 
void APickup_Base::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APortalLobbyCharacterPlayer* PortalCharacter = Cast<APortalLobbyCharacterPlayer>(OtherActor);

	if (PortalCharacter)
	{
		if (GiveWeaponToPlayer(PortalCharacter))
		{
			// Destroy();
			//bIsDestroyed = true;
			//Destroy(); // 서버에서는 자동호출 
			//OnRep_IsDestroyed();
			Multicast_DestroyPickup();
		}
	}
}

bool APickup_Base::GiveWeaponToPlayer(APortalLobbyCharacterPlayer* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("GiveWeaponToPlayer Called!"));

	if (HasAuthority() && PlayerCharacter && WeaponComponentClass)
	{
		UWeaponComponent* NewWeapon = NewObject<UWeaponComponent>(PlayerCharacter, WeaponComponentClass);
		if (NewWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Weapon Component found!"));
			if(PlayerCharacter->EquipWeapon(NewWeapon))
			{
				return true;
			}

			else
			{
				NewWeapon->DestroyComponent();
			}
		}
	}

	return false;
}

void APickup_Base::OnRep_IsDestroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_IsDestroyed Called"));
	if (bIsDestroyed)
		Destroy();
}

void APickup_Base::Multicast_DestroyPickup_Implementation()
{
	Destroy();
}

 
 

