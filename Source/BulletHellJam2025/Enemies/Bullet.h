#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class BULLETHELLJAM2025_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	ABullet();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Remove();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletSpeed = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeSpan = 1;

	FTimerHandle LifeHandler;
};
