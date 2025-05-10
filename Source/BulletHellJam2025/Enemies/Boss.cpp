#include "BulletHellJam2025/Enemies/Boss.h"
#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/UI/UIManager.h"
#include "Components/SkeletalMeshComponent.h"
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
		BeginStage1();
		break;
	case Stage2:
		BeginStage2();
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
		Stage1Update();
		break;
	case Stage2:
		Stage2Update();
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
		Stage1Reset();
		break;
	case Stage2:
		Stage2Reset();
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

	ShooterComp->SetShootPattern(StartStageShootPattern);
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

void ABoss::BeginStage1()
{
	IsInStage1Cooldown = false;
	ShooterComp->SetShootPattern(Stage1ShootPattern);
	Open();
}

void ABoss::Stage1Update()
{
	if (CurrentHealth <= PercentHealthNextStage * MaxHealth && !IsInStage1Cooldown)
	{
		IsInStage1Cooldown = true;
		GridManager->Spawn(EnemyPrefab, Stage1NumberOfEnemies);

		ShooterComp->Disable(true, true);

		Close();

		if (!IsHealthFilling && CurrentHealth < MaxHealth) {
			StartHealthFill(MaxHealth, Stage1HealthFillDuration);
		}
	}

	if (IsInStage1Cooldown && ABaseEnemy::Enemies.Num() == 0) 
	{
		StopHealthFill();
		NextStage();
	}
}

void ABoss::Stage1Reset()
{
	if (!IsInStage1Cooldown) 
	{
		SetHealth(HealthAtStartOfStage);
		Open(true);
	}
	else 
	{
		Close(true);
		ShooterComp->Disable(true, true);
		ABaseEnemy::DestroyAllEnemies();
		GridManager->Spawn(EnemyPrefab, Stage1NumberOfEnemies);
	}
}

void ABoss::BeginStage2()
{
	ShooterComp->SetShootPattern(Stage2ShootPattern);
	Open();
}

void ABoss::Stage2Update()
{

}

void ABoss::Stage2Reset()
{

}

void ABoss::BeginStage3()
{

}

void ABoss::Stage3Update()
{

}

void ABoss::Stage3Reset()
{

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
	IsStomping = true;
}

void ABoss::ResetBoss()
{
	ShooterComp->ResetShooter();
	StageReset(CurrentStage);
}

