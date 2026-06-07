// Fill out your copyright notice in the Description page of Project Settings.


#include "UIActor/DamageNumber.h"
#include "Components/WidgetComponent.h"
#include "UI/DamageNumberWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
ADamageNumber::ADamageNumber()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::World);
	WidgetComponent->SetDrawSize(FVector2D(200.f, 100.f));
	WidgetComponent->SetTwoSided(true);
	WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	SetRootComponent(WidgetComponent);
}

// Called when the game starts or when spawned
void ADamageNumber::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetComponent && WidgetComponent->GetWidget())
	{
		WidgetInstance = Cast<UDamageNumberWidget>(WidgetComponent->GetUserWidgetObject());
	}
	
}

// Called every frame
void ADamageNumber::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ElapsedTime += DeltaTime;

	// 위로 떠오르기
	FVector NewLocation = GetActorLocation();
	NewLocation.Z += FloatSpeed * DeltaTime;
	SetActorLocation(NewLocation);

	// 카메라 바라보기
	UpdateFaceToCamera();

	// 수명 끝나면 파괴
	if (ElapsedTime >= LifeTime)
	{
		Destroy();
	}
}

void ADamageNumber::InitDamage(float DamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("Init Damage Called! (DamageNumber.cpp"));

	if (!WidgetComponent || !DamageNumberWidgetClass) return;

	UE_LOG(LogTemp, Warning, TEXT("Widget Component & DamageNumberWidgetClass Exists"));

	WidgetComponent->SetWidgetClass(DamageNumberWidgetClass);
	WidgetComponent->InitWidget();

	if (UDamageNumberWidget* DamageWidget = Cast<UDamageNumberWidget>(WidgetComponent->GetUserWidgetObject()))
	{
		UE_LOG(LogTemp, Warning, TEXT("SetDamage Called! (DamageNumber.cpp"));
		//DamageWidget->SetDamageText(DamageAmount);
	}
}



void ADamageNumber::UpdateFaceToCamera()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (PC->PlayerCameraManager)
		{
			FVector CameraLocation = PC->PlayerCameraManager->GetCameraLocation();
			FVector Direction = CameraLocation - GetActorLocation();
			FRotator LookAtRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
			SetActorRotation(LookAtRotation);
		}
	}
}

