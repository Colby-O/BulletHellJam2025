#include "BulletHellJam2025/UI/UIManager.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"

AUIManager::AUIManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUIManager::BeginPlay()
{
	Super::BeginPlay();

	UGameViewWidget* gameView = CreateWidget<UGameViewWidget>(GetWorld(), GameViewClass);
	AddView<UGameViewWidget>(gameView);
}

void AUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

