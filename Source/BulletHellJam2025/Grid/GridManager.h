#pragma once

#include "BulletHellJam2025/Core/Vector2Int.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ATile;

UCLASS()
class BULLETHELLJAM2025_API AGridManager : public AActor
{
	GENERATED_BODY()

public:	
	AGridManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void UpdateTileVisibility();

	void RegisterTile(ATile* Tile);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TMap<FVector2Int, ATile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	bool DisableMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	int TileSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float ViewDist = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float MapRadius = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float BossRadius = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	FVector MapCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	TSubclassOf<AActor> TilePrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	TSubclassOf<AActor> BossStandPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	TSubclassOf<AActor> EdgePrefab;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector2Int WorldToGrid(FVector worldPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector GridToWorld(FVector2Int gridPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	ATile* GetTileAt(const FVector2Int& pt) const;

	void GenerateGrid();
	void ResetGrid();

	FVector GetRandomLocation();
	void Spawn(TSubclassOf<AActor> Actor, int Number = 1);


	FTimerHandle AttackTimeHandle;
	bool IsRunningAttack = false;
	float CurrentAttackFallDelay;

	TArray<FVector2Int> CurrentRollOutPosition;
	TArray<FVector> CurrentRolloutDirections;
	TArray<bool> AtEndOfRollOut;
	int CurrentRollOutWidth;
	void RollOutAttack(FVector Origin, TArray<FVector> Directions, int Width, float Rate, float Delay = -1);
	void RollOutStep();

	void MeteoriteAttack(int Size, int Gap, float Delay = -1);

	float CurrentRadiusAttackAngle;
	FVector CurrentRadiusAttackDirection;
	FVector CurrentRadiusAttackOrigin;
	void RadiusAttack(FVector Origin, FVector StartDirection, float Rate, float Delay = -1);
	void RadiusAttackStep();

	void StopAttack();

	float GetHeuristic(const FVector2Int& A, const FVector2Int& B);
	bool NearTileWithState(bool ATile::* State, FVector2Int Loc);
	ATile* GetNearestSafeTile(FVector2Int Start, FVector2Int& SafeLocation, float MinDist = SMALL_NUMBER);
	TArray<ATile*> FindPath(const FVector2Int& Start, const FVector2Int& Goal, float MaxDist = 5, TArray<FVector2Int> TilesToAvoid = {});
};

struct FNode
{
	FVector2Int Point;
	FNode* Parent;
	float Cost;
	float Dst;

	FNode(FVector2Int Point, FNode* Parent, float Cost, float Dst) : Point(Point), Parent(Parent), Cost(Cost), Dst(Dst) {}
};
