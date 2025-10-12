#include "MapGrid2D.h"

void UMapGrid2D::Initialize(int32 InSizeX, int32 InSizeY)
{
	SizeX = FMath::Max(0, InSizeX);
	SizeY = FMath::Max(0, InSizeY);

	const int32 Count = SizeX * SizeY;
	Cells.SetNum(Count);

	// Обнулим содержимое
	for (FMapCell& Cell : Cells)
	{
		Cell.BackgroundTag = FGameplayTag(); // пусто
		Cell.ObjectTag = FGameplayTag();     // пусто
		Cell.ObjectDurability = 0;
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
		// Считаем, что некорректные аргументы — это удаление
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
		return false; // нет объекта
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
