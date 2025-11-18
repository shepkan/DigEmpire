// MapGrid2DComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "MapGrid2DComponent.generated.h"

class UMapGrid2D;
class ACellActor;
class UMapGenerationStepDataBase;

USTRUCT(BlueprintType)
struct FZoneInfo
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Zone")
    int32 Depth = -1;
};

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

    /** Ordered list of generation steps to execute. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="MapGrid|Generation")
    TArray<TObjectPtr<UMapGenerationStepDataBase>> GenerationSteps;

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

    UFUNCTION(BlueprintCallable, Category="MapGrid|Access")
    bool SetActorAt(int32 X, int32 Y, ACellActor* InActor);

    UFUNCTION(BlueprintPure, Category="MapGrid|Access")
    ACellActor* GetActorAt(int32 X, int32 Y) const;

    UFUNCTION(BlueprintPure, Category="MapGrid|Access")
    bool GetCell(int32 X, int32 Y, struct FMapCell& OutCell) const;

	/** Builds or rebuilds the map using current settings and broadcasts the bus event. */
	UFUNCTION(BlueprintCallable, Category="MapGrid|Init")
	void InitializeAndBuild();

	/** Returns the underlying map object (can be null). */
    UFUNCTION(BlueprintPure, Category="MapGrid|Access")
    UMapGrid2D* GetMap() const { return MapInstance; }

    /** Per-zone computed info (e.g., depth from Zone 0). Index = ZoneId. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="MapGrid|Zones")
    TArray<FZoneInfo> ZoneInfos;

    UFUNCTION(BlueprintPure, Category="MapGrid|Zones")
    int32 GetZoneDepth(int32 ZoneId) const { return ZoneInfos.IsValidIndex(ZoneId) ? ZoneInfos[ZoneId].Depth : -1; }

    void SetZoneDepths(const TArray<int32>& Depths);

protected:
	virtual void BeginPlay() override;

private:
	/** Owned map object. */
	UPROPERTY(Transient)
	TObjectPtr<UMapGrid2D> MapInstance = nullptr;

	void FillBackground();
	void BroadcastMapReady();  // <-- Event Bus publisher
};
