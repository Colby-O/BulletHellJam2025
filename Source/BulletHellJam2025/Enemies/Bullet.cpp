#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Enemies/BaseEnemy.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();	

	Mesh = FindComponentByClass<UStaticMeshComponent>();
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlap);

	GetWorld()->GetTimerManager().SetTimer(LifeHandler, this, &ABullet::Remove, LifeSpan, false);
}

void ABullet::SetLifeSpan(float Span)
{
	this->LifeSpan = Span;
	GetWorld()->GetTimerManager().ClearTimer(LifeHandler);
	GetWorld()->GetTimerManager().SetTimer(LifeHandler, this, &ABullet::Remove, this->LifeSpan, false);
}

void ABullet::SetFrom(FString Tag)
{
	FromTag = Tag;
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector loc = GetActorLocation();
	loc += BulletSpeed * DeltaTime * GetActorForwardVector();
	SetActorLocation(loc);
}

void ABullet::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		UE_LOG(LogTemp, Warning, TEXT("Triggered by: %s"), *OtherActor->GetClass()->GetName());
		if (OtherActor->IsA(APlayerCharacter::StaticClass()) && FromTag.Compare("Player", ESearchCase::IgnoreCase) != 0)
		{
			APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor);
			player->OnHit();
		}
		else if (OtherActor->IsA(ABaseEnemy::StaticClass()) && FromTag.Compare("Player", ESearchCase::IgnoreCase) == 0)
		{
			ABaseEnemy* enemy = Cast<ABaseEnemy>(OtherActor);
			enemy->Knockback(GetActorForwardVector());
		}

		Destroy();
	}
}

void ABullet::Remove()
{
	Destroy();
}

