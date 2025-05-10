#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Enemies/BulletManager.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Enemies/Boss.h"
#include <Kismet/GameplayStatics.h>

UShooterComponent::UShooterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	BulletManager = Cast<ABulletManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ABulletManager::StaticClass()));
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	RawRotation = GetRelativeRotation();
	StartRotation = GetRelativeRotation();

	InitShootPattern();

	if (ShootPatterns.Num() > 0) 
	{
		SelectedPatternIndex = 0;
		SelectedPattern = ShootPatterns[0];
	}

	VelPrediction = FVector::ZeroVector;

	Enable();
}

void UShooterComponent::SetShootPattern(TArray<FShootPattern> Patterns)
{
	BulletManager->IsMarkedToRemoveBossBullets = true;
	IsMarkedToStartNewPattern = true;
	ShootPatterns.Empty();
	ShootPatterns = TArray<FShootPattern>(Patterns);
	InitShootPattern();
	UE_LOG(LogTemp, Warning, TEXT("Setting Shoot Pattern. Number of Patterns Is: %d"), ShootPatterns.Num());
}

float UShooterComponent::NormalizeAngle360(float Angle)
{
	float Result = FMath::Fmod(Angle, 360.f);
	return (Result < 0.f) ? Result + 360.f : Result;
}

float UShooterComponent::ClampAngle(float Angle, float Min, float Max, bool CanLoop, float Frequency)
{
	float Time = GetWorld()->GetTimeSeconds(); 
	float newAngle = FMath::Lerp(Min, Max, 0.5f * (FMath::Sin(Time * Frequency * 2 * PI) + 1));

	return CanLoop ? FMath::Lerp(Min, Max, 0.5f * (FMath::Sin(Time * Frequency * 2 * PI) + 1)) : NormalizeAngle360(Angle);
}

void UShooterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsMarkedToStartNewPattern)
	{
		IsMarkedToStartNewPattern = false;
		ResetShooter();
	}

	if (!IsEnabled || ShootPatterns.Num() == 0) return;

	Timer += DeltaTime;

	if (Timer > SelectedPattern.Duration && ShootPatterns.Num() > 1) NextPattern();

	RawRotation.Yaw += SelectedPattern.RotSpeed.X * DeltaTime;
	RawRotation.Pitch += SelectedPattern.RotSpeed.Y * DeltaTime;
	RawRotation.Roll += SelectedPattern.RotSpeed.Z * DeltaTime;

	RawRotation.Yaw = ClampAngle(RawRotation.Yaw, SelectedPattern.RotMin.X, SelectedPattern.RotMax.X, SelectedPattern.YawLoop, SelectedPattern.LoopFrequency.X);
	RawRotation.Pitch = ClampAngle(RawRotation.Pitch, SelectedPattern.RotMin.Y, SelectedPattern.RotMax.Y, SelectedPattern.PitchLoop, SelectedPattern.LoopFrequency.Y);
	RawRotation.Roll = ClampAngle(RawRotation.Roll, SelectedPattern.RotMin.Z, SelectedPattern.RotMax.Z, SelectedPattern.RollLoop, SelectedPattern.LoopFrequency.Z);
	SetRelativeRotation(RawRotation);
}

