#include "MapSpriteRenderer.h"

#include "EngineUtils.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "TileTextureSet.h"
#include "MapGrid2DComponent.h"
#include "MapGrid2D.h"

#include "DigEmpire/BusEvents/MapGrid2DMessages.h"

AMapSpriteRenderer::AMapSpriteRenderer()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root scene to attach HISM components under it
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
}

void AMapSpriteRenderer::BeginPlay()
{
	Super::BeginPlay();

	// Try to auto-pick a map component if none assigned
	TryAutoFindMapComponent();

	// Subscribe to map-ready messages + do one-shot check if already ready
	SetupMapReadySubscription();

	// If map already ready at BeginPlay, attempt immediate rebuild
	RebuildNow();
}

void AMapSpriteRenderer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister listener if it was registered
	if (MapReadyHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(MapReadyHandle);
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

void AMapSpriteRenderer::SetupMapReadySubscription()
{
	if (!MapReadyChannel.IsValid())
	{
		// No channel configured — just rely on direct readiness checks.
		return;
	}

	UGameplayMessageSubsystem& Bus = UGameplayMessageSubsystem::Get(this);
	
	MapReadyHandle = Bus.RegisterListener<FMapReadyMessage>(
		MapReadyChannel,
		[this](FGameplayTag SomeTag,const FMapReadyMessage& Msg)
		{
			OnMapReadyMessage(Msg);
		});
}

void AMapSpriteRenderer::OnMapReadyMessage(const FMapReadyMessage& Msg)
{
	// Accept only messages with a valid source; if our MapSource is unset, adopt it.
	if (Msg.Source)
	{
		if (!MapSource)
		{
			MapSource = Msg.Source;
		}
		// If message pertains to our current source, rebuild
		if (MapSource == Msg.Source)
		{
			RebuildNow();
		}
	}
}

void AMapSpriteRenderer::RebuildNow()
{
	// If map exists and is ready, rebuild. Otherwise do nothing.
	if (MapSource && MapSource->IsMapReady())
	{
		RebuildInternal();
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

void AMapSpriteRenderer::RebuildInternal()
{
	if (!MapSource || !MapSource->IsMapReady() || !TextureSet)
	{
		return;
	}

	// Drop previous content
	ClearAll();

	const FIntPoint Size = MapSource->GetSize();

	// Iterate over all cells
	for (int32 Y = 0; Y < Size.Y; ++Y)
	{
		for (int32 X = 0; X < Size.X; ++X)
		{
			// Background
			FGameplayTag BgTag;
			if (MapSource->GetBackgroundAt(X, Y, BgTag))
			{
				if (UTexture2D* BgTex = TextureSet->FindBackgroundTexture(BgTag))
				{
					if (auto* HISM = GetOrCreateHISMForTexture(BgTex))
					{
						HISM->AddInstance(BuildInstanceTransform(X, Y, BackgroundLayer));
					}
				}
			}

			// Object
			FGameplayTag ObjTag; int32 Durability = 0;
			if (MapSource->GetObjectAt(X, Y, ObjTag, Durability))
			{
				if (UTexture2D* ObjTex = TextureSet->FindObjectTexture(ObjTag))
				{
					if (auto* HISM = GetOrCreateHISMForTexture(ObjTex))
					{
						HISM->AddInstance(BuildInstanceTransform(X, Y, ObjectLayer));
					}
				}
			}
		}
	}
}
