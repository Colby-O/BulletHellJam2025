#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"
#include "Boss.generated.h"

UCLASS()
class BULLETHELLJAM2025_API ABoss : public AActor
{
	GENERATED_BODY()
	
public:	
	ABoss();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;
	class AUIManager* UIManager;

	UGameViewWidget* GameView;

	bool HasSetupHealth = false;

	float MaxHealth = 1000;
	float CurrentHealth;

	void SetHealth(float Health);
	void TakeHealth(float Amount);
	void ResetBoss();
};
