#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Enemies/Bullet.h"

UShooterComponent::UShooterComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	GetOwner()->GetWorldTimerManager().SetTimer(FireTimerHandler, this, &UShooterComponent::Shoot, FireRate, true);
}

void UShooterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FRotator currentRot = GetRelativeRotation();
	currentRot.Yaw += RotSpeed * DeltaTime;
	SetRelativeRotation(currentRot);
}

void UShooterComponent::Shoot()
{
	if (BulletClass)
	{
		for (FVector dir : SpawnDirections) 
		{
			FVector spawnLoc = GetComponentLocation() + GetComponentRotation().RotateVector(dir.GetSafeNormal()) * 100.0;
			FRotator spawnRot = GetComponentRotation().RotateVector(dir.GetSafeNormal()).Rotation();

			FActorSpawnParameters spawnParams;
			ABullet* Bullet = Cast<ABullet>(GetWorld()->SpawnActor<AActor>(BulletClass, spawnLoc, spawnRot, spawnParams));
			Bullet->LifeSpan = LifeSpan;
		}
	}
}