void UShooterComponent::Shoot(FVector Vel)
{
	UE_LOG(LogTemp, Warning, TEXT("Shoot Was Called!"));

	if (SelectedPattern.IsGridPattern() && !SelectedPattern.HasRanGridPattern) 
	{
		if (Boss) Boss->PlayStompAnimation();

		if (SelectedPattern.IsRollOutGridPattern) GridManager->RollOutAttack(GetComponentLocation(), SelectedPattern.SpawnDirections, SelectedPattern.RollOutWidth, SelectedPattern.RollOutRate, SelectedPattern.TileFallDelay);
		else if (SelectedPattern.IsMeteoriteGridPattern) GridManager->MeteoriteAttack(SelectedPattern.MeteoriteSize, SelectedPattern.MeteoriteGap, SelectedPattern.TileFallDelay);
		else GridManager->RadiusAttack(GetComponentLocation(), (Player->GetActorLocation() - GetComponentLocation()).GetSafeNormal(), SelectedPattern.RadiusRate, SelectedPattern.TileFallDelay);
		SelectedPattern.HasRanGridPattern = true;
		return;
	}

	for (const FVector& dir : SelectedPattern.SpawnDirections)
	{
		FVector spawnLoc = GetComponentLocation() + GetComponentRotation().RotateVector(dir.GetSafeNormal()) * Offset;
		FVector forward = GetComponentRotation().RotateVector(dir.GetSafeNormal());
		FRotator spawnRot = forward.Rotation();

		float speed = SelectedPattern.Speed + FMath::Max(Vel.Dot(forward), 0);

		UE_LOG(LogTemp, Warning, TEXT("Bullet Spawning With Life Span %f"), SelectedPattern.LifeSpan);

		if (SelectedPattern.MovesStationary)
		{
			for (float radius = 0; radius < SelectedPattern.MovesStationaryRadius; radius += SelectedPattern.MoveStationarySpacing)
			{
				BulletManager->SpawnBullet(spawnLoc + forward * radius, spawnRot, Scale, forward, speed, FMath::Max(SelectedPattern.Duration, 0.0f), CollisionDist, Damage, BulletColor, FromTag, radius, GetComponentLocation());
			}
		}
		else
		{
			BulletManager->SpawnBullet(spawnLoc, spawnRot, Scale, forward, speed, SelectedPattern.LifeSpan, CollisionDist, Damage, BulletColor, FromTag);
		}
	}
}

void UShooterComponent::SetFrom(FString Tag)
{
	FromTag = Tag;
}

FVector UShooterComponent::GetShootDirection(int index)
{
	if (index >= SelectedPattern.SpawnDirections.Num()) return FVector::ZeroVector;
	return GetComponentRotation().RotateVector(SelectedPattern.SpawnDirections[index]).GetSafeNormal();
}

void UShooterComponent::SetBoss(ABoss* B)
{
	Boss = B;
}

void UShooterComponent::NextPattern()
{
	if (ShootPatterns.Num() == 0) return;
	Disable(true);
	if (Boss) Boss->Open();
	SelectedPattern.HasRanGridPattern = false;
	SelectedPatternIndex = (SelectedPatternIndex + 1) % ShootPatterns.Num();
	SelectedPattern = ShootPatterns[SelectedPatternIndex];
	Enable(true);
}

void UShooterComponent::ResetShooter()
{
	Disable(true);
	Timer = 0;
	SelectedPattern.HasRanGridPattern = false;
	GridManager->StopAttack();
	if (ShootPatterns.Num() > 0) 
	{
		SelectedPatternIndex = 0;
		SelectedPattern = ShootPatterns[0];
	}
	Enable(true);
}

void UShooterComponent::ShootInternal()
{
	Shoot(VelPrediction);
}

void UShooterComponent::Enable(bool Force) 
{
	if (IsEnabled && !Force) return;
	IsEnabled = true;

	Timer = 0;
	GetOwner()->GetWorldTimerManager().ClearTimer(FireTimerHandler);
	if (!SelectedPattern.MovesStationary && !SelectedPattern.IsGridPattern()) GetOwner()->GetWorldTimerManager().SetTimer(FireTimerHandler, this, &UShooterComponent::ShootInternal, SelectedPattern.FireRate, true);
	else GetOwner()->GetWorldTimerManager().SetTimer(FireTimerHandler, this, &UShooterComponent::ShootInternal, 0.01f, false);
}

void UShooterComponent::Disable(bool Force, bool DestoryAll)
{
	if (!IsEnabled) return;
	IsEnabled = false;

	if (DestoryAll) BulletManager->IsMarkedToRemoveBossBullets = true;

	Timer = 0;
	GetOwner()->GetWorldTimerManager().ClearTimer(FireTimerHandler);
	if (ResetRotation) 
	{
		SetRelativeRotation(StartRotation);
		RawRotation = StartRotation;
	}
}

void UShooterComponent::InitShootPattern()
{
	int i = 0;
	for (FShootPattern& p : ShootPatterns)
	{
		p.ID = i++;
		p.Awake();
		p.HasRanGridPattern = false;
	}
}
