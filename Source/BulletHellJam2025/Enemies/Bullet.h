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
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult& SweepResult);

	void Remove();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BulletSpeed = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeSpan = 1;

	FTimerHandle LifeHandler;
	UStaticMeshComponent* Mesh;
};
