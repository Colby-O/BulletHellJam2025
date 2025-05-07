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

	float NormalizeAngle360(float Angle);

	float ClampAngle(float Angle, float Min, float Max, bool CanLoop, float Frequency);

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AActor> BulletClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FromTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Offset = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool YawLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PitchLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool RollLoop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LoopFrequency;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotMin;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector RotMax;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LifeSpan = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> SpawnDirections;

	FVector VelPrediction;
	FVector LastKnownPostion;

	void Enable();
	void Disable();
	void Shoot(FVector Vel = FVector::ZeroVector);
	void ShootInternal();
	void SetFrom(FString Tag);
	FVector GetShootDirection(int index);

protected:
	FRotator RawRotation;
	FTimerHandle FireTimerHandler;
	bool IsEnabled;
};
