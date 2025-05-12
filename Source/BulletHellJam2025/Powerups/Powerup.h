#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sound/SoundMix.h"
#include "Powerup.generated.h"

UENUM()
enum class EPowerupType
{
	Health,
	FireRate,
	Damage
};

UCLASS()
class BULLETHELLJAM2025_API APowerup : public AActor
{
	GENERATED_BODY()
	
public:	
	APowerup();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	static int Count;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* GoodSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* BadSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundMix* SoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* HealthMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* FireMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* DmgMat;

	UPROPERTY(EditAnywhere)
	EPowerupType PowerupType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CollisionDist = 150.0f;;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FailChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HealAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRateIncreaseFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRateIncreaseDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageIncreaseFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageIncreaseDuration;

	UPROPERTY(EditAnywhere)
	float TileFallDelay;

	UPROPERTY(EditAnywhere)
	int RollOutWidth;

	UPROPERTY(EditAnywhere)
	float RollOutRate;

	UPROPERTY(EditAnywhere)
	int MeteoriteSize;

	UPROPERTY(EditAnywhere)
	int MeteoriteGap;

	UPROPERTY(EditAnywhere)
	float RadiusRate;

	float LifeSpan = 30.0f;
	float Life = 0;

	class AGridManager* GridManager;
	class APlayerCharacter* Player;

	UFUNCTION()
	void CheckForOverlap();
};
