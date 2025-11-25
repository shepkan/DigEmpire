#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "DigOreLogicSettings.generated.h"

class UDigOreLogicBase;

/**
 * Settings: map Ore gameplay tag to an instanced logic object to execute
 * when a block with that ore is destroyed by UDigComponent.
 */
UCLASS(BlueprintType)
class UDigOreLogicSettings : public UDataAsset
{
    GENERATED_BODY()
public:
    /** Per-ore logic mapping (instanced subobjects for easy configuration). */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category="Dig|OreLogic")
    TMap<FGameplayTag, UDigOreLogicBase*> LogicByOreTag;

    /** Finds logic by exact ore tag key. Returns nullptr if not found. */
    UFUNCTION(BlueprintPure, Category="Dig|OreLogic")
    UDigOreLogicBase* FindLogic(const FGameplayTag& OreTag) const
    {
        if (!OreTag.IsValid()) return nullptr;
        if (const UDigOreLogicBase* const* Found = LogicByOreTag.Find(OreTag))
        {
            // const_cast is safe here: DataAsset owns the subobject; we don't modify map structure
            return const_cast<UDigOreLogicBase*>(*Found);
        }
        return nullptr;
    }
};

