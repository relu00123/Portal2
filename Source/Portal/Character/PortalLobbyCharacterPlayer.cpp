// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PortalLobbyCharacterPlayer.h"
#include "UI/PortalLobbyHUDWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/WeaponComponent.h"
#include "Net/UnrealNetwork.h"
#include "Animation/PortalPlayerAnimation.h"
#include "Animation/PortalThirdPlayerAnimation.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Player/PortalLobbyPlayerController.h"
#include "Prop/PortalActor.h"
#include "Weapon/PortalGunComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Interface/PickableObjectInterface.h"
#include "Components/GrabInteractableComponent.h"
#include "Components/TravellerComponent.h"

APortalLobbyCharacterPlayer::APortalLobbyCharacterPlayer()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true; // 네트워크 복제 활성화

	// 1인칭 카메라
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(RootComponent);
	FirstPersonCamera->bUsePawnControlRotation = true;

	// 1인칭 메시 (손 + 무기만)
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FirstPersonMesh->SetupAttachment(FirstPersonCamera);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	FirstPersonMesh->bOnlyOwnerSee = true;  // 본인만 볼 수 있도록 설정

	// 3인칭 메시 (전체 캐릭터)
	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonMesh"));
	ThirdPersonMesh->SetupAttachment(RootComponent);
	ThirdPersonMesh->bOwnerNoSee = true;  // 본인은 보이지 않도록 설정


	// Grab
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	//PhysicsHandle->InterpolationSpeed = 50.0f;
	//PhysicsHandle->LinearStiffness = 50.0f;
	//PhysicsHandle->LinearDamping = 1.0f;
	//PhysicsHandle->AngularStiffness = 50.0f;
	//PhysicsHandle->AngularDamping = 1.0f;
	////PhysicsHandle->SoftLinearConstraint = true;
	////PhysicsHandle->SoftAngularConstraint = true;


	// Traveller
	TravellerComponent = CreateDefaultSubobject<UTravellerComponent>(TEXT("TravellerComponent"));



	// 초기화 
	CurrentWeapon = nullptr;
	PistolWeapon = nullptr;
	PortalGunWeapon = nullptr;
	bIsAiming = false; 
	OwningPortal = nullptr; 
	AimPitch = 0.0f;
}

void APortalLobbyCharacterPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortalLobbyCharacterPlayer, CurrentWeapon);
	DOREPLIFETIME(APortalLobbyCharacterPlayer, bIsAiming);
	DOREPLIFETIME(APortalLobbyCharacterPlayer, PistolWeapon);
	DOREPLIFETIME(APortalLobbyCharacterPlayer, PortalGunWeapon);
	DOREPLIFETIME(APortalLobbyCharacterPlayer, OwningPortal);
	DOREPLIFETIME(APortalLobbyCharacterPlayer, AimPitch);
	DOREPLIFETIME(APortalLobbyCharacterPlayer, HeldComponent);
}

void APortalLobbyCharacterPlayer::BeginPlay()
{
	Super::BeginPlay(); 


	// InputMappingContext 등록
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(GameplayMappingContext, 0);
		}
	}

}

void APortalLobbyCharacterPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		UpdateGrabbedObjectLocation();
	}

	if (HasAuthority())
	{
		if (bShouldBlendRotation)
		{
			const float BlendSpeed = 15.f; // 원하는 속도
			BlendAlpha = FMath::Clamp(BlendAlpha + DeltaTime * BlendSpeed, 0.f, 1.f);

			FRotator BlendedRot = FMath::Lerp(InitialRotation, TargetRotation, BlendAlpha);
			SetActorRotation(BlendedRot);

			if (BlendAlpha >= 1.f)
			{
				bShouldBlendRotation = false;
			}
		}
	}

	// Test
	//FVector2D ViewportSize;
	//GEngine->GameViewport->GetViewportSize(ViewportSize);
	//UE_LOG(LogTemp, Warning, TEXT("Viewport Size : %f %f"), ViewportSize.X, ViewportSize.Y);
}

void APortalLobbyCharacterPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::StartAiming);
		EnhancedInput->BindAction(AimAction, ETriggerEvent::Completed, this, &APortalLobbyCharacterPlayer::StopAiming);
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::FireRifle);
		EnhancedInput->BindAction(RightFireAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::FireRightRifle);
		EnhancedInput->BindAction(SwitchWeaponToPortalgunAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::SwitchToPortalGun);
		EnhancedInput->BindAction(SwitchWeaponToRifleAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::SwitchToRifle);
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::Look);
		EnhancedInput->BindAction(GrabAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::GrabObject);
		EnhancedInput->BindAction(SwitchMaterialAction, ETriggerEvent::Triggered, this, &APortalLobbyCharacterPlayer::SwitchMaterial);
	}
}

void APortalLobbyCharacterPlayer::GrabObject(const FInputActionValue& Value)
{
	if (CurrentWeapon && CurrentWeapon->WeaponType == EWeaponType::PORTAL_GUN)
	{
		if (HeldComponent)
		{
			ServerReleaseObject();
		}
		else
		{
			ServerGrabObject();
		}
	}
}

void APortalLobbyCharacterPlayer::SwitchMaterial(const FInputActionValue& Value)
{
	if (OwningPortal)
	{
		OwningPortal->SwitchVisibilityControl();
	}
}

void APortalLobbyCharacterPlayer::ServerGrabObject_Implementation()
{
	TryGrabObject();
}

void APortalLobbyCharacterPlayer::ServerReleaseObject_Implementation()
{
	//if (PhysicsHandle && HeldComponent)
	//{
	//	PhysicsHandle->ReleaseComponent();
	//	HeldComponent = nullptr;
	//}

	if (PhysicsHandle && HeldComponent)
	{
		HeldComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		AActor* GrabbedActor = HeldComponent->GetOwner();

		// Grab 컴포넌트 찾기
		if (UGrabInteractableComponent* GrabComp = GrabbedActor->FindComponentByClass<UGrabInteractableComponent>())
		{
			// 나 자신이 잡은 경우에만 해제
			if (GrabComp->GrabbedBy == this)
			{
				GrabComp->ClearGrabbedBy();

				// 물리 해제
				PhysicsHandle->ReleaseComponent();
				HeldComponent = nullptr;


				// TraverllerComponent라면 Grab상태를 바꿔준다. 
				if (UTravellerComponent* Traveller = GrabbedActor->FindComponentByClass<UTravellerComponent>())
				{
					Traveller->SetTravellerGrabbed(false); 
				}

				UE_LOG(LogTemp, Warning, TEXT("Server: Released object."));
			}
		}

		 
	}
}

void APortalLobbyCharacterPlayer::TryGrabObject() // 서버에서만 실행되는 로직 
{
	FVector Start;
	FRotator ViewRot;
	GetController()->GetPlayerViewPoint(Start, ViewRot);

	FVector End = Start + ViewRot.Vector() * 250.f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
	{
		// DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.0f, 0, 3.0f);

		UPrimitiveComponent* HitComp = Hit.GetComponent();
		AActor* HitActor = Hit.GetActor();


		if (HitActor && HitComp && HitComp->IsSimulatingPhysics())
		{
			if (UGrabInteractableComponent* GrabComp = HitActor->FindComponentByClass<UGrabInteractableComponent>())
			{
				if (GrabComp->CanBeGrabbedBy(this))
				{
					GrabComp->SetGrabbedBy(this);

					FVector GrabLocation = HitComp->GetComponentLocation() + FVector(0.f, 0.f, HitComp->Bounds.BoxExtent.Z * 0.5f);

					PhysicsHandle->GrabComponentAtLocationWithRotation(
						HitComp,
						NAME_None,
						//Hit.ImpactPoint,
						GrabLocation,
						HitComp->GetComponentRotation()
					);

					HeldComponent = HitComp;

					// 캐릭터와 충돌 무시 코드 
					HeldComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);


					// 포탈 Traveller라면 Grabbed로 바꿔준다. 
					if (AActor* HitActorOwner = HitComp->GetOwner())
					{
						if (UTravellerComponent* Traveller = HitActorOwner->FindComponentByClass<UTravellerComponent>())
						{
							Traveller->SetTravellerGrabbed(true);
							UE_LOG(LogTemp, Warning, TEXT("%s is now grabbed."), *HitActorOwner->GetName());
						}
					}
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Object already grabbed!"));
				}
			}
		}
	}
}

