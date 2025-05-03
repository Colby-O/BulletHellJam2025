#pragma once

#include "CoreMinimal.h"
#include "Vector2Int.generated.h"

USTRUCT(BlueprintType)
struct BULLETHELLJAM2025_API FVector2Int
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Y;

	FVector2Int();
	FVector2Int(int X, int Y);
	~FVector2Int();

	bool operator==(const FVector2Int& Other) const;
	bool operator!=(const FVector2Int& Other) const;
	FString ToString() const;
	friend uint32 GetTypeHash(const FVector2Int& Vector);
};
