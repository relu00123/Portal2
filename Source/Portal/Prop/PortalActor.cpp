// Fill out your copyright notice in the Description page of Project Settings.


#include "Prop/PortalActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Math/Quat.h"
#include "Kismet/KismetMathLibrary.h"  // Kismet 수학 기능 포함
#include "Math/UnrealMathUtility.h"    // 유틸리티 기능 포함
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Character/PortalLobbyCharacterPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/TravellerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "Character/PortalLobbyCharacterPlayer.h"

// Sets default values
APortalActor::APortalActor()
{
	PrimaryActorTick.bCanEverTick = true;

    // Network Replication
    bReplicates = true;
    bAlwaysRelevant = true;
    SetReplicateMovement(true);

    bIsOrangeDoorHidden = true;
    bIsBlueDoorHidden = true; 

    // 루트 컴포넌트 생성
    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // BlueDoor
    BlueDoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("D1"));
    BlueDoorRoot->SetupAttachment(RootComponent);

    BlueDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlueDoor"));
    BlueDoor->SetupAttachment(BlueDoorRoot);

    BlueCamRoot = CreateDefaultSubobject<USceneComponent>(TEXT("BlueCamRoot"));
    BlueCamRoot->SetupAttachment(BlueDoorRoot);

    BlueCamCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("BlueCamCapture"));
    BlueCamCapture->SetupAttachment(BlueCamRoot);

    BlueDoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BlueDoorCollision"));
    BlueDoorCollision->SetupAttachment(BlueDoor);

    BlueDoorArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("BlueForwardDirection"));
    BlueDoorArrow->SetupAttachment(BlueDoor);

    BlueTravellerTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("BlueTravellerTrigger"));
    BlueTravellerTrigger->SetupAttachment(BlueDoor); // 또는 BlueDoor

    // OrangeDoor
    OrangeDoorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("D2"));
    OrangeDoorRoot->SetupAttachment(RootComponent);

    OrangeDoor = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrangeDoor"));
    OrangeDoor->SetupAttachment(OrangeDoorRoot);

    OrangeCamRoot = CreateDefaultSubobject<USceneComponent>(TEXT("OrangeCamRoot"));
    OrangeCamRoot->SetupAttachment(OrangeDoorRoot);

    OrangeCamCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("OrangeCamCapture"));
    OrangeCamCapture->SetupAttachment(OrangeCamRoot);


    OrangeDoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OrangeDoorCollision"));
    OrangeDoorCollision->SetupAttachment(OrangeDoor);

    OrangeDoorArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("OrangeForwardDirection"));
    OrangeDoorArrow->SetupAttachment(OrangeDoor);

    OrangeTravellerTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("OrangeTravellerTrigger"));
    OrangeTravellerTrigger->SetupAttachment(OrangeDoor);

    // OrangeCamCapture->HiddenComponents.Add(PlayerCharacter->Mesh1P);
    
}

void APortalActor::BeginPlay()
{
    Super::BeginPlay();

    // Orange
    if (OrangeCamCapture)
    {
        OrangeCamCapture->ShowFlags.SetSkyLighting(true);   // 하늘 조명
        OrangeCamCapture->ShowFlags.SetAtmosphere(true);    // 대기 효과
        OrangeCamCapture->ShowFlags.SetFog(true);           // 안개
        OrangeCamCapture->ShowFlags.SetStaticMeshes(true);  // 스카이돔 포함
        OrangeCamCapture->ShowFlags.SetDirectionalLights(true); // 태양광
        OrangeCamCapture->ShowFlags.SetPostProcessing(true); // 후처리 (톤매핑 등)
    }

    // Blue
    if (BlueCamCapture)
    {
        BlueCamCapture->ShowFlags.SetSkyLighting(true);   // 하늘 조명
        BlueCamCapture->ShowFlags.SetAtmosphere(true);    // 대기 효과
        BlueCamCapture->ShowFlags.SetFog(true);           // 안개
        BlueCamCapture->ShowFlags.SetStaticMeshes(true);  // 스카이돔 포함
        BlueCamCapture->ShowFlags.SetDirectionalLights(true); // 태양광
        BlueCamCapture->ShowFlags.SetPostProcessing(true); // 후처리 (톤매핑 등)
    }

    // 블루 포탈이 오렌지 포탈을 바라봐야한다.
    if (BlueCamCapture)
    {
        BlueCamRenderTarget = NewObject<UTextureRenderTarget2D>(this);
        if (BlueCamRenderTarget)
        {
            FVector2D ViewportSize;
            GEngine->GameViewport->GetViewportSize(ViewportSize);
            BlueCamRenderTarget->InitAutoFormat(ViewportSize.X, ViewportSize.Y);
            BlueCamRenderTarget->ClearColor = FLinearColor::Black;
            BlueCamCapture->TextureTarget = BlueCamRenderTarget;  // RenderTarget에 저장
        }
    }

    if (OrangeCamCapture)
    {
        OrangeCamRenderTarget = NewObject<UTextureRenderTarget2D>(this);
        if (BlueCamRenderTarget)
        {
            FVector2D ViewportSize;
            GEngine->GameViewport->GetViewportSize(ViewportSize);
            OrangeCamRenderTarget->InitAutoFormat(ViewportSize.X, ViewportSize.Y);
            OrangeCamRenderTarget->ClearColor = FLinearColor::Black;
            OrangeCamCapture->TextureTarget = OrangeCamRenderTarget;  // RenderTarget에 저장
        }
    }

    ApplyRenderTargetToPortal();

    // Teleport Related 
    BlueTravellerTrigger->OnComponentBeginOverlap.AddDynamic(this, &APortalActor::OnBluePortalOverlapBegin);
    BlueTravellerTrigger->OnComponentEndOverlap.AddDynamic(this, &APortalActor::OnBluePortalOverlapEnd);
    OrangeTravellerTrigger->OnComponentBeginOverlap.AddDynamic(this, &APortalActor::OnOrangePortalOverlapBegin);
    OrangeTravellerTrigger->OnComponentEndOverlap.AddDynamic(this, &APortalActor::OnOrangePortalOverlapEnd);


    // ScreenCapture Hide
    if (HasAuthority())
    {
        Multicast_SetPortalHiddenComponents();
    }


    AGameStateBase* GameState = GetWorld()->GetGameState();
    
    if (GameState)
    {
        for (APlayerState* PlayerState : GameState->PlayerArray)
        {
            if (APawn* Pawn = PlayerState->GetPawn())
            {
                APortalLobbyCharacterPlayer* Char = Cast<APortalLobbyCharacterPlayer>(Pawn);
                if (Char && Char->FirstPersonMesh)
                {
                    Char->OnWeaponMeshChanged.AddDynamic(this, &APortalActor::HandleWeaponChanged);
                }
            }
        }
    }
}

