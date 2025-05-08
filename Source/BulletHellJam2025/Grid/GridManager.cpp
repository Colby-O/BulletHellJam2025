#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Grid/Tile.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	if (!DisableMap) GenerateGrid();
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!DisableMap) UpdateTileVisibility();
}

void AGridManager::UpdateTileVisibility()
{
	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!player) return;

	FVector playerLocation = player->GetActorLocation();

	for (const TPair<FVector2Int, AActor*>& pair : Tiles)
	{
		AActor* tile = pair.Value;
		if (!tile) continue;

		float distance = FVector::Dist(tile->GetActorLocation(), playerLocation);
		bool shouldBeActive = distance <= ViewDist;

		tile->SetActorHiddenInGame(!shouldBeActive);
		tile->SetActorEnableCollision(shouldBeActive);
		//tile->SetActorTickEnabled(shouldBeActive);
	}
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
}

FVector2Int AGridManager::WorldToGrid(FVector worldPt) const
{
	return FVector2Int(FMath::RoundToInt(worldPt.X / TileSize), FMath::RoundToInt(worldPt.Y / TileSize));
}

FVector AGridManager::GridToWorld(FVector2Int gridPt) const
{
	return FVector(gridPt.X * TileSize, gridPt.Y * TileSize, 0);
}

ATile* AGridManager::GetTileAt(const FVector2Int& pt) const
{
	return Tiles.Contains(pt) ? Tiles[pt] : nullptr;
}

void AGridManager::GenerateGrid()
{
	if (!TilePrefab|| MapRadius <= 0.f || TileSize <= 0.f) return;

	UWorld* world = GetWorld();
	if (!world) return;

	float halfTile = TileSize * 0.5f;

	int tilesPerAxis = FMath::CeilToInt((2 * MapRadius) / TileSize);
	float edgeThreshold = TileSize;

	for (int32 x = -tilesPerAxis / 2; x <= tilesPerAxis / 2; ++x)
	{
		for (int32 y = -tilesPerAxis / 2; y <= tilesPerAxis / 2; ++y)
		{
			float tileCenterX = x * TileSize + halfTile;
			float tileCenterY = y * TileSize + halfTile;

			float Distance = FMath::Sqrt(tileCenterX * tileCenterX + tileCenterY * tileCenterY);

			if (Distance <= BossRadius) 
			{
				FVector spawnLocation = MapCenter + FVector(tileCenterX, tileCenterY, 0.f);
				FRotator spawnRotation = FRotator::ZeroRotator;

				world->SpawnActor<AActor>(BossStandPrefab, spawnLocation, spawnRotation);
			}
			else if (Distance <= MapRadius)
			{
				bool isEdge = (MapRadius - Distance) <= edgeThreshold;

				TSubclassOf<AActor> tileClassToUse = isEdge ? EdgePrefab : TilePrefab;

				FVector spawnLocation = MapCenter + FVector(tileCenterX, tileCenterY, 0.f);
				FRotator spawnRotation = FRotator::ZeroRotator;

				world->SpawnActor<AActor>(tileClassToUse, spawnLocation, spawnRotation);
			}
		}
	}
}

void AGridManager::ResetGrid()
{
	for (TPair<FVector2Int, ATile*>& pair : Tiles)
	{
		pair.Value->ForceStopFall();
	}
}

float AGridManager::GetHeuristic(const FVector2Int& A, const FVector2Int& B)
{
	TArray<FVector2Int> directions = { FVector2Int(1, 0), FVector2Int(-1, 0) , FVector2Int(0, 1), FVector2Int(0, -1) };

	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y) + (NearTileWithState(&ATile::IsFalling, A) ? 1000 : 0);
}

bool AGridManager::NearTileWithState(bool ATile::* State, FVector2Int Loc)
{
	TArray<FVector2Int> directions = { FVector2Int(1, 0), FVector2Int(-1, 0) , FVector2Int(0, 1), FVector2Int(0, -1) };

	if (!GetTileAt(Loc)) return true;

	bool state = GetTileAt(Loc)->*State;

	for (FVector2Int dir : directions)
	{
		if (ATile* tile = GetTileAt(Loc + dir)) state |= tile->*State;
	}

	return state;
}

ATile* AGridManager::GetNearestSafeTile(FVector2Int Start, FVector2Int& SafeLocation, float MinDist)
{
	TArray<FVector2Int> directions = { FVector2Int(1, 0), FVector2Int(-1, 0) , FVector2Int(0, 1), FVector2Int(0, -1) };

	TQueue<FVector2Int> queue;

	queue.Enqueue(Start);

	while (!queue.IsEmpty()) 
	{
		queue.Dequeue(SafeLocation);

		for (const FVector2Int& dir : directions) 
		{
			if (GetTileAt(SafeLocation + dir)) queue.Enqueue(SafeLocation + dir);
		}

		ATile* curTile = GetTileAt(SafeLocation);
		if (curTile && curTile->IsEnable && !NearTileWithState(&ATile::IsFalling, SafeLocation) && SafeLocation.Dist(Start) >= MinDist) return curTile;
	}

	SafeLocation = FVector2Int(0, 0);
	return nullptr;
}

TArray<ATile*> AGridManager::FindPath(const FVector2Int& Start, const FVector2Int& Goal, float MaxDist, TArray<FVector2Int> TilesToAvoid)
{
	TArray<FVector2Int> directions = { FVector2Int(1, 0), FVector2Int(-1, 0) , FVector2Int(0, 1), FVector2Int(0, -1) };

	TMap<FVector2Int, FNode*> open;
	TSet<FVector2Int> closed;

	FNode* startNode = new FNode(Start, nullptr, GetHeuristic(Start, Goal), 0);
	open.Add(Start, startNode);

	while (open.Num() > 0) 
	{
		FNode* cur = nullptr;

		for (auto& pair : open) 
		{
			if (!cur || pair.Value->Cost < cur->Cost) cur = pair.Value;
		}

		if (!cur) break;

		if (cur->Point.Dist(Goal) <= MaxDist)
		{
			TArray<ATile*> path;
			for (FNode* node = cur; node != nullptr; node = node->Parent)
			{
				if (ATile* tile = GetTileAt(node->Point))
				{
					path.Add(tile);
				}
			}

			while (path.Num() > 0 && path[0]->IsFalling) path.RemoveAt(0);
			
			Algo::Reverse(path);

			for (auto& pair : open) delete pair.Value;

			return path;
		}

		open.Remove(cur->Point);
		closed.Add(cur->Point);

		for (const FVector2Int& dir : directions) 
		{
			FVector2Int neighbor = cur->Point + dir;
			if (!Tiles.Contains(neighbor) || closed.Contains(neighbor) || !GetTileAt(neighbor)->IsEnable || NearTileWithState(&ATile::HasFallen, neighbor) || TilesToAvoid.Contains(neighbor)) continue;

			float dst = cur->Dst + 1;
			FNode** existing = open.Find(neighbor);

			if (!existing || dst < (*existing)->Dst) 
			{
				FNode* neighborNode = new FNode(neighbor, cur, dst + GetHeuristic(neighbor, Goal), dst);
				open.Add(neighbor, neighborNode);
			}
		}
	}

	for (auto& pair : open) delete pair.Value;

	return {};
}

