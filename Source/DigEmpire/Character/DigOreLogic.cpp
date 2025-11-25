#include "DigOreLogic.h"

#include "DwarfLightComponent.h"

void UDigOreLogic_AddLight::Execute_Implementation(AActor* InstigatorActor, UMapGrid2DComponent* /*Map*/, int32 /*X*/, int32 /*Y*/, FGameplayTag /*OreTag*/)
{
    if (!InstigatorActor) return;
    if (UDwarfLightComponent* Light = InstigatorActor->FindComponentByClass<UDwarfLightComponent>())
    {
        Light->AddLightPower(Amount);
    }
}
