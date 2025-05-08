#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Grid/Tile.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/Character.h"

AGridManager::AGridManager()
{
	PrimaryActorTick.bCanEverTick = true;

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Tiles"));
	InstancedMesh->SetupAttachment(RootComponent);
}

void AGridManager::BeginPlay()
{
	Super::BeginPlay();

	InstancedMesh->SetStaticMesh(BaseMesh);
	DynamicOutlineMaterial = UMaterialInstanceDynamic::Create(OutlineMat, this);
	InstancedMesh->SetMaterial(0, BaseMat);
	InstancedMesh->SetMaterial(1, DynamicOutlineMaterial);

	if (!DisableMap) GenerateGrid();
}

void AGridManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!DisableMap) UpdateTileVisibility();
}

void AGridManager::UpdateTileVisibility()
{
	//ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	//if (!player) return;

	//FVector playerLocation = player->GetActorLocation();

	//for (const TPair<FVector2Int, AActor*>& pair : Tiles)
	//{
	//	AActor* tile = pair.Value;
	//	if (!tile) continue;

	//	float distance = FVector::Dist(tile->GetActorLocation(), playerLocation);
	//	bool shouldBeActive = distance <= ViewDist;

	//	tile->SetActorHiddenInGame(!shouldBeActive);
	//	tile->SetActorEnableCollision(shouldBeActive);
	//	//tile->SetActorTickEnabled(shouldBeActive);
	//}
}

void AGridManager::RegisterTile(FCell Tile)
{
	FTransform worldTransform;
	InstancedMesh->GetInstanceTransform(Tile.ID, worldTransform, true);
	FVector2Int gridPT = WorldToGrid(worldTransform.GetLocation());
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

bool AGridManager::GetTileAt(const FVector2Int& pt, FCell& Cell)
{
	if (Tiles.Contains(pt)) Cell = Tiles[pt];
	return Tiles.Contains(pt);
}

bool AGridManager::GetTileAt(const FVector2Int& pt) const
{
	return Tiles.Contains(pt);
}

void AGridManager::GenerateGrid()
{
	if (!InstancedMesh || MapRadius <= 0.f || TileSize <= 0.f) return;

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

				SpawnTile(spawnLocation, spawnRotation, EdgeTileScale, false);
			}
			else if (Distance <= MapRadius)
			{
				bool isEdge = (MapRadius - Distance) <= edgeThreshold;

				FVector scale = isEdge ? EdgeTileScale: TileScale;

				FVector spawnLocation = MapCenter + FVector(tileCenterX, tileCenterY, 0.f);
				FRotator spawnRotation = FRotator::ZeroRotator;

				SpawnTile(spawnLocation, spawnRotation, scale, isEdge);
			}
		}
	}
}

void AGridManager::SpawnTile(FVector Location, FRotator Rotation, FVector Scale, bool IsEnabled) 
{
	FTransform transform;
	transform.SetLocation(Location);
	transform.SetRotation(Rotation.Quaternion());
	transform.SetScale3D(Scale);
	int instanceID = InstancedMesh->AddInstance(transform);
	FCell cell(instanceID, IsEnabled);
	RegisterTile(cell);
	SetTileColor(cell, BaseOutlineColor);
	UE_LOG(LogTemp, Warning, TEXT("Spawning Tile At: %s"), *transform.GetLocation().ToString());
}

void AGridManager::SetTileColorAt(FVector2Int Location, FLinearColor Color)
{
	FCell cell;
	if (GetTileAt(Location, cell)) SetTileColor(cell, Color);
}

void AGridManager::SetTileColor(FCell Cell, FLinearColor Color)
{
	InstancedMesh->SetCustomDataValue(Cell.ID, 0, Color.R, true);
	InstancedMesh->SetCustomDataValue(Cell.ID, 1, Color.G, true);
	InstancedMesh->SetCustomDataValue(Cell.ID, 2, Color.B, true);

}

float AGridManager::GetHeuristic(const FVector2Int& A, const FVector2Int& B)
{
	TArray<FVector2Int> directions = { FVector2Int(1, 0), FVector2Int(-1, 0) , FVector2Int(0, 1), FVector2Int(0, -1) };

	return FMath::Abs(A.X - B.X) + FMath::Abs(A.Y - B.Y) + (NearTileWithState(&FCell::IsFalling, A) ? 1000 : 0);
}

bool AGridManager::NearTileWithState(bool FCell::* State, FVector2Int Loc)
{
	TArray<FVector2Int> directions = { FVector2Int(1, 0), FVector2Int(-1, 0) , FVector2Int(0, 1), FVector2Int(0, -1) };

	if (!GetTileAt(Loc)) return true;
	
	FCell tile;
	GetTileAt(Loc, std::ref(tile));
	bool state = tile.*State;

	for (FVector2Int dir : directions)
	{
		FCell tileDir;
		if (GetTileAt(Loc + dir, tileDir)) state |= tile.*State;
	}

	return state;
}

bool AGridManager::GetNearestSafeTile(FVector2Int Start, FVector2Int& SafeLocation, float MinDist)
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

		FCell curTile;
		if (GetTileAt(SafeLocation, std::ref(curTile)) && curTile.IsEnabled && !NearTileWithState(&FCell::IsFalling, SafeLocation) && SafeLocation.Dist(Start) >= MinDist) return true;
	}

	SafeLocation = FVector2Int(0, 0);
	return false;
}

TArray<FVector2Int> AGridManager::FindPath(const FVector2Int& Start, const FVector2Int& Goal, float MaxDist, TArray<FVector2Int> TilesToAvoid)
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
			TArray<FCell> pathCells;
			TArray<FVector2Int> path;
			for (FNode* node = cur; node != nullptr; node = node->Parent)
			{
				FCell tile;
				if (GetTileAt(node->Point, std::ref(tile)))
				{
					FTransform tileTransform;
					InstancedMesh->GetInstanceTransform(tile.ID, tileTransform, true);
					path.Add(WorldToGrid(tileTransform.GetLocation()));
					pathCells.Add(tile);
				}
			}

			while (path.Num() > 0 && pathCells[0].IsFalling) path.RemoveAt(0);
			
			Algo::Reverse(path);

			for (auto& pair : open) delete pair.Value;

			return path;
		}

		open.Remove(cur->Point);
		closed.Add(cur->Point);

		for (const FVector2Int& dir : directions) 
		{
			FVector2Int neighbor = cur->Point + dir;
			FCell neighbourTile;
			GetTileAt(neighbor, std::ref(neighbourTile));
			if (!Tiles.Contains(neighbor) || closed.Contains(neighbor) || !neighbourTile.IsEnabled || NearTileWithState(&FCell::HasFallen, neighbor) || TilesToAvoid.Contains(neighbor)) continue;

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

