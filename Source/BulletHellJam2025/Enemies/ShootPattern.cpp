#include "BulletHellJam2025/Enemies/ShootPattern.h"

FShootPattern::FShootPattern()
{
}

FShootPattern::~FShootPattern()
{
}

bool FShootPattern::operator==(const FShootPattern& Other)
{
	return ID == Other.ID;
}
