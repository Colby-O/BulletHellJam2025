#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

USTRUCT(BlueprintType)
struct BULLETHELLJAM2025_API FBullet
{
	GENERATED_BODY()
	
public:	
	FBullet();
	FBullet(int InstanceID, FVector F, float S, float L, FString T = "");
	~FBullet();

	int ID;
	float Speed;
	float LifeSpan;
	float Life;
	FVector Forward;
	FString Tag;
};
