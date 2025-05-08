#pragma once

#include "CoreMinimal.h"
#include "Cell.generated.h"

USTRUCT(BlueprintType)
struct BULLETHELLJAM2025_API FCell
{
	GENERATED_BODY()

public:
	FCell();
	FCell(int InstanceID, bool Enabled = true);
	~FCell();

	int ID;
	bool IsEnabled = true;
	bool IsFalling = false;
	bool HasFallen = false;
};