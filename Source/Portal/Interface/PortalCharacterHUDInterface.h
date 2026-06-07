// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PortalCharacterHUDInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPortalCharacterHUDInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PORTAL_API IPortalCharacterHUDInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetupHUDWidget(class UUserWidget* InHUDWidget) = 0;
};
