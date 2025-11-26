#pragma once


#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DigEmpire/Inventory/DEInventoryComponent.h"
#include "DigEmpirePlayerController.generated.h"

UCLASS()
class ADigEmpirePlayerController : public APlayerController
{
    GENERATED_BODY()
public:
    ADigEmpirePlayerController();

    /** Inventory component attached to this controller. */
    UFUNCTION(BlueprintPure, Category="Inventory")
    class UDEInventoryComponent* GetInventory() const { return Inventory; }

private:
    /** Holds resource amounts (GameplayTag -> Count). */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
    class UDEInventoryComponent* Inventory;
};