// Called every frame
void APortalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


    // TimeLine이 실행중일대만 실행
    if (BlueDoorScaleTimeline.IsPlaying())
    {
        BlueDoorScaleTimeline.TickTimeline(DeltaTime);
    }

    if (OrangeDoorScaleTimeline.IsPlaying())
    {
        OrangeDoorScaleTimeline.TickTimeline(DeltaTime);
    }

    UpdatePortalCameraRotation();


    // Teleport Related 
    //ProcessPortalTravel(BlueTrackedTravellers, OrangeTrackedTravellers, BlueDoor, OrangeDoor);
    //ProcessPortalTravel(OrangeTrackedTravellers, BlueTrackedTravellers, OrangeDoor, BlueDoor);

    if (HasAuthority())
    {
        //기존에 사용하던 방식
        ProcessPortalTravel(BlueTrackedTravellers, OrangeTrackedTravellers, BlueDoorArrow, OrangeDoorArrow);
        ProcessPortalTravel(OrangeTrackedTravellers, BlueTrackedTravellers, OrangeDoorArrow, BlueDoorArrow);

    }
}

 

void APortalActor::UpdateScale(float Value, UStaticMeshComponent* TargetDoor)
{
    if (TargetDoor)
    {
        FVector NewScale = FVector(Value);
        NewScale.Z = 1.0f;
        TargetDoor->SetRelativeScale3D(NewScale);

        // UE_LOG(LogTemp, Warning, TEXT("Updating Scale: %f for %s"), Value, *TargetDoor->GetName());
    }


    if (!bIsOrangeDoorHidden && !bIsBlueDoorHidden)
    {
        if (BlueDoorMaterial)
            BlueDoorMaterial->SetScalarParameterValue(FName("isPortalLinked"), Value / 3.f);
        

        if (OrangeDoorMaterial)
            OrangeDoorMaterial->SetScalarParameterValue(FName("isPortalLinked"), Value / 3.f);
    }

    
}

void APortalActor::SwitchVisibilityControl()
{
    IsVisibilityControlSeted = !IsVisibilityControlSeted;
    float ChangeValue = (float)IsVisibilityControlSeted;

    if (BlueDoorMaterial)
    {
        BlueDoorMaterial->SetScalarParameterValue(FName("VisibilityControl"), ChangeValue);
    }

    if (OrangeDoorMaterial)
    {
        OrangeDoorMaterial->SetScalarParameterValue(FName("VisibilityControl"), ChangeValue);
    }
}

void APortalActor::UpdateScaleForBlueDoor(float Value)
{
    UpdateScale(Value, BlueDoor);

    

}

void APortalActor::UpdateScaleForOrangeDoor(float Value)
{
    UpdateScale(Value, OrangeDoor);
}

void APortalActor::OnRep_BlueDoorHidden()
{
    if (bIsBlueDoorHidden) // 문을 숨겨야 하는 경우
    {
        HideDoorCallback(EDoorType::BLUE_DOOR);
    }

    else
    {
        UnHideDoorCallback(EDoorType::BLUE_DOOR);
    }
}

