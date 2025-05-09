#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Enemies/BaseEnemy.h"

FBullet::FBullet() {}

FBullet::FBullet(int InstanceID, FVector F, float S, float LS, float hitDist, float dmg, FString T, float Radius, FVector Center)
	: ID(InstanceID), Forward(F), Speed(S), LifeSpan(LS), CollisionDist(hitDist), Damage(dmg), Tag(T), Life(0.0), MovementRadius(Radius), CircleCenter(Center) 
{
	MovesInCircle = MovementRadius > 0;

	if (MovesInCircle)
	{
		FVector directionToCenter = -Forward.GetSafeNormal();
		FVector upVector = FVector::UpVector;

		float angle = FMath::Atan2(
			FVector::DotProduct(FVector::CrossProduct(FVector::ForwardVector, directionToCenter), upVector),
			FVector::DotProduct(FVector::ForwardVector, directionToCenter)
		);
		UE_LOG(LogTemp, Warning, TEXT("Degree Bullet Start: %f"), angle);
		CurrentAngle = FMath::RadiansToDegrees(angle);

		if (CurrentAngle < 0.f)
		{
			CurrentAngle += 360.f;
		}

		UE_LOG(LogTemp, Warning, TEXT("Current Bullet Angle is: %f"), CurrentAngle);
	}
}

FBullet::~FBullet() {}
