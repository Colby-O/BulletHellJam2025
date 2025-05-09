#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"
#include "UIManager.generated.h"

template<typename T>
concept WidgetType = std::is_base_of<UUserWidget, T>::value;


UCLASS()
class BULLETHELLJAM2025_API AUIManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AUIManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "References")
	TSubclassOf<UGameViewWidget> GameViewClass;

	TArray<UUserWidget*> Widgets;

	template<WidgetType T>
	void AddView(T* Widget) 
	{
		if (Widget) 
		{
			Widget->AddToViewport();
			Widgets.Add(Widget);
		}
	}

	template<WidgetType T>
	T* GetView()
	{
		for (UUserWidget* widget : Widgets)
		{
			if (widget->IsA(T::StaticClass())) return Cast<T>(widget);
		}
		return nullptr;
	}
};
