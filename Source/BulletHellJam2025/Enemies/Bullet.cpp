#include "BulletHellJam2025/Enemies/Bullet.h"
#include "BulletHellJam2025/Player/PlayerCharacter.h"
#include "BulletHellJam2025/Enemies/BaseEnemy.h"

FBullet::FBullet() {}

FBullet::FBullet(int InstanceID, FVector F, float S, float LS, float hitDist, FString T) : ID(InstanceID), Forward(F), Speed(S), LifeSpan(LS), CollisionDist(hitDist), Tag(T), Life(0.0) {}

FBullet::~FBullet() {}
