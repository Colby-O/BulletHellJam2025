#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Grid/Tile.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGridManager::RegisterTile(ATile* Tile)
{
	FVector worldPT = Tile->GetActorLocation();
	FVector2Int gridPT = WorldToGrid(worldPT);
	if (!Tiles.Contains(gridPT)) 
	{
		Tiles.Add(gridPT, Tile);
	}
	Tiles[gridPT] = Tile;

	UE_LOG(LogTemp, Warning, TEXT("World Pos: %s Grid Pos: %s Tile Name: %s"), *Tile->GetActorLocation().ToString(), *gridPT.ToString(), *Tile->GetActorLabel());
}

FVector2Int AGridManager::WorldToGrid(FVector worldPt) const
{
	FVector clamped = FVector(FMath::RoundToInt(worldPt.X) / TileSize, FMath::RoundToInt(worldPt.Y) / TileSize, FMath::RoundToInt(worldPt.Z) / TileSize);
	return FVector2Int(FMath::FloorToInt(clamped.X), FMath::FloorToInt(clamped.Y));
}

FVector AGridManager::GridToWorld(FVector2Int gridPt) const
{
	return FVector(gridPt.X * TileSize, gridPt.Y * TileSize, 0);
}

ATile* AGridManager::GetTileAt(FVector2Int pt) const
{
	return Tiles.Contains(pt) ? Tiles[pt] : nullptr;
}

