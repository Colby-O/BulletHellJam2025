#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.generated.h"

UCLASS()
class BULLETHELLJAM2025_API ABaseEnemy : public AActor
{
	GENERATED_BODY()

public:
	ABaseEnemy();
	~ABaseEnemy();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	static TArray<ABaseEnemy*> Enemies;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	class APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	class AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float VisionRange = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Spacing = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 1.0;

	bool IsAttacking = false;
	float AttackRange;
	ABaseEnemy* ClosestEnemy;

	FVector GetDirectionToPlayer();
	float GetDistToPlayer();
	void HandleAttack();
	void HandleRotation();
	void HandleMovement(float DeltaTime);
	void MoveTowards(float DeltaTime, FVector Direction);
	ABaseEnemy* GetClosestEnemy();
};
