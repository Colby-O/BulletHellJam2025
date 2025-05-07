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
	UPROPERTY(EditAnywhere, Category = "References")
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UStaticMeshComponent* PlayerMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UStaticMeshComponent* GunMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class AGridManager* GridManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableTileFall = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableDebugMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableCornerFall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PlaneHeight = 60.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PlayerSpeed = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float JumpForce = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SpeedLimitInAir = 300.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float GravityScale = 2.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashForce = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float DashCooldown = 0.25;

	class UTapHandler* TapHandler;
	FTimerHandle DashTimeHandle;
	APlayerController* Controller;
	float PlayerWidth;
	float PlayerHeight;
	bool IsDashing;

	void MoveForward(float Input);
	void MoveRight(float Input);
	void Dash(FVector Direction);
	bool CanDash();
	void StopDashing();
	void DashForward();
	void DashBackward();
	void DashLeft();
	void DashRight();
	void DashMoveDirection();
	void Shoot();
	void CheckTile(FVector pos);
	void UpdatePlayerRotation();
	void LimitSpeed();
	void SetCursor();
	void OnDeath();

public:
	void OnHit();
};
