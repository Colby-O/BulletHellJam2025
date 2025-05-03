#include "BulletHellJam2025/Grid/Tile.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "Kismet/GameplayStatics.h"

ATile::ATile()
{
	PrimaryActorTick.bCanEverTick = true;

}

void ATile::BeginPlay()
{
	Super::BeginPlay();

    GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    if (GridManager) GridManager->RegisterTile(this);
}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

