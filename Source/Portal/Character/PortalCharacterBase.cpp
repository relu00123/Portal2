// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PortalCharacterBase.h"

// Sets default values
APortalCharacterBase::APortalCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	

}

// Called when the game starts or when spawned
void APortalCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//// 1. 현재 속도 벡터 가져오기
	//FVector Velocity = GetVelocity();

	//// 2. 속도의 크기(길이)를 구함
	//float Speed = Velocity.Size();

	//// 3. 디버그 출력
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Green, FString::Printf(TEXT("Speed: %.2f"), Speed));
	//UE_LOG(LogTemp, Log, TEXT("Speed: %.2f"), Speed);

}

// Called to bind functionality to input
void APortalCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

