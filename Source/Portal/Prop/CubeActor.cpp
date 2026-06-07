// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/CubeActor.h"
#include "Components/GrabInteractableComponent.h"
#include "Components/TravellerComponent.h"

// Sets default values
ACubeActor::ACubeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);
	

	// Static Mesh 생성 및 설정
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetIsReplicated(true);
	RootComponent = MeshComponent;

	if (HasAuthority())
	{
		MeshComponent->SetSimulatePhysics(true);
	}
	else
	{
		MeshComponent->SetSimulatePhysics(false);
	}

	// 컴포넌트 붙이기
	GrabComponent = CreateDefaultSubobject<UGrabInteractableComponent>(TEXT("GrabComponent"));

	// TravellerComponent Test. 
	TravellerComponent = CreateDefaultSubobject<UTravellerComponent>(TEXT("TravellerComponent"));
}

// Called when the game starts or when spawned
void ACubeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

 
