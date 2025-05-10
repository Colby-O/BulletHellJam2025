#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"
#include "BulletHellJam2025/Enemies/ShootPattern.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* OpenMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* ClosedMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UAnimSequence* OpenAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UAnimSequence* CloseAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UAnimSequence* StompAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage General")
	float PercentHealthNextStage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	TArray<FShootPattern> StartStageShootPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float InitalHealthFillDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float StartStageHealthFillDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	float InitalHealthFillPercentage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	int StartStageNumberOfEnemies = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	TArray<FShootPattern> Stage1ShootPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	int Stage1NumberOfEnemies = 7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	float Stage1HealthFillDuration;

	bool IsInStage1Cooldown = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	TArray<FShootPattern> Stage2ShootPattern;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	TArray<FShootPattern> Stage3ShootPattern;

	class AGridManager* GridManager;
	class AUIManager* UIManager;
	class APlayerCharacter* Player;
	UGameViewWidget* GameView;

	bool HasSetupHealth = false;
	EBossStage CurrentStage;

	FTimerHandle HealthFillTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 1000;
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collsions")
	float CollisonDist = 500;
	float HealthAtStartOfStage;

	bool IsHealthFilling = false;
	float CurrentHealthFillDuration;
	float CurrentFillTarget;

	UPROPERTY(BlueprintReadonly)
	bool IsOpen;

	UPROPERTY(BlueprintReadonly)
	bool IsStomping;

	void NextStage();
	void OnStageChange(EBossStage Stage);
	void StageUpdate(EBossStage Stage);
	void StageReset(EBossStage Stage);

	void BeginStartStage();
	void StartUpdate();
	void StartStageReset();

	void BeginStage1();
	void Stage1Update();
	void Stage1Reset();

	void BeginStage2();
	void Stage2Update();
	void Stage2Reset();

	void BeginStage3();
	void Stage3Update();
	void Stage3Reset();

	void StartHealthFill(float To, float Duration);
	void StopHealthFill();
	void HealthFillStep();
	void SetHealth(float Health);
	void TakeHealth(float Amount);
	FVector GetDirectionToPlayer();
	void RotateTowardsPlayer();
	void Open(bool Force = false);
	void Close(bool Force = false);
	void PlayStompAnimation();
	void ResetBoss();
};
