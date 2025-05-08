#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameManager.generated.h"

UCLASS()
class BULLETHELLJAM2025_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	class AGridManager* GridManager;
	class ABulletManager* BulletManager;
	class APlayerCharacter* Player;
	class ABoss* Boss;

	void RestartGame();
};