void APortalLobbyCharacterPlayer::UpdateGrabbedObjectLocation()
{
	if (PhysicsHandle && PhysicsHandle->GrabbedComponent)
	{
		FVector Start = FirstPersonCamera->GetComponentLocation();
		FRotator ViewRot = GetControlRotation(); // 서버에 공유된 회전 정보
		FVector GrabTargetLocation = Start + ViewRot.Vector() * 200.f;
		PhysicsHandle->SetTargetLocation(GrabTargetLocation);
	}
}

void APortalLobbyCharacterPlayer::Look(const FInputActionValue& Value)
{
	if (IsLocallyControlled())
	{
		// 1. 마우스 Y 입력 받아오기 (Axis1D)
		const float LookUpValue = Value.Get<float>() * 1.0f; // 값 조절 필요시 스케일링

		// 2. 카메라에 Pitch 반영 (Controller 회전 누적됨)
		AddControllerPitchInput(-LookUpValue);

		// 3. 현재 누적된 컨트롤 회전(Pitch)을 가져와서 서버에 전달
		float LocalPitch = FMath::Clamp(FRotator::NormalizeAxis(GetControlRotation().Pitch), -90.f, 90.f);


		//UE_LOG(LogTemp, Warning, TEXT("Local Pitch Value : %f"), LocalPitch);

		ServerUpdateAimPitch(LocalPitch); // 서버로 Pitch 전달
	}
}

void APortalLobbyCharacterPlayer::ServerUpdateAimPitch_Implementation(float NewPitch)
{
	AimPitch = NewPitch;
	//UE_LOG(LogTemp, Warning, TEXT("AimOffset from Character : %f"), AimPitch);
}

// 서버에서만 호출되는 함수 
bool APortalLobbyCharacterPlayer::EquipWeapon(UWeaponComponent* NewWeapon)
{
	if (!NewWeapon) return false;

	EWeaponType Type = NewWeapon->WeaponType;

	if (Type == EWeaponType::RIFLE)
	{
		if (PistolWeapon != nullptr) return false;

		PistolWeapon = NewWeapon;
	}

	else if (Type == EWeaponType::PORTAL_GUN)
	{
		if (PortalGunWeapon != nullptr) return false; 
		PortalGunWeapon = NewWeapon;
	}

	else
	{
		return false; // RIFLE혹은 PORTAL_GUN이 아닐경우 예외처리. 
	}

	// 컴포넌트 등록 및 소켓 부착 ?
	//FName SocketName = FName("GripPoint");
	//NewWeapon->SetupAttachment(FirstPersonMesh, SocketName);

	if (!NewWeapon->IsRegistered())
	{
		NewWeapon->RegisterComponent();
	}
	 
	//NewWeapon->SetVisibility(false); // 기본적으로 숨김 
	//NewWeapon->RegisterComponent();

	SwitchWeapon(Type);

	//OnRep_CurrentWeapon();

	return true;
}

