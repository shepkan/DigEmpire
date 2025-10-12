#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "MapGrid2D.generated.h"

/** Данные одной клетки карты */
USTRUCT(BlueprintType)
struct FMapCell
{
	GENERATED_BODY()

	/** Геймплей-тэг бэкграунда клетки (земля, вода, дорога и т. п.) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
	FGameplayTag BackgroundTag;

	/** Геймплей-тэг объекта в клетке (юнит, дерево, сундук и т. п.). Empty = нет объекта */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
	FGameplayTag ObjectTag;

	/** Прочность/здоровье объекта. <=0 трактуем как отсутствующий объект */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cell")
	int32 ObjectDurability = 0;

	/** Быстрый предикат: в клетке есть объект? */
	bool HasObject() const
	{
		return ObjectDurability > 0 && ObjectTag.IsValid();
	}
};

/**
 * Объект-контейнер 2D-карты.
 * Хранит сетку X*Y клеток с бэкграундом и объектом.
 */
UCLASS(BlueprintType)
class UMapGrid2D : public UObject
{
	GENERATED_BODY()

public:
	/** Создать/переинициализировать карту указанного размера (очищает содержимое) */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	void Initialize(int32 InSizeX, int32 InSizeY);

	/** Размеры карты */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	FIntPoint GetSize() const { return FIntPoint(SizeX, SizeY); }

	/** Установить бэкграунд клетки */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	bool SetBackgroundAt(int32 X, int32 Y, const FGameplayTag& BackgroundTag);

	/** Добавить/обновить объект в клетке (устанавливает тэг и прочность) */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	bool AddOrUpdateObjectAt(int32 X, int32 Y, const FGameplayTag& ObjectTag, int32 Durability);

	/** Удалить объект из клетки (обнуляет тэг и прочность) */
	UFUNCTION(BlueprintCallable, Category="MapGrid")
	bool RemoveObjectAt(int32 X, int32 Y);

	/** Геттер бэкграунд-тэга (false если координаты вне карты) */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	bool GetBackgroundAt(int32 X, int32 Y, FGameplayTag& OutBackgroundTag) const;

	/** Геттер тэга объекта и прочности (false если нет объекта или координаты вне карты) */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	bool GetObjectAt(int32 X, int32 Y, FGameplayTag& OutObjectTag, int32& OutDurability) const;

	/** Быстрый доступ к целой клетке (false если вне границ) */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	bool GetCell(int32 X, int32 Y, FMapCell& OutCell) const;

	/** Проверка координат в пределах карты */
	UFUNCTION(BlueprintPure, Category="MapGrid")
	bool IsInBounds(int32 X, int32 Y) const
	{
		return X >= 0 && Y >= 0 && X < SizeX && Y < SizeY;
	}

private:
	/** Ширина и высота (в клетках) */
	UPROPERTY(VisibleAnywhere, Category="MapGrid")
	int32 SizeX = 0;

	UPROPERTY(VisibleAnywhere, Category="MapGrid")
	int32 SizeY = 0;

	/** Плоское хранение клеток: индекс = X + Y*SizeX */
	UPROPERTY()
	TArray<FMapCell> Cells;

	int32 Index(int32 X, int32 Y) const { return X + Y * SizeX; }
};
