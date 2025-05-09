#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameViewWidget.generated.h"


UCLASS()
class BULLETHELLJAM2025_API UGameViewWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float CurrentBossHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float MaxBossHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float CurrentPlayerHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stats")
	float MaxPlayerHealth;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InOpacity = 1.0f;
};

