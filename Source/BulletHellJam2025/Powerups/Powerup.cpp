#include "BulletHellJam2025/Powerups/Powerup.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include <Kismet/GameplayStatics.h>

int APowerup::Count = 0;

APowerup::APowerup()
{
	PrimaryActorTick.bCanEverTick = true;

}

void APowerup::BeginPlay()
{
	Super::BeginPlay();

	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	UMaterialInterface* mat = HealthMat;

	if (PowerupType == EPowerupType::FireRate) mat = FireMat;
	else if (PowerupType == EPowerupType::Damage) mat = DmgMat;

	Mesh->SetMaterial(0, mat);

	Life = 0;

	FVector loc = GetActorLocation();
	loc.Z = 0;
	SetActorLocation(loc);

	APowerup::Count++;
}

void APowerup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Player->IsPaused) return; 

	Life += DeltaTime;
	if (Life > LifeSpan) 
	{
		APowerup::Count--;
		Destroy();
		return;
	}

	CheckForOverlap();
}

void APowerup::CheckForOverlap()
{
	FVector playerLoc = Player->GetActorLocation();
	playerLoc.Z = 0;

	FVector loc = GetActorLocation();
	loc.Z = 0;

	if (FVector::Dist(loc, playerLoc) < CollisionDist) 
	{
		if (FMath::FRand() < FailChance)
		{
			int attackType = FMath::RandRange(0, 2);

			if (BadSound && SoundMix)
			{
				UGameplayStatics::PushSoundMixModifier(this, SoundMix);
				UGameplayStatics::PlaySound2D(this, BadSound);
				UGameplayStatics::PopSoundMixModifier(this, SoundMix);
			}

			if (attackType == 0)
			{
				GridManager->RollOutAttack(GetActorLocation(), { FVector(1, 0 ,0), FVector(-1, 0 ,0), FVector(0, 1 ,0), FVector(0, -1 ,0) }, RollOutWidth, RollOutRate, TileFallDelay);
			}
			else if (attackType == 1)
			{
				GridManager->RadiusAttack(GetActorLocation(), (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal(), RadiusRate, TileFallDelay);
			}
			else if (attackType == 2)
			{
				GridManager->MeteoriteAttack(MeteoriteSize, MeteoriteGap, TileFallDelay);
			}
		}
		else 
		{
			if (GoodSound && SoundMix)
			{
				UGameplayStatics::PushSoundMixModifier(this, SoundMix);
				UGameplayStatics::PlaySound2D(this, GoodSound);
				UGameplayStatics::PopSoundMixModifier(this, SoundMix);
			}
		}

		if (PowerupType == EPowerupType::Health)
		{
			Player->TakeHealth(-HealAmount);
		}
		else if (PowerupType == EPowerupType::FireRate)
		{
			Player->IncreaseFireRateFor(FireRateIncreaseFactor, FireRateIncreaseDuration);
		}
		else if (PowerupType == EPowerupType::Damage)
		{
			Player->IncreaseDamageFor(DamageIncreaseFactor, DamageIncreaseDuration);
		}

		APowerup::Count--;
		Destroy();
	}
}

