#pragma once

#include "CoreMinimal.h"
#include "CMenuWidget.h"
#include "CMainMenu.generated.h"

UCLASS()
class GAME_API UCMainMenu : public UCMenuWidget
{
	GENERATED_BODY()
public:
	virtual bool Initialize() override;

private:
	UFUNCTION()
		void HostServer();
	UFUNCTION()
		void JoinServer();
	UFUNCTION()
		void QuitPressed();
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
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(meta = (BindWidget))
		class UWidget* JoinMenu;	// 오버레이는 타입이 뭐더라
	UPROPERTY(meta = (BindWidget))
		class UWidget* MainMenu;	// 오버레이는 타입이 뭐더라
	// Switcher 1번
	UPROPERTY(meta = (BindWidget))
		class UButton* CancelJoinMenuButton;
	UPROPERTY(meta = (BindWidget))
		class UEditableTextBox* IPAddressField;	// 주소 입력창
	UPROPERTY(meta = (BindWidget))
		class UButton* ComfirmJoinMenuButton;
	// 레벨 BP에 BeginPlay 에서 MainMenu 를 실행하기에, BeginPlay에서 바인딩 해야한다. 
};