void APortalLobbyCharacterPlayer::OnRep_CurrentWeapon()
{
	// 새로운 코드
	if (!CurrentWeapon) return;

	if (CurrentWeapon->WeaponType == EWeaponType::PORTAL_GUN)
		HideWeapon(EWeaponType::RIFLE);
	else if (CurrentWeapon->WeaponType == EWeaponType::RIFLE)
		HideWeapon(EWeaponType::PORTAL_GUN);

	CurrentWeapon->SetVisibility(true);

	if (IsLocallyControlled() && FirstPersonMesh)
	{
		// 자기자신 (1인칭)
		FName SocketName = FName("GripPoint");
		CurrentWeapon->AttachToComponent(FirstPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
		UPortalPlayerAnimation* AnimInstance = Cast<UPortalPlayerAnimation>(FirstPersonMesh->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->HasRifle = true;
			AnimInstance->WeaponType = CurrentWeapon->WeaponType;
		}

		// 거울 봤을때 용도  (3인칭)
		UPortalThirdPlayerAnimation* ThirdAnimInstance = Cast<UPortalThirdPlayerAnimation>(ThirdPersonMesh->GetAnimInstance());

		if (ThirdAnimInstance)
		{
			ThirdAnimInstance->HasRifle = true;
			ThirdAnimInstance->WeaponType = CurrentWeapon->WeaponType;
		}

		if (MirrorWeaponMesh)
		{
			MirrorWeaponMesh->DestroyComponent();
			MirrorWeaponMesh = nullptr;
		}

		MirrorWeaponMesh = NewObject<USkeletalMeshComponent>(this);
		MirrorWeaponMesh->RegisterComponent();
		MirrorWeaponMesh->SetSkeletalMesh(CurrentWeapon->SkeletalMesh);
		MirrorWeaponMesh->AttachToComponent(ThirdPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
		MirrorWeaponMesh->SetOwnerNoSee(true);
		//MirrorWeaponMesh->bOwnerNoSee = true;
	}

	else if (!IsLocallyControlled() && ThirdPersonMesh)
	{
		FName SocketName = FName("GripPoint");
		CurrentWeapon->AttachToComponent(ThirdPersonMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);

		UPortalThirdPlayerAnimation* AnimInstance = Cast<UPortalThirdPlayerAnimation>(ThirdPersonMesh->GetAnimInstance());

		if (AnimInstance)
		{
			AnimInstance->HasRifle = true;
			AnimInstance->WeaponType = CurrentWeapon->WeaponType;
		}
	}

	// Delegate호출
	if (IsLocallyControlled())
		OnWeaponMeshChanged.Broadcast(CurrentWeapon);
}

// 클라이언트 호출 (Input Action Binding)
void APortalLobbyCharacterPlayer::SwitchToRifle(const FInputActionValue& Value)
{
	if (PistolWeapon == nullptr) return; 
	ServerSwitchWeapon(EWeaponType::RIFLE);
}

// 클라이언트 호출 (Input Action Binding)
void APortalLobbyCharacterPlayer::SwitchToPortalGun(const FInputActionValue& Value)
{
	if (PortalGunWeapon == nullptr) return;
	ServerSwitchWeapon(EWeaponType::PORTAL_GUN);
}

// 서버에서 호출 
void APortalLobbyCharacterPlayer::SwitchWeapon(EWeaponType weaponType)
{
	UE_LOG(LogTemp, Warning, TEXT("Switch Weapon Called"));

	if (!HasAuthority()) return; 

	if (weaponType == EWeaponType::PORTAL_GUN)
	{
		// Rifle이 있다면 숨겨야한다.
	/*	if (PistolWeapon != nullptr)
		{
			PistolWeapon->SetVisibility(false);
		}*/

		// CurrentWeapon을 PortalGun으로 만들어 줘야 한다.
		if (PortalGunWeapon != nullptr)
		{
			CurrentWeapon = PortalGunWeapon;
		}
	}

	else if (weaponType == EWeaponType::RIFLE)
	{
		// Portalgun이 있다면 숨겨야 한다.
		/*if (PortalGunWeapon != nullptr)
		{
			PortalGunWeapon->SetVisibility(false);
		}*/

		if (PistolWeapon != nullptr)
		{
			CurrentWeapon = PistolWeapon;
		}

		// 들고 있는 물체가 있으면 떨어뜨려야 한다. 
		ServerReleaseObject();
	}

	OnRep_CurrentWeapon();
}

// 서버에서 호출되는 함수 
void APortalLobbyCharacterPlayer::ServerSwitchWeapon_Implementation(EWeaponType weaponType)
{
	SwitchWeapon(weaponType);
}

void APortalLobbyCharacterPlayer::HideWeapon(EWeaponType weaponType)
{
	UWeaponComponent* Hidingweapon = nullptr;
	if (weaponType == EWeaponType::PORTAL_GUN)
		Hidingweapon = PortalGunWeapon;
	else if (weaponType == EWeaponType::RIFLE)
		Hidingweapon = PistolWeapon;

	if (Hidingweapon != nullptr)
		Hidingweapon->SetVisibility(false);
}

void APortalLobbyCharacterPlayer::ServerSetIsAiming_Implementation(bool bNewAiming)
{
	bIsAiming = bNewAiming;
}

void APortalLobbyCharacterPlayer::StartAiming(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Start Aiming Called!"));

	if (CurrentWeapon && CurrentWeapon->WeaponType == EWeaponType::RIFLE)
	{
		if (HasAuthority())
		{
			bIsAiming = true;
		}
		else
		{
			ServerSetIsAiming(true);  
		}
	}
}

void APortalLobbyCharacterPlayer::StopAiming(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Aiming Called!"));

	if (HasAuthority())
	{
		bIsAiming = false;
	}
	else
	{
		ServerSetIsAiming(false);  
	}
}

void APortalLobbyCharacterPlayer::HandleWeaponFire(EMouseClickType ClickType)
{
	FVector Start;
	FRotator Direction;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->GetPlayerViewPoint(Start, Direction);
	}

	FVector End = Start + Direction.Vector() * 10000.0f;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// 무기 타입에 따라 트레이스 채널 선택
	ECollisionChannel TraceChannel = ECC_Visibility;
	if (CurrentWeapon && CurrentWeapon->WeaponType == EWeaponType::PORTAL_GUN)
	{
		TraceChannel = COLLISION_PORTAL_RAY;
	}

	// 첫번째로 막은 Actor 1개만 결과에 담긴다. 
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);

	if (bHit)
	{
		// 디버깅용도 
		if (Hit.Component.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("Component: %s"), *Hit.Component->GetName());

			if (Hit.Component->GetOwner())
			{
				UE_LOG(LogTemp, Warning, TEXT("Owner Actor: %s"), *Hit.Component->GetOwner()->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Owner Actor is nullptr"));
			}
		}

		// 맞은 지점에 Impact 재생용도 
		MulticastPlayImpactEffect(Hit.ImpactPoint, Hit.ImpactNormal);

		if (CurrentWeapon)
		{
			CurrentWeapon->HandleHit(Hit, Direction.Vector(), ClickType);
		}
	}

	// 서버에서 디버그 선
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0, 1.0f);

	// 캐릭터 애니메이션 재생용도 
	MulticastFireRifle();
}

