#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IMenuInterface.h"
#include "CMainMenu.generated.h"

UCLASS()
class GAME_API UCMainMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;
	FORCEINLINE void SetMenuInterface(IIMenuInterface* InMenuInterface) { MenuInterface = InMenuInterface; }

	void Setup();	// UI가 보일 때의 키 설정
	void Teardown();	// 플레이 모드일 때 키 설정
private:
	UFUNCTION()
		void HostServer();
	UFUNCTION()
		void OpenJoinMenu();
	UFUNCTION()
		void OpenMainMenu();

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* HostButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* JoinButton;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelJoinMenuButton;
	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;	// 오버레이는 타입이 뭐더라
	UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;	// 오버레이는 타입이 뭐더라

	// 레벨 BP에 BeginPlay 에서 MainMenu 를 실행하기에, BeginPlay에서 바인딩 해야한다. 
private:
	IIMenuInterface* MenuInterface;	// GameInstance를 저장받을 것
};