#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseView.generated.h"

/**
 * 
 */
UCLASS()
class BULLETHELLJAM2025_API UPauseView : public UUserWidget
{
	GENERATED_BODY()

public:

	class APlayerCharacter* Player;

	UFUNCTION(BlueprintCallable)
	void BackToMenu();

	UFUNCTION(BlueprintCallable)
	void ResumeGame();

	void SetPlayer(APlayerCharacter* P);
	
};
