#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "DEInventoryComponent.generated.h"

/**
 * Simple inventory for resources: GameplayTag -> Amount (>= 0)
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UDEInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UDEInventoryComponent();

    /** Get amount of the resource. Returns 0 if tag not present. */
    UFUNCTION(BlueprintPure, Category="Inventory")
    int32 GetResourceAmount(const FGameplayTag& ResourceTag) const;

    /** Set exact amount (clamped to >= 0). Returns true if value changed. */
    UFUNCTION(BlueprintCallable, Category="Inventory")
    bool SetResourceAmount(const FGameplayTag& ResourceTag, int32 NewAmount);

    /** Add amount (ignored if Amount <= 0). Returns new amount. */
    UFUNCTION(BlueprintCallable, Category="Inventory")
    int32 AddResource(const FGameplayTag& ResourceTag, int32 Amount);

    /** Consume amount (ignored if Amount <= 0). Returns new amount after clamping to >= 0. */
    UFUNCTION(BlueprintCallable, Category="Inventory")
    int32 ConsumeResource(const FGameplayTag& ResourceTag, int32 Amount);

    /** Get a copy of all resources. */
    UFUNCTION(BlueprintPure, Category="Inventory")
    TMap<FGameplayTag, int32> GetAllResources() const { return Resources; }

protected:
    /** Designers can seed defaults per controller in BP if needed. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
    TMap<FGameplayTag, int32> Resources;
};

