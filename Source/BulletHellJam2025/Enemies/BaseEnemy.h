#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.generated.h"

class ATile;
struct FVector2Int;

UENUM(BlueprintType)
enum EMovementState {
	Chasing UMETA(DisplayName = "Chasing"),
	MovingOffTile UMETA(DisplayName = "MovingOffTil"), 
	MovingAwayFromOther UMETA(DisplayName = "MovingAwayFromOther"), 
	Idle UMETA(DisplayName = "Idle")
};


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

	void static DestroyAllEnemies();
	void static DestroyEnemy(ABaseEnemy* Enemy);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool DebugMovement = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	class AGameManager* GameManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	class AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "References")
	class APlayerCharacter* Player;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float VisionRange = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Spacing = 100.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Memory = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float Speed = 1.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float KnockbackRate = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float KnockbackAmount = 100.0;

	bool IsMarkedForRemoval = false;
	bool IsAttacking = false;
	bool IsKnockingBack = false;
	bool HoldPosition = true;
	float KnockbackElapsed = 0.0f;
	float TimeStep = 0.01f;
	float AttackRange;
	FVector KnockbackStart;
	FVector KnockbackEnd;
	ABaseEnemy* ClosestEnemy;

	EMovementState CurrentState;
	EMovementState LastState = EMovementState::Idle;

	TArray<ATile*> CurrentPath;

	FTimerHandle KnockbackTimerHandle;
	FTimerHandle ChaseTimerHandle;

	FVector GetDirectionToPlayer();
	FVector GetDirectionToNextTile();
	float GetDistToPlayer();
	void UpdateState();
	void LocateTarget();
	void HandleAttack();
	void HandleRotation();
	void HandleMovement(float DeltaTime);
	void MoveTowards(float DeltaTime, FVector Direction);
	void Knockback(FVector Direction);
	void KnockbackStep();
	void RelocatePlayer();
	void CheckForDeath();
	TArray<FVector2Int> GetTilesToIgnore();
	ABaseEnemy* GetClosestEnemy();
};