void APortalActor::OnRep_OrangeDoorHidden()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRep_OrangeDoor Hidden Called!"));

    if (bIsOrangeDoorHidden)
    {
        HideDoorCallback(EDoorType::ORANGE_DOOR);
    }

    else
    {
        UnHideDoorCallback(EDoorType::ORANGE_DOOR);
    }
}

void APortalActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(APortalActor, bIsBlueDoorHidden);  
    DOREPLIFETIME(APortalActor, bIsOrangeDoorHidden);  
}

void APortalActor::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    HideDoorCallback(EDoorType::ORANGE_DOOR);
    HideDoorCallback(EDoorType::BLUE_DOOR);

}

void APortalActor::HideDoor(EDoorType doorType)
{
    if (doorType == EDoorType::BLUE_DOOR)
    {
        bIsBlueDoorHidden = true;

        if (HasAuthority())
        {
            OnRep_BlueDoorHidden();
        }
    }

    else if (doorType == EDoorType::ORANGE_DOOR)
    {
        bIsOrangeDoorHidden = true;

        if (HasAuthority())
        {
            OnRep_OrangeDoorHidden();
        }
    }
}

void APortalActor::UnHideDoor(EDoorType doorType)
{
    if (doorType == EDoorType::BLUE_DOOR)
    {
        bIsBlueDoorHidden = false;

        if (HasAuthority())
        {
            OnRep_BlueDoorHidden();
        }
    }

    else if (doorType == EDoorType::ORANGE_DOOR)
    {
        bIsOrangeDoorHidden = false; 

        if (HasAuthority())
        {
            OnRep_OrangeDoorHidden();
        }
    }
    
}

void APortalActor::MulticastSetDoorTransform_Implementation(EDoorType DoorType, const FVector& Location, const FRotator& Rotation)
{
    if (DoorType == EDoorType::BLUE_DOOR && BlueDoorRoot)
    {
        BlueDoorRoot->SetWorldLocation(Location);
        BlueDoorRoot->SetWorldRotation(Rotation);
    }
    else if (DoorType == EDoorType::ORANGE_DOOR && OrangeDoorRoot)
    {
        OrangeDoorRoot->SetWorldLocation(Location);
        OrangeDoorRoot->SetWorldRotation(Rotation);
    }
}

void APortalActor::SetDoorDeferred(EDoorType DoorType, const FVector& Location, const FRotator& Rotation)
{
    MulticastSetDoorTransform(DoorType, Location, Rotation);

    // 다음 프레임에 문 보이게 하기
    FTimerHandle TempHandle;
    GetWorld()->GetTimerManager().SetTimer(TempHandle, [this, DoorType]()
        {
            UnHideDoor(DoorType); // Timeline 시작

        }, 0.01f, false); // 0.01초 후 실행
}

void APortalActor::ApplyRenderTargetToPortal()
{
    if (BlueDoor && OrangeCamRenderTarget)
    {
        BlueDoorMaterial = BlueDoor->CreateAndSetMaterialInstanceDynamic(0);
        if (BlueDoorMaterial)
        {
            BlueDoorMaterial->SetTextureParameterValue(FName("RT"), OrangeCamRenderTarget);
        }
    }

    if (OrangeDoor && BlueCamRenderTarget)
    {
        OrangeDoorMaterial = OrangeDoor->CreateAndSetMaterialInstanceDynamic(0);
        if (OrangeDoorMaterial)
        {
            OrangeDoorMaterial->SetTextureParameterValue(FName("RT"), BlueCamRenderTarget);
        }
    }
}

