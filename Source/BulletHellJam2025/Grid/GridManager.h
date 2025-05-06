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

	void RegisterTile(ATile* Tile);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TMap<FVector2Int, ATile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	int TileSize = 100;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector2Int WorldToGrid(FVector worldPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector GridToWorld(FVector2Int gridPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	ATile* GetTileAt(const FVector2Int& pt) const;

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
