#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Enemies/Bullet.h"

UShooterComponent::UShooterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	RawRotation = GetRelativeRotation();

	Enable();
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

	if (!IsEnabled) return;

	RawRotation.Yaw += RotSpeed.X * DeltaTime;
	RawRotation.Pitch += RotSpeed.Y * DeltaTime;
	RawRotation.Roll += RotSpeed.Z * DeltaTime;

	RawRotation.Yaw = ClampAngle(RawRotation.Yaw, RotMin.X, RotMax.X, YawLoop, LoopFrequency.X);
	RawRotation.Pitch = ClampAngle(RawRotation.Pitch, RotMin.Y, RotMax.Y, PitchLoop, LoopFrequency.Y);
	RawRotation.Roll = ClampAngle(RawRotation.Roll, RotMin.Z, RotMax.Z, RollLoop, LoopFrequency.Z);
	SetRelativeRotation(RawRotation);
}

void UShooterComponent::Shoot()
{
	if (BulletClass)
	{
		for (const FVector& dir : SpawnDirections) 
		{
			FVector spawnLoc = GetComponentLocation() + GetComponentRotation().RotateVector(dir.GetSafeNormal()) * 100.0;
			FRotator spawnRot = GetComponentRotation().RotateVector(dir.GetSafeNormal()).Rotation();

			FActorSpawnParameters spawnParams;
			ABullet* Bullet = Cast<ABullet>(GetWorld()->SpawnActor<AActor>(BulletClass, spawnLoc, spawnRot, spawnParams));
			Bullet->SetLifeSpan(LifeSpan);
		}
	}
}

void UShooterComponent::Enable() 
{
	if (IsEnabled) return;
	IsEnabled = true;
	GetOwner()->GetWorldTimerManager().ClearTimer(FireTimerHandler);
	GetOwner()->GetWorldTimerManager().SetTimer(FireTimerHandler, this, &UShooterComponent::Shoot, FireRate, true);
}

void UShooterComponent::Disable() 
{
	if (!IsEnabled) return;
	IsEnabled = false;
	GetOwner()->GetWorldTimerManager().ClearTimer(FireTimerHandler);
}

