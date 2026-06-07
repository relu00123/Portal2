// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "Enum/PortalEnums.h"
#include "Structs/PortalInfo.h"
#include "PortalActor.generated.h"
 

UCLASS()
class PORTAL_API APortalActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    /** 첫 번째 포탈을 포함하는 SceneComponent */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    USceneComponent* BlueDoorRoot;

    /** 첫 번째 포탈 문 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    UStaticMeshComponent* BlueDoor;

    /** 첫 번째 카메라 루트 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    USceneComponent* BlueCamRoot;

    /** 첫 번째 포탈 뷰 Capture */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    class USceneCaptureComponent2D* BlueCamCapture;

    // 첫 번째 포탈 충돌 감지용 BoxComponent
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    class UBoxComponent* BlueDoorCollision;

    // 첫 번째 포탈 ForwardDirection
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    class UArrowComponent* BlueDoorArrow;

    // 포탈 통과 감지용 Collision (Overlap 전용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalBlue")
    class UBoxComponent* BlueTravellerTrigger;

    /** 두 번째 포탈을 포함하는 SceneComponent */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    USceneComponent* OrangeDoorRoot;

    /** 두 번째 포탈 문 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    UStaticMeshComponent* OrangeDoor;

    /** 두 번째 카메라 루트 */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    USceneComponent* OrangeCamRoot;

    /** 두 번째 포탈 뷰 Capture */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    USceneCaptureComponent2D* OrangeCamCapture;

    // 두번째 포탈 충돌 감지용 BoxComponent
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    class UBoxComponent* OrangeDoorCollision;

    // 두 번째 포탈 ForwardDirection
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    class UArrowComponent* OrangeDoorArrow;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalOrange")
    class UBoxComponent* OrangeTravellerTrigger;

   

    // RenderTarget
    UPROPERTY()
    UTextureRenderTarget2D* BlueCamRenderTarget;

    UPROPERTY()
    UTextureRenderTarget2D* OrangeCamRenderTarget; 


    // Material
    UPROPERTY()
    UMaterialInstanceDynamic* BlueDoorMaterial;

    UPROPERTY()
    UMaterialInstanceDynamic* OrangeDoorMaterial;
 


protected :
    // Portal Scale이 커지는데 필요한 함수
    UFUNCTION()
    void UpdateScale(float Value, UStaticMeshComponent* TargetDoor);


    // VisibilityControl 바꾸는 함수
public :
    void SwitchVisibilityControl();


private:
    FTimeline BlueDoorScaleTimeline;

    FTimeline OrangeDoorScaleTimeline;


    bool      IsVisibilityControlSeted; 


    UFUNCTION()
    void UpdateScaleForBlueDoor(float Value);

    UFUNCTION()
    void UpdateScaleForOrangeDoor(float Value);


    UPROPERTY(EditAnywhere, Category = "Data")
    UCurveFloat* ScaleCurve;


    UFUNCTION()
    void OnRep_BlueDoorHidden();

    UFUNCTION()
    void OnRep_OrangeDoorHidden();

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void PostInitializeComponents() override;

public:
    void HideDoor(EDoorType doorType);
    
    void UnHideDoor(EDoorType doorType);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastSetDoorTransform(EDoorType DoorType, const FVector& Location, const FRotator& Rotation);


    // Testing..
    void SetDoorDeferred(EDoorType DoorType, const FVector& Location, const FRotator& Rotation);


private:
    void ApplyRenderTargetToPortal();


private :
    UFUNCTION()
    void HideDoorCallback(EDoorType doorType);

    UFUNCTION()
    void UnHideDoorCallback(EDoorType doorType);

    UPROPERTY(ReplicatedUsing = OnRep_BlueDoorHidden)
    bool bIsBlueDoorHidden;

    UPROPERTY(ReplicatedUsing = OnRep_OrangeDoorHidden)
    bool bIsOrangeDoorHidden;


    void UpdatePortalCameraRotation();

    // GPT PRO Testing
    FMatrix BuildPerspectiveMatrix(float FOV, float Aspect, float NearPlane, float FarPlane);
    static FMatrix CreateObliqueMatrix(const FMatrix& InProjMatrix, const FPlane& InClipPlane);
    static FPlane TransformPlane(const FPlane& Plane, const FMatrix& M);


    void SetClipPlane();
    void CheckAndResizeRenderTarget();


    // Portal Traveling
    UPROPERTY()
    TSet<class UTravellerComponent*> BlueTrackedTravellers;

    UPROPERTY()
    TSet<class UTravellerComponent*> OrangeTrackedTravellers;


    UFUNCTION()
    void OnBluePortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnBluePortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void OnOrangePortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnOrangePortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    UFUNCTION()
    void ProcessPortalTravel(
        TSet<UTravellerComponent*>& FromSet,
        TSet<UTravellerComponent*>& ToSet,
        USceneComponent* FromArrow,
        USceneComponent* ToArrow);

public :
    // Hiding FirstPersonMesh
    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SetPortalHiddenComponents();

    UFUNCTION()
    void HandleWeaponChanged(UMeshComponent* NewWeaponMesh);
};
