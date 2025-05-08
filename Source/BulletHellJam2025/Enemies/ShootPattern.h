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
	float LifeSpan = 1.0;

	UPROPERTY(EditAnywhere)
	TArray<FVector> SpawnDirections;
};
