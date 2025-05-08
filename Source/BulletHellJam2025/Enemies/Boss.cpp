#include "BulletHellJam2025/Enemies/Boss.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"

ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoss::ResetBoss()
{
	ShooterComp->ResetShooter();
}