void APortalLobbyCharacterPlayer::ServerFireRifle_Implementation()
{
	HandleWeaponFire(EMouseClickType::MOUSE_LEFT);
}

void APortalLobbyCharacterPlayer::FireRifle()
{
	UE_LOG(LogTemp, Warning, TEXT("Client Fire!"));

	if (CurrentWeapon == nullptr) return;

	if (HeldComponent != nullptr) return;

	if (CurrentWeapon->Fire())
	{
		// 서버 호출
		ServerFireRifle();
	}
	else
	{
		// Fire 실패 처리 (쿨타임, 탄약 부족 등)
		UE_LOG(LogTemp, Warning, TEXT("Fire failed: cooldown or other reason"));
	}
}

void APortalLobbyCharacterPlayer::ServerFireRightRifle_Implementation()
{
	HandleWeaponFire(EMouseClickType::MOUSE_RIGHT);
}

void APortalLobbyCharacterPlayer::FireRightRifle()
{
	UE_LOG(LogTemp, Warning, TEXT("Fire Right Rifle"));

	if (HeldComponent != nullptr) return;
	
	if (CurrentWeapon && CurrentWeapon->WeaponType == EWeaponType::PORTAL_GUN)
	{
		ServerFireRightRifle(); 
	}
}

void APortalLobbyCharacterPlayer::MulticastPlayImpactEffect_Implementation(FVector ImpactPoint, FVector ImpactNormal)
{
	if (CurrentWeapon)
	{
		CurrentWeapon->PlayImpactEffect(ImpactPoint, ImpactNormal);
	}
}

