#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include <Kismet/GameplayStatics.h>

TArray<ABaseEnemy*> ABaseEnemy::Enemies;

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	ABaseEnemy::Enemies.Add(this);
}

ABaseEnemy::~ABaseEnemy()
{
	ABaseEnemy::Enemies.Remove(this);
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	if (!ShooterComp) ShooterComp = FindComponentByClass<UShooterComponent>();

	FActorSpawnParameters spawnParams;
	ABullet* Bullet = Cast<ABullet>(GetWorld()->SpawnActor<AActor>(ShooterComp->BulletClass, FVector::Zero(), FRotator::ZeroRotator, spawnParams));

	AttackRange = ShooterComp->LifeSpan * Bullet->BulletSpeed;

	Bullet->Destroy();

	UE_LOG(LogTemp, Warning, TEXT("Attack Range: %f"), AttackRange);
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector rawDirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector directionToPlayer = FVector(rawDirectionToPlayer.X, rawDirectionToPlayer.Y, 0);
	SetActorRotation(directionToPlayer.Rotation());

	float dstToClosestEnemy = ClosestEnemy ? FVector::Dist(GetActorLocation(), ClosestEnemy->GetActorLocation()) : BIG_NUMBER;

	HandleAttack();
	HandleRotation();
	HandleMovement(DeltaTime);
}

FVector ABaseEnemy::GetDirectionToPlayer()
{
	FVector rawDirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	return FVector(rawDirectionToPlayer.X, rawDirectionToPlayer.Y, 0);
}

float ABaseEnemy::GetDistToPlayer() 
{
	return FVector::Dist(GetActorLocation(), Player->GetActorLocation());
}

void ABaseEnemy::HandleAttack()
{
	float dstToPlayer = FVector::Dist(GetActorLocation(), Player->GetActorLocation());

	if (dstToPlayer <= AttackRange)
	{
		IsAttacking = true;
	}
	else
	{
		IsAttacking = false;
	}
}

void ABaseEnemy::HandleRotation()
{
	FVector directionToPlayer = GetDirectionToPlayer();
	SetActorRotation(directionToPlayer.Rotation());
}

void ABaseEnemy::HandleMovement(float DeltaTime) 
{
	FVector directionToPlayer = GetDirectionToPlayer();
	float dstToClosestEnemy = ClosestEnemy ? FVector::Dist(GetActorLocation(), ClosestEnemy->GetActorLocation()) : BIG_NUMBER;

	if (GetDistToPlayer() <= VisionRange) ShooterComp->Enable();
	else ShooterComp->Disable();

	if (!IsAttacking && GetDistToPlayer() <= VisionRange)
	{
		MoveTowards(DeltaTime, directionToPlayer);

	}
	else if (dstToClosestEnemy <= Spacing)
	{
		FVector directionAwayFromClosest = (GetActorLocation() - ClosestEnemy->GetActorLocation()).GetSafeNormal();
		MoveTowards(DeltaTime, directionAwayFromClosest);
	}
}

void ABaseEnemy::MoveTowards(float DeltaTime, FVector Direction) 
{
	FVector loc = GetActorLocation();
	loc += Speed * DeltaTime * Direction;
	SetActorLocation(loc);
	ClosestEnemy = GetClosestEnemy();
}

ABaseEnemy* ABaseEnemy::GetClosestEnemy() 
{
	float closestDst = BIG_NUMBER;
	ABaseEnemy* closest = nullptr;
	for (ABaseEnemy* e : ABaseEnemy::Enemies)
	{
		if (e != this) 
		{
			float dst = FVector::Dist(GetActorLocation(), e->GetActorLocation());
			if (dst < closestDst) closest = e;
		}
	}
	return closest;
}

