// MapGrid2DComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MapGrid2DComponent.generated.h"

class UMapGrid2D;
class UZoneGenSettings;
class UZoneBorderSettings;

/**
 * Component that owns a UMapGrid2D instance.
 * It initializes the map, fills the background, builds a border,
 * and announces readiness via the Gameplay Message Subsystem (Event Bus).
 */
UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class  UMapGrid2DComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMapGrid2DComponent();

	/** If true, the component will create and initialize the map on BeginPlay. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init")
	bool bInitializeOnBeginPlay = true;

	/** Map width (in cells). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init", meta=(ClampMin="1"))
	int32 MapSizeX = 64;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init", meta=(ClampMin="1"));
	TObjectPtr<UZoneGenSettings> ZoneSettings = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init", meta=(ClampMin="1"));
	TObjectPtr<UZoneBorderSettings> BorderSettings = nullptr;

	/** Map height (in cells). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init", meta=(ClampMin="1"))
	int32 MapSizeY = 64;

	/** Default background tag for all cells. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init")
	FGameplayTag DefaultBackgroundTag;

	/** Object tag for map borders. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init")
	FGameplayTag BorderObjectTag;

	/** Durability assigned to each border object. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init", meta=(ClampMin="1"))
	int32 BorderObjectDurability = 100;

	/** Number of border layers around the map edges. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Init", meta=(ClampMin="0"))
	int32 BorderThickness = 1;

	/** Event Bus channel to publish when the map is ready (e.g. "Gameplay.Map.Ready"). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Events")
	FGameplayTag MapReadyChannel;

	/** Returns true if the map exists and has valid size. */
	UFUNCTION(BlueprintPure, Category="MapGrid|State")
	bool IsMapReady() const;

	/** Returns current map size (0,0 if map doesn't exist). */
	UFUNCTION(BlueprintPure, Category="MapGrid|Access")
	FIntPoint GetSize() const;

	UFUNCTION(BlueprintPure, Category="MapGrid|Access")
	bool IsInBounds(int32 X, int32 Y) const;

	UFUNCTION(BlueprintPure, Category="MapGrid|Access")
	bool GetBackgroundAt(int32 X, int32 Y, FGameplayTag& OutBackgroundTag) const;

	UFUNCTION(BlueprintPure, Category="MapGrid|Access")
	bool GetObjectAt(int32 X, int32 Y, FGameplayTag& OutObjectTag, int32& OutDurability) const;

	UFUNCTION(BlueprintPure, Category="MapGrid|Access")
	bool GetCell(int32 X, int32 Y, struct FMapCell& OutCell) const;

	/** Builds or rebuilds the map using current settings and broadcasts the bus event. */
	UFUNCTION(BlueprintCallable, Category="MapGrid|Init")
	void InitializeAndBuild();

	/** Returns the underlying map object (can be null). */
	UFUNCTION(BlueprintPure, Category="MapGrid|Access")
	UMapGrid2D* GetMap() const { return MapInstance; }

protected:
	virtual void BeginPlay() override;

private:
	/** Owned map object. */
	UPROPERTY(Transient)
	TObjectPtr<UMapGrid2D> MapInstance = nullptr;

	void FillBackground();
	void BuildBorder();
	void BroadcastMapReady();  // <-- Event Bus publisher
};