void APortalLobbyCharacterPlayer::MulticastFireRifle_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Multicast Fire!")); 

	if (IsLocallyControlled())
	{
		if (FireMontage_FirstPerson && FirstPersonMesh && FirstPersonMesh->GetAnimInstance())
		{
			FirstPersonMesh->GetAnimInstance()->Montage_Play(FireMontage_FirstPerson);
		}
	}
	else
	{
		if (FireMontage_ThirdPerson && ThirdPersonMesh && ThirdPersonMesh->GetAnimInstance())
		{
			ThirdPersonMesh->GetAnimInstance()->Montage_Play(FireMontage_ThirdPerson);
		}
	}
}

// IK노드나 애니메이션 시스템은 본 기준의 상대 위치를 사용해 IK제어를 한다.
FTransform APortalLobbyCharacterPlayer::GetLeftHandIKTransform()
{
	if (!CurrentWeapon)
	{
		//UE_LOG(LogTemp, Warning, TEXT("CurrentWeapon is null"));
		return FTransform::Identity;
	}

	if (!ThirdPersonMesh)
	{
		//UE_LOG(LogTemp, Warning, TEXT("FirstPersonMesh is null"));
		return FTransform::Identity;
	}

	// **이제 CurrentWeapon이 null이 아님이 확정됐으니 여기서 호출**
	if (!CurrentWeapon->DoesSocketExist(FName("LHIK")))
	{
		//UE_LOG(LogTemp, Warning, TEXT("Socket 'LHIK' does not exist in CurrentWeapon"));
		return FTransform::Identity;
	}

	FTransform SocketTransform = CurrentWeapon->GetSocketTransform(FName("LHIK"));

	FVector OutPosition;
	FRotator OutRotator;

 
	ThirdPersonMesh->TransformToBoneSpace(
		FName("hand_r"),
		SocketTransform.GetLocation(),
		SocketTransform.GetRotation().Rotator(),
		OutPosition,
		OutRotator);
	  

	FTransform OutTransform;
	OutTransform.SetLocation(OutPosition);
	OutTransform.SetRotation(OutRotator.Quaternion());
	OutTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

	return OutTransform;
}

APortalActor* APortalLobbyCharacterPlayer::GetOrCreatePortalActor()
{
	if (!OwningPortal)
	{
		FActorSpawnParameters Params;
		Params.Owner = this;

		if (PortalActorClass)
		{
			// 그냥 적당한 고정 위치에 Spawn
			FVector PortalBaseLocation = FVector::ZeroVector;
			FRotator PortalBaseRotation = FRotator::ZeroRotator;

			OwningPortal = GetWorld()->SpawnActor<APortalActor>(
				PortalActorClass, 
				PortalBaseLocation,
				PortalBaseRotation,
				Params);
			
		}
	}


	return OwningPortal;
}

void APortalLobbyCharacterPlayer::Client_SetControlRotation_Implementation(const FRotator& NewControlRot)
{
	if (AController* CharacterController = GetController())
	{
		SetActorRotation(NewControlRot);
		CharacterController->SetControlRotation(NewControlRot);

	}
}

 