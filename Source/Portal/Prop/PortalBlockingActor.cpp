// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/PortalBlockingActor.h"
#include "Components/BoxComponent.h"
#include "Enum/PortalEnums.h"

// Sets default values
APortalBlockingActor::APortalBlockingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    BlockingBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BlockingBox"));
    BlockingBox->SetupAttachment(RootComponent);

    BlockingBox->SetBoxExtent(FVector(100.f)); // 적절히 조정
    BlockingBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    BlockingBox->SetCollisionObjectType(COLLISION_PORTAL_BLOCKER); // 혹은 적절한 커스텀 채널
    BlockingBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    BlockingBox->SetCollisionResponseToChannel(COLLISION_PORTAL_TRACE, ECR_Overlap); // 포탈 생성 트레이스랑만 충돌

    //DebugMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DebugMesh"));
    //DebugMesh->SetupAttachment(RootComponent);
    //DebugMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 충돌은 비활성화
    //DebugMesh->SetVisibility(true); // 에디터에서 보이게 설정
    //DebugMesh->SetHiddenInGame(true); // 게임 중에는 안 보이도록

}

// Called when the game starts or when spawned
void APortalBlockingActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalBlockingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

