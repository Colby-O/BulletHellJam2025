#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletManager.generated.h"

UCLASS()
class BULLETHELLJAM2025_API ABulletManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ABulletManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	TArray<FBullet> Bullets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UStaticMesh* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	UMaterialInterface* BaseMat;

	class AGameManager* GameManager;
	class AGridManager* GridManager;
	class APlayerCharacter* Player;
	class ABoss* Boss;
	UInstancedStaticMeshComponent* InstancedMesh;
	UMaterialInstanceDynamic* DynamicMat;

	bool IsMarkedForReset = false;

	void SpawnBullet(FVector Location, FRotator Rotation, FVector Scale, FVector Forward, float Speed, float LifeSpan, float CollisionDist, float Damage, FLinearColor Color = FLinearColor::White, FString Tag = "");
	void Update(float DeltaTime);
	void ProcessCollisions();
	void DestroyBullet(int InstanceID, int Index);
	void SetBulletColor(FBullet Bullet, FLinearColor Color);
	void ResetBullets();
};
