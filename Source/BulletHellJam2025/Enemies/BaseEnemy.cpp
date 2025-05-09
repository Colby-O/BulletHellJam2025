#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Enemies/ShooterComponent.h"
#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Enemies/ShootPattern.h"
#include "BulletHellJam2025/Grid/Tile.h"
#include "BulletHellJam2025/GameManager.h"
#include <Kismet/GameplayStatics.h>

TArray<ABaseEnemy*> ABaseEnemy::Enemies;

ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
}

ABaseEnemy::~ABaseEnemy()
{
	ABaseEnemy::Enemies.Remove(this);
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	ABaseEnemy::Enemies.Add(this);

	GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	FVector currenLoc = GetActorLocation();
	currenLoc.Z = Player->GetActorLocation().Z;
	SetActorLocation(currenLoc);

	CurrentState = EMovementState::Idle;
	LastState = EMovementState::Idle;

	if (!ShooterComp) ShooterComp = FindComponentByClass<UShooterComponent>();
	ShooterComp->SetFrom("Enemy");
	FActorSpawnParameters spawnParams;

	IsMarkedForRemoval = false;

	UE_LOG(LogTemp, Warning, TEXT("Attack Range: %f"), AttackRange);
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AttackRange = ShooterComp->SelectedPattern.Distance;

	if (IsKnockingBack) return;

	if (!ClosestEnemy && ABaseEnemy::Enemies.Num() > 1) 
	{
		ClosestEnemy = GetClosestEnemy();
		HoldPosition = false;
	}

	FVector rawDirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	FVector directionToPlayer = FVector(rawDirectionToPlayer.X, rawDirectionToPlayer.Y, 0);
	SetActorRotation(directionToPlayer.Rotation());

	LocateTarget();
	HandleAttack();
	HandleRotation();
	HandleMovement(DeltaTime);
	CheckForDeath();
}

void ABaseEnemy::DestroyAllEnemies()
{
	for (int i = ABaseEnemy::Enemies.Num() - 1; i >= 0; i--) 
	{
		ABaseEnemy* e = ABaseEnemy::Enemies[i];
		e->IsMarkedForRemoval = true;
	}
}

void ABaseEnemy::DestroyEnemy(ABaseEnemy* Enemy)
{
	for (int i = ABaseEnemy::Enemies.Num() - 1; i >= 0; i--)
	{
		ABaseEnemy* e = ABaseEnemy::Enemies[i];
		if (e == Enemy) 
		{
			e->Destroy();
			ABaseEnemy::Enemies.RemoveAt(i);
			return;
		}
	}
}

FVector ABaseEnemy::GetDirectionToPlayer()
{
	FVector rawDirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	return FVector(rawDirectionToPlayer.X, rawDirectionToPlayer.Y, 0);
}

