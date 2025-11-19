#include "MapSpriteRenderer.h"

#include "EngineUtils.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "TileTextureSet.h"
#include "MapGrid2DComponent.h"
#include "MapGrid2D.h"

#include "DigEmpire/BusEvents/MapGrid2DMessages.h"
#include "DigEmpire/BusEvents/CharacterGridVisionMessages.h"
#include "DigEmpire/Tags/DENativeTags.h"

AMapSpriteRenderer::AMapSpriteRenderer()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root scene to attach HISM components under it
    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    // HISM components are Static; parent must be Static too
    Root->SetMobility(EComponentMobility::Static);

    // Default event channel for first-seen cells
    FirstSeenChannel = TAG_Character_Vision_FirstSeen;
}

void AMapSpriteRenderer::BeginPlay()
{
    Super::BeginPlay();
	
    FirstSeenChannel = TAG_Character_Vision_FirstSeen;

    // Try to auto-pick a map component if none assigned (optional for event-driven)
    TryAutoFindMapComponent();

    // Subscribe to first-seen cell messages
    SetupFirstSeenSubscription();
}

void AMapSpriteRenderer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // Unregister listener if it was registered
    if (FirstSeenHandle.IsValid())
    {
        UGameplayMessageSubsystem::Get(this).UnregisterListener(FirstSeenHandle);
    }
    Super::EndPlay(EndPlayReason);
}

void AMapSpriteRenderer::TryAutoFindMapComponent()
{
	if (MapSource) return;

	// Simple auto-find: first valid UMapGrid2DComponent in the world
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (UMapGrid2DComponent* Comp = It->FindComponentByClass<UMapGrid2DComponent>())
		{
			MapSource = Comp;
			break;
		}
	}
}

void AMapSpriteRenderer::SetupFirstSeenSubscription()
{
    if (!FirstSeenChannel.IsValid())
    {
        return;
    }

    UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
    FirstSeenHandle = Bus.RegisterListener<FCellsFirstSeenMessage>(
        FirstSeenChannel,
        [this](FGameplayTag, const FCellsFirstSeenMessage& Msg)
        {
            OnCellsFirstSeen(Msg);
        });
}

void AMapSpriteRenderer::OnCellsFirstSeen(const FCellsFirstSeenMessage& Msg)
{
    if (!TextureSet) return;

    // For each newly seen cell, render background and object layers
    for (const FGridCellWithCoord& Entry : Msg.Cells)
    {
        const int32 X = Entry.Coord.X;
        const int32 Y = Entry.Coord.Y;

        if (UTexture2D* BgTex = TextureSet->FindBackgroundTexture(Entry.Cell.BackgroundTag))
        {
            if (auto* HISM = GetOrCreateHISMForTexture(BgTex))
            {
                HISM->AddInstance(BuildInstanceTransform(X, Y, BackgroundLayer));
            }
        }

        if (Entry.Cell.HasObject())
        {
            if (UTexture2D* ObjTex = TextureSet->FindObjectTexture(Entry.Cell.ObjectTag))
            {
                if (auto* HISM = GetOrCreateHISMForTexture(ObjTex))
                {
                    HISM->AddInstance(BuildInstanceTransform(X, Y, ObjectLayer));
                }
            }
        }
    }
}

void AMapSpriteRenderer::ClearAll()
{
    for (auto& Pair : TextureToHISM)
    {
        if (Pair.Value)
		{
			Pair.Value->DestroyComponent();
		}
	}
	TextureToHISM.Empty();
}
UHierarchicalInstancedStaticMeshComponent* AMapSpriteRenderer::GetOrCreateHISMForTexture(UTexture2D* Texture)
{
	if (!Texture) return nullptr;
	if (!TilePlaneMesh || !TileBaseMaterial) return nullptr;

	if (TObjectPtr<UHierarchicalInstancedStaticMeshComponent>* Found = TextureToHISM.Find(Texture))
	{
		return *Found;
	}

	const FName CompName = *FString::Printf(TEXT("HISM_%s"), *GetNameSafe(Texture));
	auto* HISM = NewObject<UHierarchicalInstancedStaticMeshComponent>(this, CompName);
	HISM->SetupAttachment(GetRootComponent());
	HISM->SetStaticMesh(TilePlaneMesh);
	HISM->SetMobility(EComponentMobility::Static);
	HISM->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HISM->SetCastShadow(false);

	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(TileBaseMaterial, this);
	MID->SetTextureParameterValue(TextureParamName, Texture);
	HISM->SetMaterial(0, MID);

	HISM->RegisterComponent();
	TextureToHISM.Add(Texture, HISM);
	return HISM;
}

FTransform AMapSpriteRenderer::BuildInstanceTransform(int32 GridX, int32 GridY, int32 LayerIndex) const
{
	const float X = static_cast<float>(GridX) * TileSize;
	const float Y = static_cast<float>(GridY) * TileSize;
	const float Z = ZBaseOffset + (static_cast<float>(LayerIndex) * LayerStep);

	const float Scale = TileSize / 100.f; // default plane is 100x100
	FTransform T;
	T.SetLocation(FVector(X, Y, Z));
	T.SetRotation(FQuat::Identity);
	T.SetScale3D(FVector(Scale, Scale, 1.f));
	return T;
}