void APortalActor::HideDoorCallback(EDoorType doorType)
{
    if (doorType == EDoorType::BLUE_DOOR)
    {
        if (BlueDoor)
        {
            // BlueDoor->SetVisibility(false, true);  // Editor + InGame?
            BlueDoor->SetHiddenInGame(true); // InGame? 
            BlueDoor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }

    }

    else if (doorType == EDoorType::ORANGE_DOOR)
    {
        if (OrangeDoor)
        {
            // OrangeDoor->SetVisibility(false, true);  // Editor + InGame?
            OrangeDoor->SetHiddenInGame(true); // InGame? 
            OrangeDoor->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
    }

    if (bIsOrangeDoorHidden || bIsBlueDoorHidden)
    {
        if (BlueDoorMaterial)
            BlueDoorMaterial->SetScalarParameterValue(FName("isPortalLinked"), 0.f);

        if (OrangeDoorMaterial)
            OrangeDoorMaterial->SetScalarParameterValue(FName("isPortalLinked"), 0.f);
    }
}

void APortalActor::UnHideDoorCallback(EDoorType doorType)
{

    if (doorType == EDoorType::ORANGE_DOOR && OrangeDoor && ScaleCurve)
    {
        OrangeDoor->SetHiddenInGame(false);  // 다시 보이게 설정
        //OrangeDoor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  // 충돌 다시 활성화

        FOnTimelineFloat TimelineProgress2;
        TimelineProgress2.BindUFunction(this, FName("UpdateScaleForOrangeDoor"));
        OrangeDoorScaleTimeline.AddInterpFloat(ScaleCurve, TimelineProgress2);
        OrangeDoorScaleTimeline.SetLooping(false);
        OrangeDoorScaleTimeline.PlayFromStart();

    }

    else if (doorType == EDoorType::BLUE_DOOR && BlueDoor && ScaleCurve)
    {
        BlueDoor->SetHiddenInGame(false);  // 다시 보이게 설정
        //BlueDoor->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);  // 충돌 다시 활성화

        FOnTimelineFloat TimelineProgress1;
        TimelineProgress1.BindUFunction(this, FName("UpdateScaleForBlueDoor"));
        BlueDoorScaleTimeline.AddInterpFloat(ScaleCurve, TimelineProgress1);
        BlueDoorScaleTimeline.SetLooping(false);
        BlueDoorScaleTimeline.PlayFromStart();

    }

    // 2개의 Door화면을 연결 시켜준다. 
    if (!bIsOrangeDoorHidden && !bIsBlueDoorHidden)
    {
        if (BlueDoorMaterial)
            BlueDoorMaterial->SetScalarParameterValue(FName("isPortalLinked"), 1);

        if (OrangeDoorMaterial)
            OrangeDoorMaterial->SetScalarParameterValue(FName("isPortalLinked"), 1);
    }
}

void APortalActor::UpdatePortalCameraRotation()
{
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn || !BlueDoorRoot || !OrangeDoorRoot || !BlueCamCapture || !OrangeCamCapture) return;

    APortalLobbyCharacterPlayer* Player = Cast<APortalLobbyCharacterPlayer>(PlayerPawn);
    if (!Player || !Player->FirstPersonCamera) return;

    UCameraComponent* PlayerCamera = Player->FirstPersonCamera;
    const FTransform PlayerCamTransform = PlayerCamera->GetComponentTransform();
    

    auto CalculatePortalView = [](const FTransform& From, const FTransform& To, const FTransform& CamTransform) -> FTransform
        {
            // 1. 입구 기준으로 카메라 위치/회전을 로컬로 변환
            const FTransform LocalCamTransform = CamTransform.GetRelativeTransform(From);

            // 2. 회전 보정 (Yaw 반전)
            const FQuat FlipYaw180 = FQuat(FVector::UpVector, PI);
            const FQuat FlippedRot = FlipYaw180 * LocalCamTransform.GetRotation();

            // 3. 회전 → 위치 순으로 적용
            FTransform FinalTransform;
            FinalTransform.SetRotation(FlippedRot);
            FinalTransform.SetLocation(FlipYaw180.RotateVector(LocalCamTransform.GetLocation()));
            FinalTransform.SetScale3D(LocalCamTransform.GetScale3D());

            // 4. 출구 포탈 기준으로 다시 월드로 변환
            return FinalTransform * To;
        };
 
    // 1) Blue → Orange 포털
    {
        // 기존 계산 (카메라 위치/회전)
        FTransform FinalTransform = CalculatePortalView(
            BlueDoorRoot->GetComponentTransform(),
            OrangeDoorRoot->GetComponentTransform(),
            PlayerCamTransform
        );
        OrangeCamCapture->SetWorldTransform(FinalTransform);
        OrangeCamCapture->FOVAngle = PlayerCamera->FieldOfView;
    }

    // 2) Orange → Blue 포털
    {
        FTransform FinalTransform = CalculatePortalView(
            OrangeDoorRoot->GetComponentTransform(),
            BlueDoorRoot->GetComponentTransform(),
            PlayerCamTransform
        );
        BlueCamCapture->SetWorldTransform(FinalTransform);
        BlueCamCapture->FOVAngle = PlayerCamera->FieldOfView;

        // **Oblique** 설정
        //SetupObliqueClipping(BlueCamCapture, BlueDoorRoot->GetComponentTransform(), FinalTransform);
    }

    SetClipPlane();
    CheckAndResizeRenderTarget();
}


FMatrix APortalActor::BuildPerspectiveMatrix(float FOV, float Aspect, float Near, float Far)
{
    // Unreal은 종종 (NearPlane=GNearClippingPlane, FarPlane=플레이스 홀더) 식으로만 써놓고,
   // 사실상 "씬에 따라" Far를 무한대처럼 처리하기도 합니다. 여기서는 단순 예시.
   // Reversed Z에서 Z=Near가 가장 큰 깊이, Z=Far가 가장 작은 깊이가 됩니다.

    const float S = 1.f / FMath::Tan(FOV * 0.5f);

    // Row-major 순서
    FMatrix M = FMatrix::Identity;
    M.M[0][0] = S / Aspect;  // X축 스케일
    M.M[1][1] = S;           // Y축 스케일

    // Reversed Z 핵심 (UE 소스참조)
    // M[2][2] =   F / (F - N)
    // M[2][3] =  +1
    // M[3][2] = -(F*N) / (F - N)
    // M[3][3] =   0
    // 여기서 F = FarPlane, N = NearPlane

    M.M[2][2] = (Far) / (Far - Near);
    M.M[2][3] = 1.f;
    M.M[3][2] = -(Far * Near) / (Far - Near);
    M.M[3][3] = 0.f;

    return M;
}

