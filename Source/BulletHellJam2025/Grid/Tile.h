#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class BULLETHELLJAM2025_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATile();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AGridManager* GridManager;

	UFUNCTION(BlueprintCallable)
	void TriggerFall();

	UFUNCTION(BlueprintCallable)
	void ResetTile();

	void StartFall();
	void Fall();
	void SetColor(FLinearColor Color);

	UStaticMeshComponent* Mesh;
	UMaterialInterface* Mat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasFallen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float FallTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float FallDelay = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float FallAmount = 1000;

	FLinearColor DefaultColor;
	FTimerHandle TimerHandler;
};
