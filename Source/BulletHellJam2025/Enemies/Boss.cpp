#include "BulletHellJam2025/Enemies/Boss.h"
#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/UI/UIManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "BulletHellJam2025/Enemies/BulletManager.h"
#include <Kismet/GameplayStatics.h>

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
	UIManager = Cast<AUIManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AUIManager::StaticClass()));
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	HasSetupHealth = false;
	CurrentHealth = 0;
	CurrentStage = EBossStage::None;
	ShooterComp->SetBoss(this);
	ShooterComp->Disable();

	FlagForReset = false;
	FlagForRestart = false;
	FlagForStageReset = false;
	IsReset = false;

	Close(true);
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasSetupHealth)
	{
		HasSetupHealth = true;
		GameView = UIManager->GetView<UGameViewWidget>();
		GameView->MaxBossHealth = MaxHealth;
		SetHealth(0);
	}

	if (FlagForStageReset) 
	{
		FlagForStageReset = false;
		CurrentStage = EBossStage::None;
		StopHealthFill();
		IsInStageCooldown = false;
		HasSetupHealth = false;
		NextStage();
		ResetBoss();
		IsReset = true;
	}
	else if (FlagForReset) 
	{
		FlagForReset = false;
		IsReset = true;
		ResetBoss();
	}
	else if (FlagForRestart) 
	{
		FlagForRestart = false;
		RestartBoss();
		IsReset = false;
	}

	if (Player->IsPaused) return;

	StageUpdate(CurrentStage);
	RotateTowardsPlayer();
}

void ABoss::NextStage()
{
	IsReset = false;
	OnStageChange(++CurrentStage);
}

void ABoss::OnStageChange(EBossStage Stage)
{
	UEnum* EnumPtr = StaticEnum<EBossStage>();
	if (EnumPtr) {
		UE_LOG(LogTemp, Log, TEXT("Boss Stage Changed to: %s"), *EnumPtr->GetNameStringByValue((int64)Stage));
	}

	HealthAtStartOfStage = CurrentHealth;

	switch (Stage)
	{
	case Start:
		BeginStartStage();
		break;
	case Stage1:
		BeginStage(Stage1ShootPattern, Stage1MaxHealthMul * MaxHealth);
		break;
	case Stage2:
		BeginStage(Stage2ShootPattern, Stage2MaxHealthMul * MaxHealth);
		break;
	case Stage3:
		BeginStage(Stage3ShootPattern, Stage3MaxHealthMul * MaxHealth);
		break;
	case Stage4:
		BeginStage(Stage4ShootPattern, Stage4MaxHealthMul * MaxHealth);
		break;
	case End:
		break;
	default:
		break;
	}
}

void ABoss::StageUpdate(EBossStage Stage)
{
	if (IsReset) return;

	switch (Stage)
	{
	case Start:
		StartUpdate();
		break;
	case Stage1:
		UpdateStage(Stage1HealthFillDuration, Stage1NumberOfEasyEnemies, Stage1NumberOfMediumEnemies, Stage1NumberOfHardEnemies);
		break;
	case Stage2:
		UpdateStage(Stage1HealthFillDuration, Stage2NumberOfEasyEnemies, Stage2NumberOfMediumEnemies, Stage2NumberOfHardEnemies);
		break;
	case Stage3:
		UpdateStage(Stage1HealthFillDuration, Stage3NumberOfEasyEnemies, Stage3NumberOfMediumEnemies, Stage3NumberOfHardEnemies);
		break;
	case Stage4:
		UpdateStage(Stage1HealthFillDuration, Stage4NumberOfEasyEnemies, Stage4NumberOfMediumEnemies, Stage4NumberOfHardEnemies);
		break;
	case End:
		break;
	default:
		break;
	}
}

void ABoss::StageRestart(EBossStage Stage)
{
	UEnum* EnumPtr = StaticEnum<EBossStage>();
	if (EnumPtr) {
		UE_LOG(LogTemp, Log, TEXT("Restart at Stage Changed to: %s"), *EnumPtr->GetNameStringByValue((int64)Stage));
	}

	switch (Stage)
	{
	case Start:
		StartStageReset();
		break;
	case Stage1:
		StageRestart(Stage1NumberOfEasyEnemies, Stage1NumberOfMediumEnemies, Stage1NumberOfHardEnemies);
		break;
	case Stage2:
		StageRestart(Stage2NumberOfEasyEnemies, Stage2NumberOfMediumEnemies, Stage2NumberOfHardEnemies);
		break;
	case Stage3:
		StageRestart(Stage3NumberOfEasyEnemies, Stage3NumberOfMediumEnemies, Stage3NumberOfHardEnemies);
		break;
	case Stage4:
		StageRestart(Stage4NumberOfEasyEnemies, Stage4NumberOfMediumEnemies, Stage4NumberOfHardEnemies);
		break;
	case End:
		break;
	default:
		break;
	}
}

void ABoss::BeginStartStage()
{
	if (!IsHealthFilling && CurrentHealth < MaxHealth * InitalHealthFillPercentage)
	{
		StartHealthFill(MaxHealth * InitalHealthFillPercentage, InitalHealthFillDuration);
	}

	Close(true);

	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(CloseAnimation, false);

	GridManager->Spawn(EasyEnemyPrefab, StartStageNumberOfEnemies);

	ShooterComp->SetShootPattern(StartStageShootPattern);
}

