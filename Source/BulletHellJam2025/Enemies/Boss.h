#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"
#include "Boss.generated.h"

UENUM(BlueprintType)
enum EBossStage {
	None,
	Start,
	Stage1,
	Stage2,
	Stage3,
	End
};

inline EBossStage& operator++(EBossStage& b) {
	b = static_cast<EBossStage>((static_cast<int>(b) + 1) % static_cast<int>(EBossStage::End));
	return b;
}

UCLASS()
class BULLETHELLJAM2025_API ABoss : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoss();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TSubclassOf<AActor> EnemyPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float InitalHealthFillDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float StartStageHealthFillDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float InitalHealthFillPercentage;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float StartStageNumberOfEnemies = 5;

	class AGridManager* GridManager;
	class AUIManager* UIManager;
	UGameViewWidget* GameView;

	bool HasSetupHealth = false;
	EBossStage CurrentStage;

	FTimerHandle HealthFillTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 1000;
	float CurrentHealth;

	bool IsHealthFilling = false;
	float CurrentHealthFillDuration;
	float CurrentFillTarget;

	void NextStage();
	void OnStageChange(EBossStage Stage);
	void StageUpdate(EBossStage Stage);
	void StageReset(EBossStage Stage);
	void BeginStartStage();
	void StartUpdate();
	void StartStageReset();
	void StartHealthFill(float To, float Duration);
	void StopHealthFill();
	void HealthFillStep();
	void SetHealth(float Health);
	void TakeHealth(float Amount);
	void ResetBoss();
};
