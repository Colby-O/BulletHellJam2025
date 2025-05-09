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

FVector AGridManager::GetRandomLocation()
{
	TArray<ATile*> tileList;
	Tiles.GenerateValueArray(tileList);

	int maxAttempts = 1000;
	int attempts = 0;
	while (attempts++ < maxAttempts) 
	{
		int randomIndex = FMath::RandRange(0, tileList.Num() - 1);

		if (!tileList[randomIndex]->IsDisabled) return tileList[randomIndex]->GetActorLocation();
	}

	return FVector();
}

void AGridManager::Spawn(TSubclassOf<AActor> Actor, int Number)
{
	for (int i = 0; i < Number; i++) 
	{
		FVector loc = GetRandomLocation();
		GetWorld()->SpawnActor<AActor>(Actor, loc, FRotator::ZeroRotator);
	}
}

void AGridManager::RollOutAttack(FVector Origin, TArray<FVector> Directions, int Width, float Rate, float Delay)
{
	if (IsRunningAttack) return;
	UE_LOG(LogTemp, Warning, TEXT("Starting Rollout Attack"));
	IsRunningAttack = true;
	CurrentRolloutDirections = Directions;
	CurrentRollOutWidth = Width;
	CurrentAttackFallDelay = Delay;

	for (const FVector& _ : Directions) 
	{
		FVector2Int startLoc = WorldToGrid(Origin);
		CurrentRollOutPosition.Add(startLoc);
		AtEndOfRollOut.Add(false);
		GetTileAt(startLoc)->TriggerFall(CurrentAttackFallDelay);
	}

	GetWorld()->GetTimerManager().SetTimer(AttackTimeHandle, this, &AGridManager::RollOutStep, Rate, true);
}

void AGridManager::RollOutStep()
{
	for (int i = 0; i < CurrentRollOutPosition.Num(); i++)
	{
		if (AtEndOfRollOut[i]) continue;

		FVector curLoc = GridToWorld(CurrentRollOutPosition[i]);

		FVector forward = CurrentRolloutDirections[i];
		FVector right = FVector::CrossProduct(forward, FVector::UpVector);

		FVector nextLoc = curLoc + TileSize * forward;

		UE_LOG(LogTemp, Warning, TEXT("Next Roll Out Loc is: %s"), *nextLoc.ToString());

		for (int j = -CurrentRollOutWidth; j <= CurrentRollOutWidth; j++)
		{
			FVector2Int nextGridLoc = WorldToGrid(nextLoc + (TileSize * 0.5f) * j * right);
			UE_LOG(LogTemp, Warning, TEXT("Next Roll Out Step Grid Loc is: %s"), *nextGridLoc.ToString());
			ATile* tile = GetTileAt(nextGridLoc);
			
			if (!tile && j == 0)
			{
				AtEndOfRollOut[i] = true;
				continue;
			}
			else if (j == 0) 
			{
				CurrentRollOutPosition[i] = nextGridLoc;
			}

			if (tile)
			{
				tile->TriggerFall(CurrentAttackFallDelay);
			}
		}
	}

	bool endAttack = true;
	for (bool state : AtEndOfRollOut) endAttack &= state;
	UE_LOG(LogTemp, Warning, TEXT("Is At End Of Attack: %d"), endAttack);
	if (endAttack) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopping Attack"))
		StopAttack();
	}
}

void AGridManager::MeteoriteAttack(int Size, int Gap, float Delay)
{
	if (IsRunningAttack) return;
	UE_LOG(LogTemp, Warning, TEXT("Starting Meteorite Attack"));
	IsRunningAttack = true;
	CurrentAttackFallDelay = Delay;

	int mapTileRadius = MapRadius / TileSize;
	for (int i = -mapTileRadius; i <= mapTileRadius; i = i + Size + Gap) 
	{
		for (int j = -mapTileRadius; j <= mapTileRadius; j = j + Size + Gap)
		{
			for (int k = i; k <= i + Size; k++)
			{
				for (int t = j; t <= j + Size; t++)
				{
					if (ATile* tile = GetTileAt(FVector2Int(k, t)))
					{
						tile->TriggerFall(CurrentAttackFallDelay);
					}
				}
			}
		}
	}

	StopAttack();
}

void AGridManager::RadiusAttack(FVector Origin, FVector StartDirection, float Rate, float Delay)
{
	if (IsRunningAttack) return;
	UE_LOG(LogTemp, Warning, TEXT("Starting Radius Attack"));
	IsRunningAttack = true;

	CurrentAttackFallDelay = Delay;
	CurrentRadiusAttackDirection = StartDirection;
	CurrentRadiusAttackOrigin = Origin;
	CurrentRadiusAttackAngle = 0.0f;

	GetWorld()->GetTimerManager().SetTimer(AttackTimeHandle, this, &AGridManager::RadiusAttackStep, Rate / 360.0f, true);
}

void AGridManager::RadiusAttackStep()
{
	FVector2Int curLoc = WorldToGrid(CurrentRadiusAttackOrigin);
	ATile* nextTile = GetTileAt(curLoc);

	FQuat RotationQuat = FQuat(FVector::UpVector, FMath::DegreesToRadians(CurrentRadiusAttackAngle));
	FVector direction = RotationQuat.RotateVector(CurrentRadiusAttackDirection);
	UE_LOG(LogTemp, Warning, TEXT("Current Direction Is: %s"), *direction.ToString());
	FVector nextLoc = GridToWorld(curLoc);
	
	int visited = 0;
	while (nextTile && visited < MapRadius / TileSize)
	{
		FVector2Int nextGridLoc = WorldToGrid(nextLoc);
	
		if (ATile* tile = GetTileAt(nextGridLoc))
		{
			tile->TriggerFall(CurrentAttackFallDelay);
		}

		nextLoc = nextLoc + TileSize * direction;
		nextTile = GetTileAt(WorldToGrid(nextLoc));
		visited++;
	}

	CurrentRadiusAttackAngle += 1;
	UE_LOG(LogTemp, Warning, TEXT("Current Angle is: %f"), CurrentRadiusAttackAngle);
	if (CurrentRadiusAttackAngle >= 360.0) StopAttack();
}

void AGridManager::StopAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("Stopped Attack"));
	GetWorld()->GetTimerManager().ClearTimer(AttackTimeHandle);

	CurrentRollOutPosition.Empty();
	AtEndOfRollOut.Empty();
	CurrentAttackFallDelay = -1;
	IsRunningAttack = false;
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
		if (curTile && !curTile->IsDisabled && !NearTileWithState(&ATile::IsFalling, SafeLocation) && SafeLocation.Dist(Start) >= MinDist) return curTile;
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
			if (!Tiles.Contains(neighbor) || closed.Contains(neighbor) || NearTileWithState(&ATile::IsDisabled, neighbor) || NearTileWithState(&ATile::HasFallen, neighbor) || TilesToAvoid.Contains(neighbor)) continue;

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

