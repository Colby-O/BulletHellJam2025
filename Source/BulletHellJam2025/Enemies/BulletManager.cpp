#include "BulletHellJam2025/Enemies/BulletManager.h"
#include "BulletHellJam2025/Enemies/Bullet.h"
#include "Components/InstancedStaticMeshComponent.h"
#include <Kismet/GameplayStatics.h>
#include "BulletHellJam2025/Enemies/BaseEnemy.h"
#include "BulletHellJam2025/Grid/GridManager.h"
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

	Player = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	GridManager = Cast<AGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGridManager::StaticClass()));

	InstancedMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (BaseMat) InstancedMesh->SetMaterial(0, BaseMat);

	if (BaseMesh) 
	{
		InstancedMesh->SetStaticMesh(BaseMesh);
	}
}

void ABulletManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ProcessCollisions();
	Update(DeltaTime);
}

void ABulletManager::SpawnBullet(FVector Location, FRotator Rotation, FVector Scale, FVector Forward, float Speed, float LifeSpan, FString Tag)
{
	FTransform transform;
	transform.SetLocation(Location);
	transform.SetRotation(Rotation.Quaternion());
	transform.SetScale3D(Scale);
	int instanceID = InstancedMesh->AddInstance(transform);
	Bullets.Add(FBullet(instanceID, Forward, Speed, LifeSpan, Tag));
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

		TArray<AActor*> actors;
		actors.Add(Player);
		for (ABaseEnemy* e : ABaseEnemy::Enemies) actors.Add(e);
		
		for (AActor* actor : actors) 
		{
			if (FVector::Dist(actor->GetActorLocation(), bulletTransform.GetLocation()) < CollisionDist)
			{
				UE_LOG(LogTemp, Warning, TEXT("Bullet Hit: %s"), *actor->GetName());
				if (actor->IsA(APlayerCharacter::StaticClass()) && bullet.Tag.Compare("Player", ESearchCase::IgnoreCase) != 0)
				{
					APlayerCharacter* player = Cast<APlayerCharacter>(actor);
					player->OnHit();
					DestroyBullet(bullet.ID, i);
					break;
				}
				else if (actor->IsA(ABaseEnemy::StaticClass()) && bullet.Tag.Compare("Player", ESearchCase::IgnoreCase) == 0)
				{
					ABaseEnemy* enemy = Cast<ABaseEnemy>(actor);
					enemy->Knockback(GetActorForwardVector());
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

