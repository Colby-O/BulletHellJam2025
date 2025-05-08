#include "BulletHellJam2025/GameManager.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Enemies/BulletManager.h"
#include "BulletHellJam2025/Enemies/Boss.h"
#include <Kismet/GameplayStatics.h>

AGameManager::AGameManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	BulletManager = Cast<ABulletManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
}

void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGameManager::RestartGame()
{
	if (GridManager) GridManager->ResetGrid();
	if (BulletManager) BulletManager->IsMarkedForReset = true;
	if (Player) Player->ResetPlayer();
	if (Boss) Boss->ResetBoss();
}