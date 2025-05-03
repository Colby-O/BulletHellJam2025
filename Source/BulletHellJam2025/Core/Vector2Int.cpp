#include "BulletHellJam2025/Core/Vector2Int.h"

FVector2Int::FVector2Int() : X(0), Y(0) {}
FVector2Int::FVector2Int(int X, int Y) : X(X), Y(Y) {}
FVector2Int::~FVector2Int() {}

bool FVector2Int::operator==(const FVector2Int& Other) const
{
	return X == Other.X && Y == Other.Y;
}

bool FVector2Int::operator!=(const FVector2Int& Other) const 
{
	return !(*this == Other);
}

FString FVector2Int::ToString() const 
{
	return FString::Printf(TEXT("X=%d Y=%d"), X, Y);
}

uint32 GetTypeHash(const FVector2Int& Vector)
{
	return HashCombine(::GetTypeHash(Vector.X), ::GetTypeHash(Vector.Y));
}