FMatrix APortalActor::CreateObliqueMatrix(const FMatrix& InProjMatrix, const FPlane& InClipPlane)
{
    // 1) ClipPlane 부호 맞추기 (카메라를 향하게)
    //    - 보통 W>0이면 뒤집어주는데, ReversedZ여도 큰 차이는 없습니다.
    FPlane ClipPlane = InClipPlane;
    if (ClipPlane.W > 0.f)
    {
        ClipPlane = FPlane(-ClipPlane.X, -ClipPlane.Y, -ClipPlane.Z, -ClipPlane.W);
    }

     

    // 2) 투영행렬에서 '3번째 컬럼'(= (0,2), (1,2), (2,2), (3,2)) 과
    //    '4번째 컬럼'(= (0,3), (1,3), (2,3), (3,3))을 뽑아온다.
    //    Unreal은 row-major 이므로 M[row][col] 순서.
    FVector4 PRow2(InProjMatrix.M[0][2], InProjMatrix.M[1][2], InProjMatrix.M[2][2], InProjMatrix.M[3][2]);
    FVector4 PRow3(InProjMatrix.M[0][3], InProjMatrix.M[1][3], InProjMatrix.M[2][3], InProjMatrix.M[3][3]);

    // 이제 ReversedZ 투영행렬이라서 p(2,3) = +1 이라는 점이 중요!
    // => 일반 Oblique 예제는 p(2,3)=-1 가정 -> sign 계산할 때 약간 달라짐.

    // 3) Q벡터 계산
    //    Unity/DirectX 예제는  Q.z = -1 로 두는데,
    //    여기서는 p(2,3)=+1 이므로 Q.z = +1 로 잡는 식이 필요합니다.
    //    (아래는 한 가지 예시이며, 상황 따라 부호 바뀔 수 있음)

    FVector4 q;
    q.X = (FMath::Sign(ClipPlane.X) + PRow2.X) / InProjMatrix.M[0][0];
    q.Y = (FMath::Sign(ClipPlane.Y) + PRow2.Y) / InProjMatrix.M[1][1];
    // *** 핵심: ReversedZ이므로 p(2,3)=+1 => Q.z = +1
    q.Z = 1.f;
    q.W = (FMath::Sign(ClipPlane.Z) + PRow2.W) / InProjMatrix.M[2][3];
    // InProjMatrix.M[2][3] = +1 (위에서 만든 행렬을 보면)

    // 4) c = 2 * plane / dot(plane, q)
    //    (또는 일부 예제에서는 +2, -2가 다를 수 있음)
    FVector4 planeVec(ClipPlane.X, ClipPlane.Y, ClipPlane.Z, ClipPlane.W);
    float denom =
        planeVec.X * q.X +
        planeVec.Y * q.Y +
        planeVec.Z * q.Z +
        planeVec.W * q.W;
    FVector4 c = (2.f / denom) * planeVec;

    // 5) 프로젝션 행렬에서 '2번째 컬럼' = c - PRow3
    //    (row=0..3, col=2)
    FMatrix OutMatrix = InProjMatrix;
    OutMatrix.M[0][2] = c.X - PRow3.X;
    OutMatrix.M[1][2] = c.Y - PRow3.Y;
    OutMatrix.M[2][2] = c.Z - PRow3.Z;
    OutMatrix.M[3][2] = c.W - PRow3.W;

    return OutMatrix;
}

FPlane APortalActor::TransformPlane(const FPlane& Plane, const FMatrix& M)
{
    // Plane: (A, B, C, D)
    // 점 (x,y,z) 가 Plane 위에 있다는 것은 A x + B y + C z + D = 0
    // 변환 행렬 M이 주어졌을 때, 변환된 Plane’ 을 찾으려면 (x’, y’, z’)에 대한 식...
    // 구글 “transform plane by matrix” 하면 많이 나옵니다.

    // Matrix의 inverse-transpose 로 normal, inverse로 position 쪽을...
    // 간단히는 Eric Lengyel의 'Plane Transformation' 공식을 응용.

    FMatrix InvTranspose = M.InverseFast().GetTransposed();
    FVector4 planeV(Plane.X, Plane.Y, Plane.Z, Plane.W);
    FVector4 t = InvTranspose.TransformFVector4(planeV);
    return FPlane(t.X, t.Y, t.Z, t.W);
}

