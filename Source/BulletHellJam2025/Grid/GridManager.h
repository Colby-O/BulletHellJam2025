#pragma once

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
	TMap<FVector2D, ATile*> Tiles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int TileSize;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector2D WorldToGrid(FVector worldPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	FVector GridToWorld(FVector2D gridPt) const;

	UFUNCTION(BlueprintCallable, Category = "Tiles")
	ATile* GetTileAt(FVector2D pt) const;

};
