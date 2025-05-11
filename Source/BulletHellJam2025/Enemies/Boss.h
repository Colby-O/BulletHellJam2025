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
	Stage4,
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
	TSubclassOf<AActor> EasyEnemyPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TSubclassOf<AActor> MediumEnemyPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TSubclassOf<AActor> HardEnemyPrefab;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* OpenMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* ClosedMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* RestMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* DisabledMat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* DamagedMat;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Start Stage")
	int StartMaxHealthMul = 1;


	bool IsInStageCooldown = false;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	TArray<FShootPattern> Stage1ShootPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	int Stage1NumberOfEasyEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	int Stage1NumberOfMediumEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	int Stage1NumberOfHardEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	float Stage1HealthFillDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 1")
	float Stage1MaxHealthMul = 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	TArray<FShootPattern> Stage2ShootPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	int Stage2NumberOfEasyEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	int Stage2NumberOfMediumEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	int Stage2NumberOfHardEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	float Stage2HealthFillDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 2")
	float Stage2MaxHealthMul = 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	TArray<FShootPattern> Stage3ShootPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	int Stage3NumberOfEasyEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	int Stage3NumberOfMediumEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	int Stage3NumberOfHardEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	float Stage3HealthFillDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 3")
	float Stage3MaxHealthMul = 1;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 4")
	TArray<FShootPattern> Stage4ShootPattern;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 4")
	int Stage4NumberOfEasyEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 4")
	int Stage4NumberOfMediumEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 4")
	int Stage4NumberOfHardEnemies = 7;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 4")
	float Stage4HealthFillDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage 4")
	float Stage4MaxHealthMul = 1;

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
	bool IsTakingDamage;

	UFUNCTION(BlueprintImplementableEvent)
	void SetRestMaterial();

	UFUNCTION(BlueprintImplementableEvent)
	void SetDamagedMaterial();

	UFUNCTION(BlueprintImplementableEvent)
	void SetOpenMaterial();

	UFUNCTION(BlueprintImplementableEvent)
	void SetClosedMaterial();

	FTimerHandle DamageTimerHandle;

	UPROPERTY(BlueprintReadonly)
	bool IsOpen;

	UPROPERTY(BlueprintReadonly)
	bool IsStomping;

	bool FlagForReset = false;
	bool FlagForRestart = false;
	bool IsReset = false;

	void NextStage();
	void OnStageChange(EBossStage Stage);
	void StageUpdate(EBossStage Stage);
	void StageRestart(EBossStage Stage);

	void BeginStartStage();
	void StartUpdate();
	void StartStageReset();

	void BeginStage(TArray<FShootPattern> Pattern, float NewMaxHealth);
	void UpdateStage(float HealthFillDuration, int NumberOfEasy = 0, int NumberOfMedium = 0, int NumberOfHard = 0);
	void StageRestart(int NumberOfEasy, int NumberOfMedium, int NumberOfHard);

	void StartHealthFill(float To, float Duration);
	void StopHealthFill();
	void HealthFillStep();
	void OnDamageEnd();
	void SetHealth(float Health);
	void TakeHealth(float Amount);
	FVector GetDirectionToPlayer();
	void RotateTowardsPlayer();
	void Open(bool Force = false);
	void Close(bool Force = false);
	void PlayStompAnimation();
	void RestartBoss();
	void ResetBoss();
};