void APortalActor::SetClipPlane()
{
    if (!BlueCamCapture || !BlueDoorRoot)
        return;

    BlueCamCapture->bEnableClipPlane = true;
    BlueCamCapture->ClipPlaneBase = BlueDoor->GetComponentLocation() + BlueDoorArrow->GetForwardVector();  
    BlueCamCapture->ClipPlaneNormal = BlueDoorArrow->GetForwardVector();


    if (!OrangeCamCapture || !OrangeDoorRoot)
        return;

    OrangeCamCapture->bEnableClipPlane = true;
    OrangeCamCapture->ClipPlaneBase = OrangeDoor->GetComponentLocation() + OrangeDoorArrow->GetForwardVector(); //  *-0.01f;
    OrangeCamCapture->ClipPlaneNormal = OrangeDoorArrow->GetForwardVector();
}

void APortalActor::CheckAndResizeRenderTarget()
{
    if (!GEngine || !GEngine->GameViewport) return;

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);

    int32 CurrentWidth = FMath::TruncToInt(ViewportSize.X);
    int32 CurrentHeight = FMath::TruncToInt(ViewportSize.Y);

    if (BlueCamRenderTarget)
    {
        const int32 RTWidth = BlueCamRenderTarget->SizeX;
        const int32 RTHeight = BlueCamRenderTarget->SizeY;

        if (RTWidth != CurrentWidth || RTHeight != CurrentHeight)
        {
            UE_LOG(LogTemp, Warning, TEXT("RESIZING RenderTarget to match viewport (%d x %d)"), CurrentWidth, CurrentHeight);

            BlueCamRenderTarget->ResizeTarget(CurrentWidth, CurrentHeight);
        }
    }

    if (OrangeCamRenderTarget)
    {
        const int32 RTWidth = OrangeCamRenderTarget->SizeX;
        const int32 RTHeight = OrangeCamRenderTarget->SizeY;

        if (RTWidth != CurrentWidth || RTHeight != CurrentHeight)
        {
            UE_LOG(LogTemp, Warning, TEXT("RESIZING Orange RenderTarget to match viewport (%d x %d)"), CurrentWidth, CurrentHeight);

            OrangeCamRenderTarget->ResizeTarget(CurrentWidth, CurrentHeight);
        }
    }
}

void APortalActor::OnBluePortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    OtherComp->SetCollisionResponseToChannel(COLLISION_PORTAL_SURFACE, ECR_Ignore);

    if (!HasAuthority()) return;
    UE_LOG(LogTemp, Warning, TEXT("OnBluePortalOverlapBegin Called!"));
    if (UTravellerComponent* Traveller = OtherActor->FindComponentByClass<UTravellerComponent>())
    {
        BlueTrackedTravellers.Add(Traveller);
        FVector Offset = OtherActor->GetActorLocation() - BlueDoor->GetComponentLocation();
        Traveller->SetPreviousOffset(Offset);
    }
}

void APortalActor::OnBluePortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    OtherComp->SetCollisionResponseToChannel(COLLISION_PORTAL_SURFACE, ECR_Block);


    if (!HasAuthority()) return;
    UE_LOG(LogTemp, Warning, TEXT("OnBluePortalOverlapEnd Called!"));
    if (UTravellerComponent* Traveller = OtherActor->FindComponentByClass<UTravellerComponent>())
    {
        BlueTrackedTravellers.Remove(Traveller);
    }
}

void APortalActor::OnOrangePortalOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    OtherComp->SetCollisionResponseToChannel(COLLISION_PORTAL_SURFACE, ECR_Ignore);

    if (!HasAuthority()) return;
    UE_LOG(LogTemp, Warning, TEXT("OnOrangePortalOverlapBegin Called!"));
    if (UTravellerComponent* Traveller = OtherActor->FindComponentByClass<UTravellerComponent>())
    {
        OrangeTrackedTravellers.Add(Traveller);
        FVector Offset = OtherActor->GetActorLocation() - OrangeDoor->GetComponentLocation();
        Traveller->SetPreviousOffset(Offset);
    }

    

}

void APortalActor::OnOrangePortalOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    OtherComp->SetCollisionResponseToChannel(COLLISION_PORTAL_SURFACE, ECR_Block);

    if (!HasAuthority()) return;
    UE_LOG(LogTemp, Warning, TEXT("OnOrangePortalOverlapEnd Called!"));
    if (UTravellerComponent* Traveller = OtherActor->FindComponentByClass<UTravellerComponent>())
    {
        OrangeTrackedTravellers.Remove(Traveller);
    }
    
}

