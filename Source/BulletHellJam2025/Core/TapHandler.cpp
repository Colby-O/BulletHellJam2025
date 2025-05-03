#include "BulletHellJam2025/Core/TapHandler.h"

UTapHandler::UTapHandler(float TapDelay) : TapDelay(TapDelay)
{
}

UTapHandler::~UTapHandler()
{
}

void UTapHandler::AddListener(FName EventName, TFunction<void()> Callback)
{
	if (!Events.Contains(EventName))
	{
		Events.Add(EventName, FTapInfo());
	}
	Events[EventName].CallBack = Callback;
}

void UTapHandler::Invoke(FName EventName, UWorld* World)
{
	if (!Events.Contains(EventName))
	{
		Events.Add(EventName, FTapInfo());
	}

	FTapInfo& info = Events[EventName];

	if (info.IsTapped) 
	{
		info.IsTapped = false;
		World->GetTimerManager().ClearTimer(info.TimerHandle);
		if (info.CallBack) info.CallBack();
	}
	else 
	{
		info.IsTapped = true;
		FTimerDelegate ResetDelegate = FTimerDelegate::CreateLambda([this, EventName]() {
			if (Events.Contains(EventName)) 
			{
				Events[EventName].IsTapped = false;
			}
		});
		World->GetTimerManager().SetTimer(info.TimerHandle, ResetDelegate, TapDelay, false);
	}
}
