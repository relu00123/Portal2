// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GrabInteractableComponent.h"
#include "Components/ActorComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGrabInteractableComponent::UGrabInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true); // 컴포넌트 복제 활성화
}


// Called when the game starts
void UGrabInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UGrabInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UGrabInteractableComponent, GrabbedBy);
}


// Called every frame
void UGrabInteractableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UGrabInteractableComponent::CanBeGrabbedBy(AActor* Grabber) const
{
	if (GrabbedBy == nullptr || GrabbedBy == Grabber)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can Grab Object!"));
		return true;
	}

	return false;
}

void UGrabInteractableComponent::SetGrabbedBy(AActor* Grabber)
{
	GrabbedBy = Grabber;
}

void UGrabInteractableComponent::ClearGrabbedBy()
{
	GrabbedBy = nullptr;
}

