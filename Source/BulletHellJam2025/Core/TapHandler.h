#pragma once

#include "CoreMinimal.h"

struct FTapInfo 
{
	bool IsTapped = false;
	FTimerHandle TimerHandle;
	TFunction<void()> CallBack;
};

class BULLETHELLJAM2025_API UTapHandler
{
public:
	UTapHandler(float TapDelay);
	~UTapHandler();

	void AddListener(FName EventName, TFunction<void()> Callback);
	void Invoke(FName EventName, UWorld* World);

private:
	TMap<FName, FTapInfo> Events;
	float TapDelay;
};
