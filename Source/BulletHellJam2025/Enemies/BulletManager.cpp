#include "BulletHellJam2025/Enemies/BulletManager.h"
#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletHellJam2025/GameManager.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Grid/GridManager.h"
#include "BulletHellJam2025/Grid/Tile.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"

ABulletManager::ABulletManager()
{
	PrimaryActorTick.bCanEverTick = true;

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Meshes"));
	InstancedMesh->SetupAttachment(RootComponent);
}

void ABulletManager::BeginPlay()
{
	Super::BeginPlay();

	IsMarkedForReset = false;

	GameManager = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));

	FVector currenLoc = GetActorLocation();
	currenLoc.Z = Player->GetActorLocation().Z;
	SetActorLocation(currenLoc);

	if (BaseMat) 
	{
		InstancedMesh->NumCustomDataFloats = 3;
		DynamicMat = UMaterialInstanceDynamic::Create(BaseMat, this);
		InstancedMesh->SetMaterial(0, DynamicMat);
	}
	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (BaseMesh) 
	{
		InstancedMesh->SetStaticMesh(BaseMesh);
	}
}

void ABulletManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsMarkedForReset) ResetBullets();
	ProcessCollisions();
	Update(DeltaTime);
}

void ABulletManager::SetBulletColor(FBullet Bullet, FLinearColor Color)
{
	InstancedMesh->SetCustomDataValue(Bullet.ID, 0, Color.R, false);
	InstancedMesh->SetCustomDataValue(Bullet.ID, 1, Color.G, false);
	InstancedMesh->SetCustomDataValue(Bullet.ID, 2, Color.B, true);
}

void ABulletManager::ResetBullets()
{
	for (int i = Bullets.Num() - 1; i >= 0; i--)
	{
		FBullet& bullet = Bullets[i];
		DestroyBullet(bullet.ID, i);
	}
	IsMarkedForReset = false;
}

void ABulletManager::SpawnBullet(FVector Location, FRotator Rotation, FVector Scale, FVector Forward, float Speed, float LifeSpan, float CollisionDist, FLinearColor Color, FString Tag)
{
	FTransform transform;
	transform.SetLocation(Location);
	transform.SetRotation(Rotation.Quaternion());
	transform.SetScale3D(Scale);
	int instanceID = InstancedMesh->AddInstance(transform);
	FBullet bullet = FBullet(instanceID, Forward, Speed, LifeSpan, CollisionDist, Tag);
	Bullets.Add(bullet);
	SetBulletColor(bullet, Color);
	UE_LOG(LogTemp, Warning, TEXT("Spawning Bullet At: %s"), *transform.GetLocation().ToString());
}

void ABulletManager::Update(float DeltaTime)
{
	for (int i = Bullets.Num() - 1; i >= 0; i--) 
	{
		FBullet& bullet = Bullets[i];
		int instanceID = bullet.ID;
		float speed = bullet.Speed;
		float lifeSpan = bullet.LifeSpan;
		float life = bullet.Life;

		if (life > lifeSpan) 
		{
			DestroyBullet(instanceID, i);
			continue;
		}

		FTransform transform;
		if (InstancedMesh->GetInstanceTransform(instanceID, transform, true))
		{
			transform.SetLocation(speed * DeltaTime * bullet.Forward + transform.GetLocation());
			InstancedMesh->UpdateInstanceTransform(instanceID, transform, true, true);
		}

		bullet.Life += DeltaTime;
	}
}

void ABulletManager::ProcessCollisions()
{
	for (int i = Bullets.Num() - 1; i >= 0; i--)
	{
		FBullet& bullet = Bullets[i];

		FTransform bulletTransform;
		InstancedMesh->GetInstanceTransform(bullet.ID, bulletTransform, true);
		FVector bulletLoc = bulletTransform.GetLocation();

		ATile* bulletTile = GridManager->GetTileAt(GridManager->WorldToGrid(bulletLoc));

		if (!bulletTile || !bulletTile->IsEnable) continue;

		TArray<AActor*> actors;
		actors.Add(Player);
		for (ABaseEnemy* e : ABaseEnemy::Enemies) actors.Add(e);
		
		bulletLoc.Z = 0;
		for (AActor* actor : actors) 
		{
			FVector actorLoc = actor->GetActorLocation();
			actorLoc.Z = 0;
			if (FVector::Dist(actorLoc, bulletLoc) < bullet.CollisionDist)
			{
				UE_LOG(LogTemp, Warning, TEXT("Bullet Hit: %s"), *actor->GetName());
				if (actor->IsA(APlayerCharacter::StaticClass()) && bullet.Tag.Compare("Player", ESearchCase::IgnoreCase) != 0)
				{
					APlayerCharacter* player = Cast<APlayerCharacter>(actor);
					player->OnHit();
					UE_LOG(LogTemp, Warning, TEXT("Player Hit!"));
					DestroyBullet(bullet.ID, i);
					break;
				}
				else if (actor->IsA(ABaseEnemy::StaticClass()) && bullet.Tag.Compare("Player", ESearchCase::IgnoreCase) == 0)
				{
					ABaseEnemy* enemy = Cast<ABaseEnemy>(actor);
					enemy->Knockback(bullet.Forward);
					DestroyBullet(bullet.ID, i);
					break;
				}
			}
		}
	}
}

void ABulletManager::DestroyBullet(int InstanceID, int Index)
{
	InstancedMesh->RemoveInstance(InstanceID);
	Bullets.RemoveAt(Index);
	for (int j = Index; j < Bullets.Num(); j++) Bullets[j].ID -= 1;
}

