// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TravellerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/PortalLobbyPlayerController.h"
#include "Character/PortalLobbyCharacterPlayer.h"
#include "Camera/CameraComponent.h"

// Sets default values for this component's properties
UTravellerComponent::UTravellerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TeleportIgnoreDuration = 0.2f; 
	IgnoreTimeRemaining = 0.0f; 
	// ...

	bIsGrabbed = false; 
}

// Called when the game starts
void UTravellerComponent::BeginPlay()
{
	Super::BeginPlay();

	// 그래픽용 컴포넌트 찾기 (여기선 StaticMeshComponent 기준, 필요 시 커스터마이징)
	GraphicsComponent = Cast<UStaticMeshComponent>(GetOwner()->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if (GraphicsComponent)
	{
		// 머티리얼 복사
		for (int32 i = 0; i < GraphicsComponent->GetNumMaterials(); ++i)
		{
			UMaterialInterface* BaseMat = GraphicsComponent->GetMaterial(i);
			if (BaseMat)
			{
				UMaterialInstanceDynamic* DynMat = UMaterialInstanceDynamic::Create(BaseMat, this);
				OriginalMaterials.Add(DynMat);
				GraphicsComponent->SetMaterial(i, DynMat);
			}
		}
	}

}

// Called every frame
void UTravellerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IgnoreTimeRemaining > 0.f)
	{
		IgnoreTimeRemaining -= DeltaTime;
	}
}


void UTravellerComponent::Teleport(const FTransform& FromPortal, const FTransform& ToPortal)
{
	if (AActor* Owner = GetOwner())
	{
		const FTransform OwnerWorldTransform = Owner->GetActorTransform();

		// 1. Owner 위치/회전을 FromPortal 기준 로컬로 변환
		const FVector LocalLoc = FromPortal.InverseTransformPosition(OwnerWorldTransform.GetLocation());
		const FQuat LocalRot = FromPortal.InverseTransformRotation(OwnerWorldTransform.GetRotation());

		// 2. Y축(RightVector) 기준 180도 회전 (좌우 반사)
		const FQuat Flip = FQuat(FVector::RightVector, PI);
		const FVector FlippedLoc = Flip.RotateVector(LocalLoc);
		const FQuat FlippedRot = Flip * LocalRot;

		// 3. ToPortal 기준으로 다시 월드 좌표로 변환
		const FVector FinalWorldLoc = ToPortal.TransformPosition(FlippedLoc);
		const FQuat FinalWorldRot = ToPortal.TransformRotation(FlippedRot);

		const FTransform FinalWorldTransform(FinalWorldRot, FinalWorldLoc, OwnerWorldTransform.GetScale3D());

		// 4. 실제 적용
		if (APortalLobbyCharacterPlayer* Character = Cast<APortalLobbyCharacterPlayer>(Owner))
		{
			const FVector TargetLocation = FinalWorldTransform.GetLocation();
			const FRotator TargetRotation = FinalWorldTransform.Rotator();

			const FVector Forward = FVector::ForwardVector;  
			const FVector Up = FVector::UpVector;
			const FVector Right = FVector::RightVector;
			//const FQuat UprightRot = FRotationMatrix::MakeFromXZ(Forward, Up).ToQuat();

			// Forward를 기준으로 Z를 강제한 뒤 → 다시 Right 계산해서 안정화

			FVector FinalForward = Forward.GetSafeNormal();
			FVector FinalUp = Up.GetSafeNormal();
			FVector FinalRight = Right.GetSafeNormal();

			FVector::CreateOrthonormalBasis(FinalForward, FinalRight, FinalUp);

			const FMatrix RotMatrix(FinalForward, FinalRight, FinalUp, FVector::ZeroVector);
			const FQuat UprightRot = FQuat(RotMatrix);
			// 이동
			Character->TeleportTo(TargetLocation, TargetRotation);
			
			//Character->TeleportTo(TargetLocation, UprightRot.Rotator());

			// 회전 동기화
			if (AController* Controller = Character->GetController())
			{
				Controller->SetControlRotation(TargetRotation);
				//Controller->SetControlRotation(UprightRot.Rotator());
				Character->Client_SetControlRotation(TargetRotation);
			}

			APortalLobbyCharacterPlayer* lobbycharacter = Cast<APortalLobbyCharacterPlayer>(Character);


			const FVector OldVelocity = Character->GetVelocity();

			// 1. 월드 속도를 FromPortal 기준 로컬로 변환
			const FVector LocalVel = FromPortal.InverseTransformVectorNoScale(OldVelocity);

			// 2. Y축 기준 Flip
			const FVector FlippedLocalVel = Flip.RotateVector(LocalVel);

			// 3. 다시 ToPortal 기준으로 월드 변환
			const FVector NewVelocity = ToPortal.TransformVectorNoScale(FlippedLocalVel);

			// 추가 회전 보정 (캐릭터의 새 기준 좌표계에 맞춤)
			const FVector FinalVelocity = UprightRot.RotateVector(NewVelocity);

			// 4. 적용
			Character->GetCharacterMovement()->Velocity = NewVelocity;

			Character->FirstPersonCamera->SetActive(false);
			GetWorld()->GetTimerManager().SetTimerForNextTick([this, Character]()
				{
					Character->FirstPersonCamera->SetActive(true);
				});
			 
		}
		else
		{
			// 일반 Actor 처리
			Owner->SetActorTransform(FinalWorldTransform);
		}

		// Overlap 무시 타이머
		IgnoreTimeRemaining = TeleportIgnoreDuration;
	}
}

