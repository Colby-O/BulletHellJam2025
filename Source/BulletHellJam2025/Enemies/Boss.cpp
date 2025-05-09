#include "BulletHellJam2025/Enemies/Boss.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
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
	HasSetupHealth = false;
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasSetupHealth) 
	{
		HasSetupHealth = true;
		GameView = UIManager->GetView<UGameViewWidget>();
		GameView->MaxBossHealth = MaxHealth;
		SetHealth(MaxHealth);
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
	SetHealth(CurrentHealth - Amount);
}

void ABoss::ResetBoss()
{
	ShooterComp->ResetShooter();
}