void APortalActor::ProcessPortalTravel(TSet<UTravellerComponent*>& FromSet, TSet<UTravellerComponent*>& ToSet, USceneComponent* FromArrow, USceneComponent* ToArrow)
{
    const float Threshold = 0.f;

    //FString Arrowname = TEXT(""); //  = FromArrow->GetName();
    //if (FromArrow == BlueDoorArrow)
    //{
    //    Arrowname = TEXT("BlueDoor");
    //}
    //else if (FromArrow == OrangeDoorArrow)
    //{
    //    Arrowname = TEXT("OrangeDoor");
    //}


    for (UTravellerComponent* Traveller : FromSet)
    {
        if (!IsValid(Traveller)) continue;

        AActor* Actor = Traveller->GetOwner();

        // Grabbed된 Traveller의 경우
        if (Traveller->IsTravellerGrabbed())
        {
            UE_LOG(LogTemp, Warning, TEXT("Traveller is Currently Grabbed!"));


            if (!(Traveller->HasClone()) && Cast<ACharacter>(Traveller->GetOwner()) == nullptr) // HasClone()는 bool 함수로 구현하거나 CloneActor null 체크
            {
                Traveller->CreateClone();
            }

            // 기존에 사용하던 방식
            Traveller->UpdateCloneTransform(FromArrow->GetComponentTransform(), ToArrow->GetComponentTransform());
        }
    

        // Grabbed된 Traveller가 아닌 경우 
        else
        {
            // 기준이 되는 포탈 위치와 방향을 ArrowComponent에서 얻음
            const FVector FromLocation = FromArrow->GetComponentLocation();
            const FVector FromForward = FromArrow->GetForwardVector();

            const FVector ToLocation = ToArrow->GetComponentLocation();
            const FRotator ToRotation = ToArrow->GetComponentRotation();

            FVector CurrentOffset = Actor->GetActorLocation() - FromLocation;
            FVector PreviousOffset = Traveller->GetPreviousOffset();

            float DotNow = FVector::DotProduct(CurrentOffset, FromForward);
            float DotPrev = FVector::DotProduct(PreviousOffset, FromForward);

            if (FMath::Sign(DotNow) < 0.0f && FMath::Sign(DotPrev) > 0.0f)
            {
                FVector CurrentActorLocation = Actor->GetActorLocation();
                FVector CurrentArrowLocation = FromArrow->GetComponentLocation();

                {
                    // 상대 위치 변환 (FromArrow 기준으로 ToArrow 쪽으로)
                    FTransform Local = Actor->GetActorTransform().GetRelativeTransform(FromArrow->GetComponentTransform());
                    FTransform NewWorld = Local * ToArrow->GetComponentTransform();
                    Traveller->Teleport(FromArrow->GetComponentTransform(), ToArrow->GetComponentTransform());

                    // Impulse 주기
                    UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Traveller->GetOwner()->GetRootComponent());
                    if (Primitive && Primitive->IsSimulatingPhysics())
                    {
                        AActor* TravellerOwnerOwner = Traveller->GetOwner();
                        if (!Cast<ACharacter>(TravellerOwnerOwner)) // 캐릭터는 제외
                        {
                            const FTransform FromTransform = FromArrow->GetComponentTransform();
                            const FTransform ToTransform = ToArrow->GetComponentTransform();

                            // ---- 선속도 (Linear Velocity) 처리 ----
                            {
                                const FVector OldVelocity = Primitive->GetComponentVelocity();

                                // 1. From 기준으로 로컬화
                                const FVector LocalVel = FromTransform.InverseTransformVectorNoScale(OldVelocity);

                                // 2. Y축(오른쪽) 기준 180도 회전
                                const FQuat Flip = FQuat(FVector::RightVector, PI);
                                const FVector FlippedLocalVel = Flip.RotateVector(LocalVel);

                                // 3. 다시 To 기준으로 월드 변환
                                const FVector FinalVel = ToTransform.TransformVectorNoScale(FlippedLocalVel);

                                // 4. 적용
                                Primitive->SetPhysicsLinearVelocity(FinalVel);
                            }

                            // ---- 각속도 (Angular Velocity) 처리 ----
                            {
                                const FVector OldAngularDeg = Primitive->GetPhysicsAngularVelocityInDegrees();

                                // 1. From 기준으로 로컬화
                                const FVector LocalAngVel = FromTransform.InverseTransformVectorNoScale(OldAngularDeg);

                                // 2. Flip
                                const FQuat Flip = FQuat(FVector::RightVector, PI);
                                const FVector FlippedLocalAng = Flip.RotateVector(LocalAngVel);

                                // 3. To 기준으로 다시 변환
                                const FVector FinalAngVel = ToTransform.TransformVectorNoScale(FlippedLocalAng);

                                // 4. 적용
                                Primitive->SetPhysicsAngularVelocityInDegrees(FinalAngVel);
                            }

                        }
                    }
                    


                    FVector AfterActorLocation = Actor->GetActorLocation();
                    //UE_LOG(LogTemp, Warning, TEXT("[%s] After Teleport ActorLocation : (%f, %f, %f)"), *Arrowname, AfterActorLocation.X, AfterActorLocation.Y, AfterActorLocation.Z);
                    Traveller->SetPreviousOffset(Actor->GetActorLocation() - ToLocation);
                    float DotTleportedPrev = FVector::DotProduct(Traveller->GetPreviousOffset(), ToArrow->GetForwardVector());
                    //UE_LOG(LogTemp, Warning, TEXT("[%s]After Teleport :  NewDotPrev :  %f"), *Arrowname, DotTleportedPrev);

                    ToSet.Add(Traveller);
                    FromSet.Remove(Traveller);

                    Traveller->DestoryClone();
                    break;
                }

            }
            else
            {
                Traveller->SetPreviousOffset(CurrentOffset);

                if (!(Traveller->HasClone()) && Cast<ACharacter>(Traveller->GetOwner()) == nullptr) // HasClone()는 bool 함수로 구현하거나 CloneActor null 체크
                {
                    Traveller->CreateClone();
                }

                Traveller->UpdateCloneTransform(FromArrow->GetComponentTransform(), ToArrow->GetComponentTransform());


                // Slicing. 만드는 프로젝트에서는 벽에 딱 붙여 설치할 것이므로 Slicing이 필요없음 
                {
                    const FVector SliceOrigin = FromArrow->GetComponentLocation();
                    const FVector SliceNormal = FromArrow->GetForwardVector();
                    //Traveller->SetSliceParams(SliceOrigin, SliceNormal, false); // 원본

                    const FVector CloneOrigin = ToArrow->GetComponentLocation();
                    const FVector CloneNormal = -ToArrow->GetForwardVector(); // 반대 방향
                    //Traveller->SetSliceParams(CloneOrigin, CloneNormal, true); // 복제본
                }
            }
        }
    }
}

