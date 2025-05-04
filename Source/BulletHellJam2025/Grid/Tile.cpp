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
	Mesh = FindComponentByClass<UStaticMeshComponent>();
	Mat = Mesh->GetMaterial(1);
    GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
    if (GridManager) GridManager->RegisterTile(this);

	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Mat, this);
	if (DynMaterial)
	{
		DynMaterial->GetVectorParameterValue(FMaterialParameterInfo(TEXT("BaseColor")), DefaultColor);
	}
}

void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATile::TriggerFall()
{
	if (IsFalling) return;
	IsFalling = true;
	SetColor(FLinearColor::Red);
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ATile::StartFall, FallDelay, true);
}

void ATile::StartFall()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ATile::Fall, FallTime / 100.0, true);
}

void ATile::ResetTile()
{
	FVector pos = GetActorLocation();
	pos.Z += FallAmount / 100.0;

	if (pos.Z >= 0) 
	{
		SetColor(DefaultColor);

		pos.Z = 0;
		IsFalling = false;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
	}

	SetActorLocation(pos);
}

void ATile::Fall()
{
	FVector pos = GetActorLocation();
	pos.Z -= FallAmount / 100.0;

	if (pos.Z <= -FallAmount) 
	{
		pos.Z = -FallAmount;
		GetWorld()->GetTimerManager().ClearTimer(TimerHandler);
		GetWorld()->GetTimerManager().SetTimer(TimerHandler, this, &ATile::ResetTile, FallTime / 100.0, true);
	}

	SetActorLocation(pos);
}

void ATile::SetColor(FLinearColor Color)
{
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Mat, this);

	if (DynMaterial)
	{
		DynMaterial->SetVectorParameterValue("BaseColor", Color);
		Mesh->SetMaterial(1, DynMaterial);
	}
}

