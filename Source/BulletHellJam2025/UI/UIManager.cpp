#include "BulletHellJam2025/UI/UIManager.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include <Kismet/GameplayStatics.h>

AUIManager::AUIManager()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AUIManager::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	UGameViewWidget* gameView = CreateWidget<UGameViewWidget>(GetWorld(), GameViewClass);
	AddView<UGameViewWidget>(gameView);

	UMainMenuView* mainMenuView = CreateWidget<UMainMenuView>(GetWorld(), MainMenuViewClass);
	mainMenuView->SetPlayer(Player);
	AddView<UMainMenuView>(mainMenuView);

	UPauseView* pauseView = CreateWidget<UPauseView>(GetWorld(), PauseViewClass);
	pauseView->SetPlayer(Player);
	AddView<UPauseView>(pauseView);

	HideAll();
	ShowView<UMainMenuView>();
}

void AUIManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AUIManager::HideAll()
{
	for (UUserWidget* view : Views) view->SetVisibility(ESlateVisibility::Hidden);
	CurrentView = nullptr;
}

void AUIManager::HideCurrent()
{
	CurrentView->SetVisibility(ESlateVisibility::Hidden);
}

