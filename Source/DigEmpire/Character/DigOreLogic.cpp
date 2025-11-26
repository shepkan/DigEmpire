#include "DigOreLogic.h"

#include "DwarfLightComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "DigEmpire/Inventory/DEInventoryComponent.h"

void UDigOreLogic_AddLight::Execute_Implementation(AActor* InstigatorActor, UMapGrid2DComponent* /*Map*/, int32 /*X*/, int32 /*Y*/, FGameplayTag /*OreTag*/)
{
    if (!InstigatorActor) return;
    if (UDwarfLightComponent* Light = InstigatorActor->FindComponentByClass<UDwarfLightComponent>())
    {
        Light->AddLightPower(Amount);
    }
}

void UDigOreLogic_AddResource::Execute_Implementation(AActor* InstigatorActor, UMapGrid2DComponent* /*Map*/, int32 /*X*/, int32 /*Y*/, FGameplayTag OreTag)
{
    if (!InstigatorActor) return;

    // Determine controller from instigator
    AController* Controller = nullptr;
    if (APawn* Pawn = Cast<APawn>(InstigatorActor))
    {
        Controller = Pawn->GetController();
    }
    else if (AController* AsController = Cast<AController>(InstigatorActor))
    {
        Controller = AsController;
    }

    APlayerController* PC = Controller ? Cast<APlayerController>(Controller) : nullptr;
    if (!PC) return;

    if (UDEInventoryComponent* Inv = PC->FindComponentByClass<UDEInventoryComponent>())
    {
        const FGameplayTag TargetTag = ResourceTag.IsValid() ? ResourceTag : OreTag;
        if (TargetTag.IsValid() && Amount > 0)
        {
            Inv->AddResource(TargetTag, Amount);
        }
    }
}
