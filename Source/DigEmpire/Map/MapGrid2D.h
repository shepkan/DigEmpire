#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Generation/ZonePassageTypes.h" // FZonePassage
#include "Rooms/RoomTypes.h"                // FRoomInfo
#include "MapGrid2D.generated.h"

class ACellActor;

/** Single map cell data */
USTRUCT(BlueprintType)
struct FMapCell
{
    GENERATED_BODY()

	/** Gameplay tag for cell background (ground, water, road, etc.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
	FGameplayTag BackgroundTag;

	/** Gameplay tag for an object in the cell (unit, tree, chest, etc.). Empty = no object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
	FGameplayTag ObjectTag;

	/** Object durability/health. <=0 means object is absent */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
	int32 ObjectDurability = 0;

    /** Optional actor placed on this cell. If set, use it to determine blocking. */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cell")
    TObjectPtr<ACellActor> Occupant = nullptr;

	/** Quick predicate: does the cell have an object? */
    bool HasObject() const
    {
        return ObjectDurability > 0 && ObjectTag.IsValid();
    }

    /** Zone id this cell belongs to (>=0) or -1 if unset */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cell")
    int32 ZoneId = -1;

    /** Has this cell been seen (via vision)? */
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Cell")
    bool bVieved = false;

    /** Optional ore tag present in this cell (separate from background/object). Empty = no ore. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
    FGameplayTag OreTag;
};

/**
 * 2D map container object.
 * Stores an X*Y grid of cells with background and object data.
 */
UCLASS(BlueprintType)
class UMapGrid2D : public UObject
{
	GENERATED_BODY()

public:
	/** Create/Reinitialize the map with given size (clears content) */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	void Initialize(int32 InSizeX, int32 InSizeY);

	/** Map size */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	FIntPoint GetSize() const { return FIntPoint(SizeX, SizeY); }

	/** Set cell background */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	bool SetBackgroundAt(int32 X, int32 Y, const FGameplayTag& BackgroundTag);

	/** Add or update an object in a cell (tag and durability) */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	bool AddOrUpdateObjectAt(int32 X, int32 Y, const FGameplayTag& ObjectTag, int32 Durability);

	/** Remove object from a cell (clears tag and durability) */
    UFUNCTION(BlueprintCallable, Category="MapGrid")
    bool RemoveObjectAt(int32 X, int32 Y);

	/** Get background tag (false if out of bounds) */
    UFUNCTION(BlueprintPure, Category="MapGrid")
    bool GetBackgroundAt(int32 X, int32 Y, FGameplayTag& OutBackgroundTag) const;

	/** Get object tag and durability (false if no object or out of bounds) */
    UFUNCTION(BlueprintPure, Category="MapGrid")
    bool GetObjectAt(int32 X, int32 Y, FGameplayTag& OutObjectTag, int32& OutDurability) const;

    /** Set ore tag at a cell (empty tag clears ore). */
    UFUNCTION(BlueprintCallable, Category="MapGrid|Ore")
    bool SetOreAt(int32 X, int32 Y, const FGameplayTag& InOreTag);

    /** Get ore tag at a cell (false if out of bounds). */
    UFUNCTION(BlueprintPure, Category="MapGrid|Ore")
    bool GetOreAt(int32 X, int32 Y, FGameplayTag& OutOreTag) const;

    /** Set the actor occupant at a cell (nullptr to clear). */
    UFUNCTION(BlueprintCallable, Category="MapGrid")
    bool SetActorAt(int32 X, int32 Y, ACellActor* InActor);

    /** Get the actor occupant at a cell (nullptr if none or OOB). */
    UFUNCTION(BlueprintPure, Category="MapGrid")
    ACellActor* GetActorAt(int32 X, int32 Y) const;

    /** Fast access to a whole cell (false if out of bounds) */
    UFUNCTION(BlueprintPure, Category="MapGrid")
    bool GetCell(int32 X, int32 Y, FMapCell& OutCell) const;

    /** Mark or clear the viewed flag for a cell */
    UFUNCTION(BlueprintCallable, Category="MapGrid")
    bool SetViewedAt(int32 X, int32 Y, bool bViewed);

    // Zones API
    UFUNCTION(BlueprintCallable, Category="MapGrid|Zones")
    bool SetZoneAt(int32 X, int32 Y, int32 InZoneId);

    UFUNCTION(BlueprintCallable, Category="MapGrid|Zones")
    bool ApplyZoneLabels(const TArray<int32>& Labels);

    UFUNCTION(BlueprintPure, Category="MapGrid|Zones")
    int32 GetZoneAt(int32 X, int32 Y) const;

    UFUNCTION(BlueprintPure, Category="MapGrid|Zones")
    TArray<FIntPoint> GetCellsForZone(int32 InZoneId) const;

    // Passages API (C++)
    const TArray<FZonePassage>& GetPassages() const { return Passages; }
    void SetPassages(const TArray<FZonePassage>& InPassages) { Passages = InPassages; }

    // Rooms API
    UFUNCTION(BlueprintPure, Category="MapGrid|Rooms")
    const TArray<FRoomInfo>& GetRooms() const { return Rooms; }

    UFUNCTION(BlueprintPure, Category="MapGrid|Rooms")
    TArray<FRoomInfo> GetRoomsForZone(int32 InZoneId) const;

    UFUNCTION(BlueprintCallable, Category="MapGrid|Rooms")
    void AddRoom(const FRoomInfo& Info) { Rooms.Add(Info); }

	/** In-bounds check */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	bool IsInBounds(int32 X, int32 Y) const
	{
		return X >= 0 && Y >= 0 && X < SizeX && Y < SizeY;
	}

private:
	/** Map width and height (in cells) */
	UPROPERTY(VisibleAnywhere, Category="MapGrid")
	int32 SizeX = 0;

	UPROPERTY(VisibleAnywhere, Category="MapGrid")
	int32 SizeY = 0;

	/** Flat storage of cells: index = X + Y*SizeX */
    UPROPERTY()
    TArray<FMapCell> Cells;

    // Stored passages between zones
    TArray<FZonePassage> Passages;

    // Stored generated rooms
    UPROPERTY(Transient)
    TArray<FRoomInfo> Rooms;

	int32 Index(int32 X, int32 Y) const { return X + Y * SizeX; }
};
