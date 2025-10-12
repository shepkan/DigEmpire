#include "MapGrid2D.h"

void UMapGrid2D::Initialize(int32 InSizeX, int32 InSizeY)
{
    SizeX = FMath::Max(0, InSizeX);
    SizeY = FMath::Max(0, InSizeY);

    const int32 Count = SizeX * SizeY;
    Cells.SetNum(Count);

    // Reset content
    for (FMapCell& Cell : Cells)
    {
        Cell.BackgroundTag = FGameplayTag(); // empty
        Cell.ObjectTag = FGameplayTag();     // empty
        Cell.ObjectDurability = 0;
        // ZoneId defaults from struct initializer
    }
}

bool UMapGrid2D::SetBackgroundAt(int32 X, int32 Y, const FGameplayTag& BackgroundTag)
{
    if (!IsInBounds(X, Y)) return false;
    Cells[Index(X, Y)].BackgroundTag = BackgroundTag;
    return true;
}

bool UMapGrid2D::AddOrUpdateObjectAt(int32 X, int32 Y, const FGameplayTag& ObjectTag, int32 Durability)
{
    if (!IsInBounds(X, Y)) return false;

    FMapCell& Cell = Cells[Index(X, Y)];

    if (!ObjectTag.IsValid() || Durability <= 0)
    {
        // Treat invalid args as deletion
        Cell.ObjectTag = FGameplayTag();
        Cell.ObjectDurability = 0;
        return true;
    }

    Cell.ObjectTag = ObjectTag;
    Cell.ObjectDurability = Durability;
    return true;
}

bool UMapGrid2D::RemoveObjectAt(int32 X, int32 Y)
{
    if (!IsInBounds(X, Y)) return false;
    FMapCell& Cell = Cells[Index(X, Y)];
    Cell.ObjectTag = FGameplayTag();
    Cell.ObjectDurability = 0;
    return true;
}

bool UMapGrid2D::GetBackgroundAt(int32 X, int32 Y, FGameplayTag& OutBackgroundTag) const
{
    if (!IsInBounds(X, Y)) return false;
    OutBackgroundTag = Cells[Index(X, Y)].BackgroundTag;
    return true;
}

bool UMapGrid2D::GetObjectAt(int32 X, int32 Y, FGameplayTag& OutObjectTag, int32& OutDurability) const
{
    if (!IsInBounds(X, Y)) return false;

    const FMapCell& Cell = Cells[Index(X, Y)];
    if (!Cell.HasObject())
    {
        return false; // no object present
    }

    OutObjectTag = Cell.ObjectTag;
    OutDurability = Cell.ObjectDurability;
    return true;
}

bool UMapGrid2D::GetCell(int32 X, int32 Y, FMapCell& OutCell) const
{
    if (!IsInBounds(X, Y)) return false;
    OutCell = Cells[Index(X, Y)];
    return true;
}

bool UMapGrid2D::SetZoneAt(int32 X, int32 Y, int32 InZoneId)
{
    if (!IsInBounds(X, Y)) return false;
    Cells[Index(X, Y)].ZoneId = InZoneId;
    return true;
}

bool UMapGrid2D::ApplyZoneLabels(const TArray<int32>& Labels)
{
    const int32 N = SizeX * SizeY;
    if (Labels.Num() != N) return false;
    for (int32 i = 0; i < N; ++i)
    {
        Cells[i].ZoneId = Labels[i];
    }
    return true;
}

int32 UMapGrid2D::GetZoneAt(int32 X, int32 Y) const
{
    if (!IsInBounds(X, Y)) return -1;
    return Cells[Index(X, Y)].ZoneId;
}

TArray<FIntPoint> UMapGrid2D::GetCellsForZone(int32 InZoneId) const
{
    TArray<FIntPoint> Result;
    Result.Reserve(SizeX * SizeY / 4);
    for (int32 y = 0; y < SizeY; ++y)
    {
        for (int32 x = 0; x < SizeX; ++x)
        {
            const int32 id = Index(x, y);
            if (Cells[id].ZoneId == InZoneId)
            {
                Result.Add(FIntPoint(x, y));
            }
        }
    }
    return Result;
}

