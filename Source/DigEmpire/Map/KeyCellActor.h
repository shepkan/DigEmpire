#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CellActor.h"
#include "KeyCellActor.generated.h"

/**
 * Key actor placed on a map cell. When used, it destroys itself and
 * opens all doors with the same DoorColor tag (by calling OpenDoor()).
 */
UCLASS(Blueprintable)
class AKeyCellActor : public ACellActor
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Key")
    FGameplayTag DoorColor;

    /** Call from Blueprint to consume the key and open matching doors. */
    UFUNCTION(BlueprintCallable, Category="Key")
    void UseKey();

    /** Set the key color tag and notify BP. */
    UFUNCTION(BlueprintCallable, Category="Key")
    void SetDoorColor(const FGameplayTag& InColor);

    /** Fired when DoorColor is assigned (via SetDoorColor). */
    UFUNCTION(BlueprintNativeEvent, Category="Key|Events")
    void OnDoorColorAssigned(const FGameplayTag& InColor);
    virtual void OnDoorColorAssigned_Implementation(const FGameplayTag& /*InColor*/) {}
};
