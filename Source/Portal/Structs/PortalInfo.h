#pragma once

#include "CoreMinimal.h"
#include "PortalInfo.generated.h"

USTRUCT(BlueprintType)
struct FPortalInfo
{
    GENERATED_BODY()

    /** Æ÷Å» ·çÆ® */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    USceneComponent* PortalRoot;

    /** Æ÷Å» ¹® */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    UStaticMeshComponent* Door;

    /** Ä«¸Þ¶ó ·çÆ® */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    USceneComponent* CamRoot;

    /** Æ÷Å» ºä Capture */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
    USceneCaptureComponent2D* Capture;

    /** ±âº» »ý¼ºÀÚ */
    FPortalInfo()
        : PortalRoot(nullptr), Door(nullptr), CamRoot(nullptr), Capture(nullptr)
    {
    }
};