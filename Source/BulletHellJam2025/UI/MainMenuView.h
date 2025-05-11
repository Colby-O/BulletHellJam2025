#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuView.generated.h"


UCLASS()
class BULLETHELLJAM2025_API UMainMenuView : public UUserWidget
{
	GENERATED_BODY()

public:

	class APlayerCharacter* Player;

	UFUNCTION(BlueprintCallable)
	void StartGame();

	void SetPlayer(APlayerCharacter* P);
	
};
