#pragma once

#include "CoreMinimal.h"
#include "ShootPattern.generated.h"

USTRUCT(BlueprintType)
struct BULLETHELLJAM2025_API FShootPattern
{
	GENERATED_BODY()

public:
	FShootPattern();
	~FShootPattern();

	bool operator==(const FShootPattern& Other);

	int ID;

	UPROPERTY(EditAnywhere)
	float Duration;

	UPROPERTY(EditAnywhere)
	float Speed = 1000.0;

	UPROPERTY(EditAnywhere)
	float FireRate = 1;

	UPROPERTY(EditAnywhere)
	FVector RotSpeed;

	UPROPERTY(EditAnywhere)
	bool YawLoop;

	UPROPERTY(EditAnywhere)
	bool PitchLoop;

	UPROPERTY(EditAnywhere)
	bool RollLoop;

	UPROPERTY(EditAnywhere)
	FVector LoopFrequency;

	UPROPERTY(EditAnywhere)
	FVector RotMin;

	UPROPERTY(EditAnywhere)
	FVector RotMax;

	UPROPERTY(EditAnywhere)
	bool MovesStationary = false;
	UPROPERTY(EditAnywhere)
	float MovesStationaryRadius = 1000;
	UPROPERTY(EditAnywhere)
	float MoveStationarySpacing = 10;

	UPROPERTY(EditAnywhere)
	float Distance = 1000;

	UPROPERTY(EditAnywhere)
	float TileFallDelay;

	UPROPERTY(EditAnywhere)
	bool IsRollOutGridPattern;

	UPROPERTY(EditAnywhere)
	int RollOutWidth;

	UPROPERTY(EditAnywhere)
	float RollOutRate;

	UPROPERTY(EditAnywhere)
	bool IsMeteoriteGridPattern;

	UPROPERTY(EditAnywhere)
	int MeteoriteSize;

	UPROPERTY(EditAnywhere)
	int MeteoriteGap;

	UPROPERTY(EditAnywhere)
	bool IsRadiusGridPattern;

	UPROPERTY(EditAnywhere)
	float RadiusRate;

	bool HasRanGridPattern = false;

	float LifeSpan;

	UPROPERTY(EditAnywhere)
	TArray<FVector> SpawnDirections;

	void Awake();
	bool IsGridPattern();
};
