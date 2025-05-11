#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "BulletHellJam2025/UI/GameViewWidget.h"
#include "BulletHellJam2025/UI/MainMenuView.h"
#include "BulletHellJam2025/UI/PauseView.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "References")
	TSubclassOf<UMainMenuView> MainMenuViewClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "References")
	TSubclassOf<UPauseView> PauseViewClass;

	class APlayerCharacter* Player;

	TArray<UUserWidget*> Views;
	UUserWidget* CurrentView;

	void HideAll();
	void HideCurrent();

	template<WidgetType T>
	bool CurrentViewIsA()
	{
		return CurrentView ? CurrentView->IsA(T::StaticClass()) : false;
	}

	template<WidgetType T>
	void ShowView()
	{
		CurrentView = nullptr;
		for (UUserWidget* view : Views)
		{
			if (view->IsA(T::StaticClass())) 
			{
				CurrentView = view;
				return view->SetVisibility(ESlateVisibility::Visible);
			}
		}

	}

	template<WidgetType T>
	void AddView(T* View) 
	{
		if (View) 
		{
			View->AddToViewport();
			Views.Add(View);
		}
	}

	template<WidgetType T>
	T* GetView()
	{
		for (UUserWidget* view : Views)
		{
			if (view->IsA(T::StaticClass())) return Cast<T>(view);
		}
		return nullptr;
	}
};
