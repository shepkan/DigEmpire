// MapGrid2DMessages.h
#pragma once

#include "CoreMinimal.h"
#include "MapGrid2DMessages.generated.h"

class UMapGrid2DComponent;

/** Payload sent when the map is created and ready. */
USTRUCT(BlueprintType)
struct FMapReadyMessage
{
	GENERATED_BODY()

	/** Component that owns the map. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMapGrid2DComponent> Source = nullptr;

	/** Final map size. */
	UPROPERTY(BlueprintReadOnly)
	FIntPoint Size = FIntPoint::ZeroValue;
};
