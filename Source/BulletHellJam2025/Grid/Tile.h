#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundMix.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FallSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundMix* SoundMix;

	UFUNCTION(BlueprintCallable)
	void TriggerFall(float FallDelayOverride = -1);

	UFUNCTION(BlueprintCallable)
	void ResetTile();

	void StopFallIfPossible();
	void StartFall();
	void Fall();
	void ForceStopFall();
	void SetColor(FLinearColor Color);

	UStaticMeshComponent* Mesh;
	UMaterialInterface* Mat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	bool IsDisabled = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool HasFallen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float FallTime = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float ResetTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float FallDelay = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	float FallAmount = 1000;

	bool IsPaused = false;
	float RemainingTime = -1;

	void Pause();
	void Resume();

	FLinearColor DefaultColor;
	FTimerHandle TimerHandler;
};
