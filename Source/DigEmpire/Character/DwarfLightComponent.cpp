#include "DwarfLightComponent.h"

#include "DwarfLightSettings.h"
#include "DigEmpire/Character/CharacterGridVisionComponent.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"

UDwarfLightComponent::UDwarfLightComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    bAutoActivate = true;
}

void UDwarfLightComponent::BeginPlay()
{
    Super::BeginPlay();

    // Init LightPower from settings (if present), clamp [0..100]
    if (Settings)
    {
        LightPower = FMath::Clamp(Settings->DefaultLightPower, 0.f, 100.f);
    }
    else
    {
        LightPower = 100.f;
    }

    // Cache owner's vision component if available
    if (AActor* Owner = GetOwner())
    {
        if (UCharacterGridVisionComponent* Vision = Owner->FindComponentByClass<UCharacterGridVisionComponent>())
        {
            CachedVision = Vision;
        }
    }

    ApplyVisionFromLight();
}

void UDwarfLightComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!bDecayEnabled) return;

    const float Decrease = (Settings ? Settings->DecreasePerSecond : 1.0f) * FMath::Max(0.f, DeltaTime);
    if (Decrease <= 0.f) return;

    const float OldPower = LightPower;
    LightPower = FMath::Clamp(LightPower - Decrease, 0.f, 100.f);

    if (!FMath::IsNearlyEqual(OldPower, LightPower))
    {
        ApplyVisionFromLight();
    }
}

int32 UDwarfLightComponent::ComputeVisionRadiusFor(float Power) const
{
    if (!Settings) return 0;
    if (const FDwarfLightVisibilityConfig* Cfg = Settings->FindBestConfig(Power))
    {
        return FMath::Max(0, Cfg->VisionRadius);
    }
    return 0;
}

void UDwarfLightComponent::ApplyVisionFromLight()
{
    const int32 TargetRadius = ComputeVisionRadiusFor(LightPower);

    UCharacterGridVisionComponent* Vision = CachedVision.Get();
    if (!Vision)
    {
        if (AActor* Owner = GetOwner())
        {
            Vision = Owner->FindComponentByClass<UCharacterGridVisionComponent>();
            if (Vision)
            {
                CachedVision = Vision;
            }
        }
    }

    // Only apply effects when threshold value (mapped radius) changes
    const bool bThresholdChanged = (LastAppliedVisionRadius != TargetRadius);
    if (bThresholdChanged)
    {
        // Update vision component if allowed
        if (Vision && !Vision->bVisionLockedByCheat && Vision->VisionRadiusCells != TargetRadius)
        {
            Vision->VisionRadiusCells = FMath::Max(0, TargetRadius);
            // Threshold crossed -> force immediate vision & luminance update
            Vision->ForceVisionUpdate();
        }

        // Update MPC if enabled in settings
        if (Settings && Settings->bWriteRadiusToMPC)
        {
            WriteRadiusToMPC(TargetRadius);
        }

        LastAppliedVisionRadius = TargetRadius;
    }
}

void UDwarfLightComponent::WriteRadiusToMPC(int32 Radius)
{
    if (!Settings)
    {
        return;
    }

    if (!Settings->VisionRadiusMPC || Settings->VisionRadiusParamName.IsNone())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    if (UMaterialParameterCollectionInstance* MPCInstance = World->GetParameterCollectionInstance(Settings->VisionRadiusMPC))
    {
        MPCInstance->SetScalarParameterValue(Settings->VisionRadiusParamName, static_cast<float>(FMath::Max(0, Radius)));
    }
}