void UTravellerComponent::UpdateCloneTransform(const FTransform& MirrorTransform)
{
	if (CloneActor)
	{
		CloneActor->SetActorTransform(MirrorTransform);
	}
}

UStaticMeshComponent* UTravellerComponent::GetGraphicsComponent()
{
	return GraphicsComponent;
}

void UTravellerComponent::SetSliceParams(float Offset, FVector Normal, bool bForClone)
{
	/*const TArray<UMaterialInstanceDynamic*>& Materials = bForClone ? CloneMaterials : OriginalMaterials;

	for (UMaterialInstanceDynamic* Mat : Materials)
	{
		if (Mat)
		{
			Mat->SetScalarParameterValue(TEXT("sliceOffsetDst"), Offset);
			Mat->SetVectorParameterValue(TEXT("sliceNormal"), Normal);
		}
	}*/
}

FVector UTravellerComponent::GetPreviousOffset() const
{
	return PreviousOffset;
}

void UTravellerComponent::SetPreviousOffset(const FVector& Offset)
{
	PreviousOffset = Offset;
}

void UTravellerComponent::CreateClone()
{
	if (!GetOwner()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 같은 클래스로 복제
	FActorSpawnParameters Params;
	Params.Template = GetOwner(); // 복제할 Actor 템플릿 지정
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CloneActor = World->SpawnActor<AActor>(GetOwner()->GetClass(), GetOwner()->GetActorTransform(), Params);
	//CloneActor = World->SpawnActor<AActor>(GetOwner()->GetClass(), GetOwner()->GetActorTransform());

	if (!CloneActor) return;

	// 복제본은 렌더링용이라 충돌 제거
	TArray<UPrimitiveComponent*> Components;
	CloneActor->GetComponents<UPrimitiveComponent>(Components);
	for (auto Comp : Components)
	{
		if (Comp)
		{
			Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}

	// 머티리얼 복사 (복제먼저 확인할 것이라 생략)
	/*UPrimitiveComponent* SourceMesh = GetOwner()->FindComponentByClass<UPrimitiveComponent>();
	UPrimitiveComponent* CloneMesh = CloneActor->FindComponentByClass<UPrimitiveComponent>();

	if (SourceMesh && CloneMesh)
	{
		int32 NumMaterials = SourceMesh->GetNumMaterials();
		for (int32 i = 0; i < NumMaterials; ++i)
		{
			UMaterialInterface* BaseMat = SourceMesh->GetMaterial(i);
			auto DynamicMatOriginal = UMaterialInstanceDynamic::Create(BaseMat, this);
			auto DynamicMatClone = UMaterialInstanceDynamic::Create(BaseMat, this);

			SourceMesh->SetMaterial(i, DynamicMatOriginal);
			CloneMesh->SetMaterial(i, DynamicMatClone);

			OriginalMaterials.Add(DynamicMatOriginal);
			CloneMaterials.Add(DynamicMatClone);
		}
	}*/
}

void UTravellerComponent::UpdateCloneTransform(const FTransform& FromPortal, const FTransform& ToPortal)
{
	if (!CloneActor || !GetOwner()) return;

	const AActor* Owner = GetOwner();
	const FTransform OwnerWorldTransform = Owner->GetActorTransform();

	// 1. Owner 위치/회전을 FromPortal 기준 로컬로 변환
	const FVector LocalLoc = FromPortal.InverseTransformPosition(OwnerWorldTransform.GetLocation());
	const FQuat LocalRot = FromPortal.InverseTransformRotation(OwnerWorldTransform.GetRotation());

	// 2. Y축 기준 180도 회전 (거울 반사)
	const FQuat Flip = FQuat(FVector::RightVector, PI);
	const FVector FlippedLoc = Flip.RotateVector(LocalLoc);
	const FQuat FlippedRot = Flip * LocalRot;

	// 3. ToPortal 기준으로 다시 월드 좌표로 변환
	const FVector FinalWorldLoc = ToPortal.TransformPosition(FlippedLoc);
	const FQuat FinalWorldRot = ToPortal.TransformRotation(FlippedRot);

	// 4. 적용
	CloneActor->SetActorTransform(FTransform(FinalWorldRot, FinalWorldLoc, OwnerWorldTransform.GetScale3D()));
}

void UTravellerComponent::SetSliceParams(const FVector& SliceOrigin, const FVector& SliceNormal, bool bIsClone)
{
	/*const TArray<UMaterialInstanceDynamic*>& Materials = bIsClone ? CloneMaterials : OriginalMaterials;

	for (auto Mat : Materials)
	{
		if (Mat)
		{
			Mat->SetVectorParameterValue(FName("sliceCentre"), SliceOrigin);
			Mat->SetVectorParameterValue(FName("sliceNormal"), SliceNormal);
		}
	}*/
}

void UTravellerComponent::DestoryClone()
{
	if (CloneActor)
	{
		CloneActor->Destroy();
		CloneActor = nullptr;
	}
}

bool UTravellerComponent::IsIgnoringOverlap() const
{
	return IgnoreTimeRemaining > 0.f;
}



 

 
