#include "BulletHellJam2025/Enemies/Bullet.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ABullet::BeginPlay()
{
	Super::BeginPlay();	

	GetWorld()->GetTimerManager().SetTimer(LifeHandler, this, &ABullet::Remove, LifeSpan, false);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector loc = GetActorLocation();
	loc += BulletSpeed * DeltaTime * GetActorForwardVector();
	SetActorLocation(loc);
}

void ABullet::Remove()
{
	Destroy();
}

