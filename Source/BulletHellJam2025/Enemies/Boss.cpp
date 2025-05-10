#include "BulletHellJam2025/Enemies/Boss.h"
#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/UI/UIManager.h"
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
		NextStage();
	}

	StageUpdate(CurrentStage);
	RotateTowardsPlayer();
}

void ABoss::NextStage()
{
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
		ShooterComp->Enable();
		Open();
		break;
	case Stage2:
		break;
	case Stage3:
		break;
	case End:
		break;
	default:
		break;
	}
}

void ABoss::StageUpdate(EBossStage Stage)
{
	switch (Stage)
	{
	case Start:
		StartUpdate();
		break;
	case Stage1:
		break;
	case Stage2:
		break;
	case Stage3:
		break;
	case End:
		break;
	default:
		break;
	}
}

void ABoss::StageReset(EBossStage Stage)
{
	switch (Stage)
	{
	case Start:
		StartStageReset();
		break;
	case Stage1:
		SetHealth(HealthAtStartOfStage);
		Open(true);
		break;
	case Stage2:
		break;
	case Stage3:
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

	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(CloseAnimation, false);

	GridManager->Spawn(EnemyPrefab, StartStageNumberOfEnemies);
}

void ABoss::StartUpdate()
{
	if (!IsHealthFilling && CurrentHealth < MaxHealth) {
		StartHealthFill(MaxHealth, StartStageHealthFillDuration);
	}

	if (ABaseEnemy::Enemies.Num() == 0) 
	{
		StopHealthFill();
		NextStage();
	}
}

void ABoss::StartStageReset()
{
	ShooterComp->Disable();
	ABaseEnemy::DestroyAllEnemies();
	BeginStartStage();
	//SetHealth(0);
	//StopHealthFill();
	//BeginStartStage();
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

void ABoss::SetHealth(float Health)
{
	if (!GameView) return;
	CurrentHealth = FMath::Clamp(Health, 0.0, MaxHealth);
	GameView->CurrentBossHealth = CurrentHealth;
}

void ABoss::TakeHealth(float Amount)
{
	if (!IsOpen && Amount >= 0) return;
	SetHealth(CurrentHealth - Amount);
}

FVector ABoss::GetDirectionToPlayer()
{
	FVector rawDirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	return -FVector(rawDirectionToPlayer.X, rawDirectionToPlayer.Y, 0);
}

void ABoss::RotateTowardsPlayer()
{
	if (!IsOpen) return;

	FVector directionToPlayer = GetDirectionToPlayer();
	SetActorRotation(directionToPlayer.Rotation());
}

void ABoss::Open(bool Force)
{
	if (IsOpen && !Force) return;
	IsOpen = true;
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(OpenAnimation, false);
}

void ABoss::Close(bool Force)
{
	if (!IsOpen && !Force) return;
	IsOpen = false;
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(CloseAnimation, false);
}

void ABoss::PlayStompAnimation()
{
	Mesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	Mesh->PlayAnimation(StompAnimation, false);
	IsOpen = false;
}

void ABoss::ResetBoss()
{
	ShooterComp->ResetShooter();
	StageReset(CurrentStage);
}

