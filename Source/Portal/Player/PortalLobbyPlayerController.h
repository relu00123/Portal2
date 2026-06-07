// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PORTAL_API APortalLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APortalLobbyPlayerController();

protected:
	virtual void BeginPlay() override;

	
// HUD Section
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UPortalLobbyHUDWidget> PortalLobbyHUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UPortalLobbyHUDWidget> PortalLobbyHUDWidget;

	UFUNCTION(Server, Reliable)
	void ServerSetReady();

	UFUNCTION()
	void HandleStartOrReadyButton();

public :
	void ClickOnPlayerControllerTest();

	FORCEINLINE UPortalLobbyHUDWidget* GetLobbyHUDWidget() { return PortalLobbyHUDWidget;  }

	virtual void OnRep_PlayerState() override; 

	UFUNCTION(Client, Reliable)
	void ClientAdjustCharacterPos(FTransform transform);

	
	UFUNCTION(Client, Reliable)
	void ClientChangeToGameModeHUD();
};
