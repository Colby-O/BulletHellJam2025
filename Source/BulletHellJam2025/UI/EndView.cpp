#include "BulletHellJam2025/UI/EndView.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"

void UEndView::BackToMenu()
{
	Player->ReturnToMenu();
}

void UEndView::SetPlayer(APlayerCharacter* P)
{
	Player = P;
}
