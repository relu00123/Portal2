// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Interface/PortalLobbyHUDWidget_Interface.h"
#include "Enum/EReadyState.h"
#include "PortalPlayerState.generated.h"

/**
 * 
 */

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerNameChanged, const FString&);

UCLASS()
class PORTAL_API APortalPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	APortalPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_ReadyState, BlueprintReadOnly, Category = "State")
	EReadyState ReadyState;
	

//protected:
//	UFUNCTION()
//	void OnRep_MyPlayerName();
//
//private:
//	UPROPERTY(ReplicatedUsing = OnRep_MyPlayerName)
//	FString PlayerName; 

public:
	void SetName(const FString& NewName);

	virtual void OnRep_PlayerName() override;

	UFUNCTION()
	void OnRep_ReadyState();

 
	virtual void CopyProperties(APlayerState* NewPlayerState) override;

	//virtual void SetupLobbyHUDWidget(class UPortalLobbyHUDWidget* InHUDWidget) override; 

	virtual void PostInitializeComponents() override;

	UFUNCTION()
	void ClientNameChanged();

};
