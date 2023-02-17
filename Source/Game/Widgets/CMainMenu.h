#pragma once

#include "CoreMinimal.h"
#include "CMenuWidget.h"
#include "CMainMenu.generated.h"

UCLASS()
class GAME_API UCMainMenu : public UCMenuWidget
{
	GENERATED_BODY()
public:
	UCMainMenu(const FObjectInitializer& ObjectInitializer);	// 생성자

	virtual bool Initialize() override;
	void SetServerList(TArray<FString> InServerNames);
	void SetSelectedIndex(uint32 Index);
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
		class UButton* ComfirmJoinMenuButton;
	// 레벨 BP에 BeginPlay 에서 MainMenu 를 실행하기에, BeginPlay에서 바인딩 해야한다. 
	UPROPERTY(meta = (BindWidget))
		class UPanelWidget* ServerList;	// 주소 입력창

private: // 서버 리스트
	TSubclassOf<class UUserWidget> ServerRowClass;
	TOptional<uint32> SelectedIndex;
};