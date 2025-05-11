#include "BulletHellJam2025/UI/MainMenuView.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"

void UMainMenuView::StartGame()
{
	Player->StartGame();
}

void UMainMenuView::SetPlayer(APlayerCharacter* P)
{
	Player = P;
}
