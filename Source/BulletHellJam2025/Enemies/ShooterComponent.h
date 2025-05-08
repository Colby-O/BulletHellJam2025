#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BulletHellJam2025/Enemies/ShootPattern.h"
#include "ShooterComponent.generated.h"

class ABulletManager;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	class ABulletManager* BulletManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString FromTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor BulletColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector Scale = FVector(0.2, 0.2, 0.2);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Offset = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CollisionDist = 100;

	UPROPERTY(EditAnywhere)
	TArray<FShootPattern> ShootPatterns;

	FShootPattern SelectedPattern;
	int SelectedPatternIndex;

	FVector VelPrediction;
	FVector LastKnownPostion;
	float Timer;

	void Enable(bool Force = false);
	void Disable(bool Force = false);
	void Shoot(FVector Vel = FVector::ZeroVector);
	void ShootInternal();
	void SetFrom(FString Tag);
	FVector GetShootDirection(int index);
	void NextPattern();
	void ResetShooter();

protected:
	FRotator RawRotation;
	FTimerHandle FireTimerHandler;
	FTimerHandle PatternTimerHandler;
	bool IsEnabled;
};