void APortalActor::HandleWeaponChanged(UMeshComponent* NewWeaponMesh)
{
    if (NewWeaponMesh && OrangeCamCapture && BlueCamCapture)
    {
        OrangeCamCapture->HiddenComponents.Add(NewWeaponMesh);
        BlueCamCapture->HiddenComponents.Add(NewWeaponMesh);
    }
}

void APortalActor::Multicast_SetPortalHiddenComponents_Implementation()
{
    AGameStateBase* GameState = GetWorld()->GetGameState();
    if (GameState && OrangeCamCapture && BlueCamCapture)
    {
        for (APlayerState* PlayerState : GameState->PlayerArray)
        {
            if (APawn* Pawn = PlayerState->GetPawn())
            {
                APortalLobbyCharacterPlayer* Char = Cast<APortalLobbyCharacterPlayer>(Pawn);
                if (Char && Char->FirstPersonMesh)
                {
                    OrangeCamCapture->HiddenComponents.Add(Char->FirstPersonMesh);
                    BlueCamCapture->HiddenComponents.Add(Char->FirstPersonMesh);

                    // 자식 컴포넌트들 전부 탐색해서 숨김 설정
                    TArray<USceneComponent*> ChildrenArray;
                    Char->FirstPersonMesh->GetChildrenComponents(true, ChildrenArray);

                    for (USceneComponent* ChildComp : ChildrenArray)
                    {
                        if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(ChildComp))
                        {
                            OrangeCamCapture->HiddenComponents.Add(Prim);
                            BlueCamCapture->HiddenComponents.Add(Prim);
                        }
                    }
                }
            }
        }
    }
}

 

//void APortalActor::ProcessPortalTravel(TSet<UTravellerComponent*>& FromSet, TSet<UTravellerComponent*>& ToSet, USceneComponent* FromDoor, USceneComponent* ToDoor)
//{
//    for (UTravellerComponent* Traveller : FromSet)
//    {
//        if (!IsValid(Traveller)) continue;
//
//        AActor* Actor = Traveller->GetOwner();
//        FVector CurrentOffset = Actor->GetActorLocation() - FromDoor->GetComponentLocation();
//        FVector PreviousOffset = Traveller->GetPreviousOffset();
//
//
//
//        float DotNow = FVector::DotProduct(CurrentOffset, FromDoor->GetForwardVector());
//        float DotPrev = FVector::DotProduct(PreviousOffset, FromDoor->GetForwardVector());
//
//
//        if (FMath::Sign(DotNow) != FMath::Sign(DotPrev))
//        {
//            // Teleport 처리
//            FTransform Local = Actor->GetActorTransform().GetRelativeTransform(FromDoor->GetComponentTransform());
//            FTransform NewWorld = Local * ToDoor->GetComponentTransform();
//            Traveller->Teleport(FromDoor->GetComponentTransform(), ToDoor->GetComponentTransform());
//
//            Traveller->SetPreviousOffset(Actor->GetActorLocation() - ToDoor->GetComponentLocation());
//
//            ToSet.Add(Traveller);
//            FromSet.Remove(Traveller);
//            break; // 수정 중이므로 break 필요
//        }
//        else
//        {
//            Traveller->SetPreviousOffset(CurrentOffset);
//        }
//    }
//}

 