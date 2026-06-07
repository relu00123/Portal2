// Fill out your copyright notice in the Description page of Project Settings.

 
#include "UI/PortalWidgetComponent.h"
#include "UI/PortalUserWidget.h"

void UPortalWidgetComponent::InitWidget()
{
	Super::InitWidget();

	UPortalUserWidget* PortalUserWidget = Cast<UPortalUserWidget>(GetWidget());
	if (PortalUserWidget)
	{
		PortalUserWidget->SetOwningActor(GetOwner());
	}
}