FVector ABaseEnemy::GetDirectionToNextTile()
{
	if (CurrentPath.IsEmpty()) return FVector::ZeroVector;
	ATile* next = CurrentPath[0];
	if (!next) return FVector::ZeroVector;
	FVector rawDirection = (next->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	return FVector(rawDirection.X, rawDirection.Y, 0);
}

float ABaseEnemy::GetDistToPlayer() 
{
	FVector cur = GetActorLocation();
	cur = FVector(cur.X, cur.Y, 0);

	FVector playerCur = Player->GetActorLocation();
	playerCur = FVector(playerCur.X, playerCur.Y, 0);

	return FVector::Dist(cur, playerCur);
}

void ABaseEnemy::UpdateState()
{
	LastState = CurrentState;

	FVector2Int curLoc = GridManager->WorldToGrid(GetActorLocation());
	ATile* currentTile = GridManager->GetTileAt(curLoc);

	if (currentTile && currentTile->IsFalling)
	{
		CurrentState = EMovementState::MovingOffTile;
	}
	else if (ClosestEnemy && ClosestEnemy->HoldPosition == true && HoldPosition == false)
	{
		CurrentState = EMovementState::MovingAwayFromOther;
	}
	else if (!IsAttacking && GetDistToPlayer() <= VisionRange && GetDistToPlayer() >= AttackRange)
	{
		CurrentState = EMovementState::Chasing;
	}
	else if (ClosestEnemy && FVector::Dist(GetActorLocation(), ClosestEnemy->GetActorLocation()) < Spacing)
	{
		CurrentState = EMovementState::MovingAwayFromOther;
	}
	else 
	{
		HoldPosition = false;
		CurrentState = EMovementState::Idle;
	}
}

void ABaseEnemy::LocateTarget()
{
	UpdateState();

	if (DebugMovement) for (ATile* t : CurrentPath) t->SetColor(t->IsFalling ? FLinearColor::Red : t->DefaultColor);

	UEnum* EnumPtr = StaticEnum<EMovementState>();
	if (EnumPtr)
	{
		FString EnumName = EnumPtr->GetNameStringByValue((int64)CurrentState);
		FString EnumName2 = EnumPtr->GetNameStringByValue((int64)CurrentState);
		UE_LOG(LogTemp, Log, TEXT("Current State: %s"), *EnumName);
		UE_LOG(LogTemp, Log, TEXT("Last State: %s"), *EnumName2);
	}

	if (CurrentState != EMovementState::Chasing) GetWorld()->GetTimerManager().ClearTimer(ChaseTimerHandle);

	if (CurrentState != LastState)
	{
		CurrentPath.Empty();
	}

	FVector2Int curLoc = GridManager->WorldToGrid(GetActorLocation());
	ATile* currentTile = GridManager->GetTileAt(curLoc);

	if (CurrentState == EMovementState::Chasing)
	{
		if (CurrentState != LastState) 
		{
			GetWorld()->GetTimerManager().ClearTimer(ChaseTimerHandle);
			RelocatePlayer();
			GetWorld()->GetTimerManager().SetTimer(ChaseTimerHandle, this, &ABaseEnemy::RelocatePlayer, Memory, true);
		}
		else if (!CurrentPath.IsEmpty() && currentTile == CurrentPath[0])
		{
			CurrentPath.RemoveAt(0);
		}
		else if (CurrentPath.IsEmpty()) 
		{
			RelocatePlayer();
		}

		if (!CurrentPath.IsEmpty()) 
		{
			UE_LOG(LogTemp, Log, TEXT("Next Tile Loc: %s"), *(GridManager->WorldToGrid(CurrentPath[0]->GetActorLocation()).ToString()));
		}
	}
	else if (CurrentState == EMovementState::MovingOffTile)
	{
		FVector2Int safeLoc;
		if (GridManager->GetNearestSafeTile(curLoc, safeLoc))
		{
			CurrentPath.Empty();
			CurrentPath = GridManager->FindPath(curLoc, safeLoc, 0.5f);
		}
	}
	else if (CurrentState == EMovementState::MovingAwayFromOther)
	{
		if (ClosestEnemy && ClosestEnemy->HoldPosition && LastState != CurrentState)
		{
				HoldPosition = false;
				ClosestEnemy->HoldPosition = true;
				FVector2Int safeLoc;
				if (GridManager->GetNearestSafeTile(curLoc, safeLoc, Spacing / GridManager->TileSize + 1))
				{
					CurrentPath.Empty();
					CurrentPath = GridManager->FindPath(curLoc, safeLoc, SMALL_NUMBER);
				}
		}
		else if (ClosestEnemy && ClosestEnemy->HoldPosition && !CurrentPath.IsEmpty() && currentTile == CurrentPath[0])
		{
			CurrentPath.RemoveAt(0);
		}
		else if (ClosestEnemy && ClosestEnemy->HoldPosition && CurrentPath.IsEmpty())
		{
			HoldPosition = false;
			ClosestEnemy->HoldPosition = false;
			ClosestEnemy = GetClosestEnemy();
		}
	}

	if (!CurrentPath.IsEmpty() && GridManager->GetTileAt(curLoc) == CurrentPath[0]) CurrentPath.RemoveAt(0);
	if (DebugMovement) for (ATile* t : CurrentPath) t->SetColor(FLinearColor::Blue);
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
	if (GetDistToPlayer() <= VisionRange) ShooterComp->Enable();
	else ShooterComp->Disable();

	if (CurrentPath.Num() == 0) return;

	FVector direction = GetDirectionToNextTile();

	MoveTowards(DeltaTime, direction);

	FVector newDirection = GetDirectionToNextTile();

	if (!CurrentPath.IsEmpty() && (newDirection + direction).IsNearlyZero()) 
	{
		FVector tileLoc = CurrentPath[0]->GetActorLocation();
		SetActorLocation(FVector(tileLoc.X, tileLoc.Y, GetActorLocation().Z));
	}
	
}

void ABaseEnemy::MoveTowards(float DeltaTime, FVector Direction) 
{
	FVector loc = GetActorLocation();
	loc += Speed * DeltaTime * Direction;
	SetActorLocation(loc, true);
	if (CurrentState != EMovementState::MovingAwayFromOther) 
	{
		ClosestEnemy = GetClosestEnemy();
		HoldPosition = false;
	}
}

void ABaseEnemy::Knockback(FVector Direction)
{
	//if (IsKnockingBack) return;

	KnockbackStart = GetActorLocation();
	KnockbackEnd = KnockbackStart + Direction * KnockbackAmount;
	KnockbackEnd.Z = KnockbackStart.Z;

	KnockbackElapsed = 0.0f;
	IsKnockingBack = true;
	GetWorld()->GetTimerManager().SetTimer(KnockbackTimerHandle, this, &ABaseEnemy::KnockbackStep, TimeStep, true);
}

void ABaseEnemy::KnockbackStep()
{
	KnockbackElapsed += TimeStep;
	float alpha = KnockbackElapsed / KnockbackRate;
	float smoothedAlpha = FMath::InterpEaseOut(0.f, 1.f, alpha, 2.0f);
	FVector newLocation = FMath::Lerp(KnockbackStart, KnockbackEnd, smoothedAlpha);

	ATile* nextTile = GridManager->GetTileAt(GridManager->WorldToGrid(newLocation));

	if (!nextTile || (nextTile && nextTile->IsEnable)) 
	{
		SetActorLocation(newLocation, true);
		CheckForDeath();
	}
	
	if (alpha >= 1.0f || (nextTile && !nextTile->IsEnable))
	{
		IsKnockingBack = false;
		GetWorld()->GetTimerManager().ClearTimer(KnockbackTimerHandle);
	}
}

void ABaseEnemy::RelocatePlayer()
{
	if (DebugMovement) for (ATile* t : CurrentPath) t->SetColor(t->IsFalling ? FLinearColor::Red : t->DefaultColor);
	CurrentPath.Empty();
	FVector2Int curLoc = GridManager->WorldToGrid(GetActorLocation());
	CurrentPath = GridManager->FindPath(curLoc, GridManager->WorldToGrid(Player->GetActorLocation()), FMath::Max(AttackRange / GridManager->TileSize - 1, SMALL_NUMBER), GetTilesToIgnore());
	if (DebugMovement) for (ATile* t : CurrentPath) t->SetColor(FLinearColor::Blue);
}

void ABaseEnemy::CheckForDeath()
{
	if (IsMarkedForRemoval) DestroyEnemy(this);

	ATile* curTile = GridManager->GetTileAt(GridManager->WorldToGrid(GetActorLocation()));
	if (!curTile || curTile->HasFallen)
	{
		DestroyEnemy(this);
		if (ClosestEnemy)
		{
			ClosestEnemy->ClosestEnemy = ClosestEnemy->GetClosestEnemy();
			ClosestEnemy->HoldPosition = false;
		}
	}
}

TArray<FVector2Int> ABaseEnemy::GetTilesToIgnore()
{
	TArray<FVector2Int> tileToIgnore;

	for (ABaseEnemy* e : ABaseEnemy::Enemies)
	{
		if (e == this) continue;
		int numTiles = FMath::CeilToInt32(Spacing / GridManager->TileSize);
		for (int i = -FMath::FloorToInt32(numTiles / 2.0); i < FMath::FloorToInt32(numTiles / 2.0); i++)
		{
			for (int j = -FMath::FloorToInt32(numTiles / 2.0); j < FMath::FloorToInt32(numTiles / 2.0); j++)
			{
				tileToIgnore.Add(GridManager->WorldToGrid(e->GetActorLocation()) + FVector2Int(i, j));
			}
		}
	}
	return tileToIgnore;
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

