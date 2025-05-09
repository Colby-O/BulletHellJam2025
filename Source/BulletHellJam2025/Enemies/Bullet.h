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
	FBullet(int InstanceID, FVector F, float S, float L, float hitDist, float dmg, FString T = "");
	~FBullet();

	int ID;
	float Speed;
	float LifeSpan;
	float Life;
	float CollisionDist;
	float Damage;
	FVector Forward;
	FString Tag;
};
