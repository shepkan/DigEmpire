#include "DigEmpire/Inventory/DEInventoryComponent.h"

UDEInventoryComponent::UDEInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

int32 UDEInventoryComponent::GetResourceAmount(const FGameplayTag& ResourceTag) const
{
    if (const int32* Found = Resources.Find(ResourceTag))
    {
        return *Found;
    }
    return 0;
}

bool UDEInventoryComponent::SetResourceAmount(const FGameplayTag& ResourceTag, int32 NewAmount)
{
    const int32 Clamped = FMath::Max(0, NewAmount);
    const int32* Existing = Resources.Find(ResourceTag);
    if (Existing && *Existing == Clamped)
    {
        return false;
    }
    Resources.Add(ResourceTag, Clamped);
    return true;
}

int32 UDEInventoryComponent::AddResource(const FGameplayTag& ResourceTag, int32 Amount)
{
    if (Amount <= 0)
    {
        return GetResourceAmount(ResourceTag);
    }
    const int32 NewValue = GetResourceAmount(ResourceTag) + Amount;
    Resources.Add(ResourceTag, NewValue);
    return NewValue;
}

int32 UDEInventoryComponent::ConsumeResource(const FGameplayTag& ResourceTag, int32 Amount)
{
    if (Amount <= 0)
    {
        return GetResourceAmount(ResourceTag);
    }

    const int32 Current = GetResourceAmount(ResourceTag);
    const int32 NewValue = FMath::Max(0, Current - Amount);
    Resources.Add(ResourceTag, NewValue);
    return NewValue;
}

