#include "BulletHellJam2025/Enemies/ShootPattern.h"

FShootPattern::FShootPattern()
{

}

FShootPattern::~FShootPattern()
{
}

void FShootPattern::Awake()
{
	LifeSpan = !FMath::IsNearlyZero(Speed) ? Distance / Speed : 0;
	UE_LOG(LogTemp, Warning, TEXT("Set BulletLife Span To %f from %f / %f"), LifeSpan, Distance, Speed);
}

bool FShootPattern::IsGridPattern()
{
	return IsRollOutGridPattern || IsRadiusGridPattern || IsMeteoriteGridPattern;
}

bool FShootPattern::operator==(const FShootPattern& Other)
{
	return ID == Other.ID;
}

