#pragma once

#include "BulletHellJam2025/Core/Vector2Int.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHellJam2025/Grid/Cell.h"
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

	void RegisterTile(FCell Tile);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UStaticMesh* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* BaseMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* OutlineMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tiles")
	TMap<FVector2Int, FCell> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	bool DisableMap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	FLinearColor BaseOutlineColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	FLinearColor FallOutlineColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	int TileSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float TileFallTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float TileResetTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float TileFallDelay = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float TileFallAmount = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float ViewDist = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float MapRadius = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	float BossRadius = 200;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	FVector MapCenter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	FVector TileScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tiles")
	FVector EdgeTileScale;

	UInstancedStaticMeshComponent* InstancedMesh;
	UMaterialInstanceDynamic* DynamicOutlineMaterial;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector2Int WorldToGrid(FVector worldPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector GridToWorld(FVector2Int gridPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	bool GetTileAt(const FVector2Int& pt, FCell& Cell);

	bool GetTileAt(const FVector2Int& pt) const;
	void GenerateGrid();
	void SpawnTile(FVector Location, FRotator Rotation, FVector Scale, bool IsEnabled);
	void SetTileColorAt(FVector2Int Location, FLinearColor Color);
	void SetTileColor(FCell Cell, FLinearColor Color);


	float GetHeuristic(const FVector2Int& A, const FVector2Int& B);
	bool NearTileWithState(bool FCell::* State, FVector2Int Loc);
	bool GetNearestSafeTile(FVector2Int Start, FVector2Int& SafeLocation, float MinDist = SMALL_NUMBER);
	TArray<FVector2Int> FindPath(const FVector2Int& Start, const FVector2Int& Goal, float MaxDist = 5, TArray<FVector2Int> TilesToAvoid = {});
};

struct FNode
{
	FVector2Int Point;
	FNode* Parent;
	float Cost;
	float Dst;

	FNode(FVector2Int Point, FNode* Parent, float Cost, float Dst) : Point(Point), Parent(Parent), Cost(Cost), Dst(Dst) {}
};
