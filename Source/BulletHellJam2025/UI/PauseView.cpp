#include "BulletHellJam2025/UI/PauseView.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"

void UPauseView::BackToMenu()
{
	Player->ReturnToMenu();
}

void UPauseView::ResumeGame()
{
	Player->ResumeGame();
}

void UPauseView::SetPlayer(APlayerCharacter* P)
{
	Player = P;
}