void ABoss::StartUpdate()
{
	if (!IsHealthFilling && CurrentHealth < MaxHealth) {
		StartHealthFill(MaxHealth, StartStageHealthFillDuration);
	}

	UE_LOG(LogTemp, Log, TEXT("Enemies Left: %d"), ABaseEnemy::Enemies.Num());

	if (ABaseEnemy::Enemies.Num() == 0) 
	{
		StopHealthFill();
		NextStage();
	}
}

void ABoss::StartStageReset()
{
	ShooterComp->Disable();
	BeginStartStage();
}

void ABoss::BeginStage(TArray<FShootPattern> Pattern, float NewMaxHealth)
{
	IsInStageCooldown = false;
	ShooterComp->SetShootPattern(Pattern);
	int newHealth = NewMaxHealth - MaxHealth + CurrentHealth;
	HealthAtStartOfStage = newHealth;
	GameView->MaxBossHealth = MaxHealth;
	SetHealth(newHealth);
	Open();
}

void ABoss::UpdateStage(float HealthFillDuration, int NumberOfEasy, int NumberOfMedium, int NumberOfHard)
{
	if (CurrentHealth <= PercentHealthNextStage * MaxHealth && !IsInStageCooldown)
	{
		IsInStageCooldown = true;

		GridManager->Spawn(EasyEnemyPrefab, NumberOfEasy);
		GridManager->Spawn(MediumEnemyPrefab, NumberOfMedium);
		GridManager->Spawn(HardEnemyPrefab, NumberOfHard);

		ShooterComp->Disable(true, true);

		Close(true);

		if (!IsHealthFilling && CurrentHealth < MaxHealth) {
			StartHealthFill(MaxHealth, HealthFillDuration);
		}
	}

	if (IsInStageCooldown && ABaseEnemy::Enemies.Num() == 0)
	{
		StopHealthFill();
		NextStage();
	}
}

void ABoss::StageRestart(int NumberOfEasy, int NumberOfMedium, int NumberOfHard)
{
	if (!IsInStageCooldown) 
	{
		ShooterComp->Enable();
		SetHealth(HealthAtStartOfStage);
		Open(true);
	}
	else 
	{
		Close(true);
		ShooterComp->Disable(true, true);
		GridManager->Spawn(EasyEnemyPrefab, NumberOfEasy);
		GridManager->Spawn(MediumEnemyPrefab, NumberOfMedium);
		GridManager->Spawn(HardEnemyPrefab, NumberOfHard);
	}
}

void ABoss::StartHealthFill(float To, float Duration)
{
	StopHealthFill();
	IsHealthFilling = true;
	CurrentFillTarget = FMath::Clamp(To, 0, MaxHealth);
	CurrentHealthFillDuration = Duration;
	if (CurrentHealth >= CurrentFillTarget) return;
	GetWorld()->GetTimerManager().SetTimer(HealthFillTimerHandle, this, &ABoss::HealthFillStep, CurrentHealthFillDuration / (CurrentFillTarget - CurrentHealth), true);
}

void ABoss::StopHealthFill()
{
	IsHealthFilling = false;
	GetWorld()->GetTimerManager().ClearTimer(HealthFillTimerHandle);
}

void ABoss::HealthFillStep()
{
	if (CurrentFillTarget >= CurrentHealth) 
	{
		TakeHealth(-1);
	}
	else 
	{
		SetHealth(CurrentFillTarget);
		StopHealthFill();
	}
}

void ABoss::OnDamageEnd()
{
	IsTakingDamage = false;
	SetRestMaterial();
}

void ABoss::SetHealth(float Health)
{
	if (!GameView) return;
	CurrentHealth = FMath::Clamp(Health, 0.0, MaxHealth);
	GameView->CurrentBossHealth = CurrentHealth;
}

void ABoss::TakeHealth(float Amount)
{
	if (!IsOpen && Amount >= 0) return; 
	if (Amount > 0) 
	{
		IsTakingDamage = true;
		SetDamagedMaterial();
		GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &ABoss::OnDamageEnd, 0.1f, false);

	}
	SetHealth(CurrentHealth - Amount);
}

FVector ABoss::GetDirectionToPlayer()
{
	FVector rawDirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	return -FVector(rawDirectionToPlayer.X, rawDirectionToPlayer.Y, 0);
}

void ABoss::RotateTowardsPlayer()
{
	if (!IsOpen && !IsStomping) return;
	FVector directionToPlayer = GetDirectionToPlayer();
	SetActorRotation(directionToPlayer.Rotation());
}

void ABoss::Open(bool Force)
{
	if (IsOpen && !Force) return;
	IsOpen = true;
	IsStomping = false;
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(OpenAnimation, false);
	SetOpenMaterial();
}

void ABoss::Close(bool Force)
{
	if (!IsOpen && !Force) return;
	IsOpen = false;
	GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(CloseAnimation, false);
	SetClosedMaterial();
}

void ABoss::PlayStompAnimation()
{
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(StompAnimation, false);
	IsOpen = false;
	IsStomping = true;
}

void ABoss::RestartBoss()
{
	ABaseEnemy::DestroyAllEnemies();
	StageRestart(CurrentStage);
}

void ABoss::ResetBoss()
{
	SetHealth(HealthAtStartOfStage);
	ShooterComp->ResetShooter();
	ShooterComp->Disable(true);
	ShooterComp->BulletManager->IsMarkedToRemoveBossBullets = true;
	ABaseEnemy::DestroyAllEnemies();
	Close(true);
}

