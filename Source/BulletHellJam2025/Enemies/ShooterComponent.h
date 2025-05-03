#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ShooterComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BULLETHELLJAM2025_API UShooterComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UShooterComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Shoot();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotSpeed = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeSpan = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> SpawnDirections;

	FTimerHandle FireTimerHandler;
};
