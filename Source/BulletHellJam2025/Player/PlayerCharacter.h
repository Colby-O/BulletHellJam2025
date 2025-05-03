#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class BULLETHELLJAM2025_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class AGridManager* GridManager;

	class UTapHandler* TapHandler;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PlayerSpeed = 1000.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpForce = 1000.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashForce = 1000.0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DashCooldown = 0.25;

	float Width;

	void MoveForward(float Input);
	void MoveRight(float Input);
	void Dash(FVector Direction);
	void OnWPressed();
	void OnSPressed();
	void OnAPressed();
	void OnDPressed();
	void OnShiftPressed();
	void CheckTile(FVector pos);
};
