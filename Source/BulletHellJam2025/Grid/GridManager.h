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
	ATile* GetTileAt(FVector2Int pt) const;

};
