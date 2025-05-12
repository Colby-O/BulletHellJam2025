#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndView.generated.h"


UCLASS()
class BULLETHELLJAM2025_API UEndView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	class APlayerCharacter* Player;

	UFUNCTION(BlueprintCallable)
	void BackToMenu();

	void SetPlayer(APlayerCharacter* P);
};
