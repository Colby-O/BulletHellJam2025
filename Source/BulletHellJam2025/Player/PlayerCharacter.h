 #pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"
#include "BulletHellJam2025/UI/MainMenuView.h"
#include "BulletHellJam2025/Enemies/Bullet.h"
#include "Sound/SoundMix.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UShooterComponent* ShooterComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* DashSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* DeathSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* WinSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundMix* SoundMix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableTileFall = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableDebugMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableGodMode = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool EnableCornerFall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PlaneHeight = 60.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PlayerSpeed = 1000.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float PlayerControllerRotationSpeed = 1;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int DashCopies = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float BossRestartDelay = 1;

	float CurrentFireRate;
	FTimerHandle FireRateTimerHandle;

	float CurrentDamage;
	float BaseDamage;
	FTimerHandle DamageTimerHandle;

	FTimerHandle BossRestartHandle;

	class AGameManager* GameManager;
	class AGridManager* GridManager;
	class AUIManager* UIManager;
	class ABoss* Boss;

	UGameViewWidget* GameView;

	class UTapHandler* TapHandler;
	FTimerHandle DashTimeHandle;
	FTimerHandle DashCopyTimeHandle;
	APlayerController* Controller;
	FTransform StartTransform;
	float PlayerWidth;
	float PlayerHeight;
	bool IsDashing;
	bool HasMoved;
	bool IsFiring = false;
	float FiringRate = 0.1;

	bool IsUsingGamepad;
	FVector RightStickInput;

	void MoveForward(float Input);
	void MoveRight(float Input);
	void LookForward(float Input);
	void LookRight(float Input);
	void Dash(FVector Direction);
	bool CanDash();
	void StopDashing();
	void DashForward();
	void DashBackward();
	void DashLeft();
	void DashRight();
	void DashMoveDirection();
	void AddDashCopy();
	void Shoot();
	void StartShoot();
	void StopShoot();
	void CheckTile(FVector pos);
	void UpdatePlayerRotation(float DeltaTime);
	void LimitSpeed();
	void SetCursor(bool State = true);
	void OnDeath();
	void SetHealth(float Health);
	void CheckIfUsingGamepad();
	void RestartBoss();
	void TogglePause();
	void RevertFiringRate();
	void RevertDamage();

public:
	bool IsPaused = true;

	void StartGame();
	void OnWin();
	void PauseGame();
	void ResumeGame();
	void ReturnToMenu();
	void IncreaseFireRateFor(float Mul, float Duration);
	void IncreaseDamageFor(float Mul, float Duration);
	void OnHit(const FBullet& Bullet);
	void TakeHealth(float Amount);
	void ResetPlayer();
};
