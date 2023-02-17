#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Widgets/IMenuInterface.h"
#include "OnlineSubsystem.h"
#include "CGameInstance.generated.h"

UCLASS()
class GAME_API UCGameInstance : public UGameInstance, public IIMenuInterface
{
	GENERATED_BODY()
public:
	UCGameInstance(const FObjectInitializer& ObjectInitializer);
	virtual void Init() override;
	UFUNCTION(BlueprintCallable, Exec)
		void LoadMainMenu();
	UFUNCTION(BlueprintCallable, Exec)
		void LoadInGameMenu();

	UFUNCTION(Exec)
		void Host() override;
	UFUNCTION(Exec)
		void Join(const FString& InAddress) override;

	void LoadMainMenuLevel() override;

	void RefreshServerList() override;	// 서버 재검색
private:
	void OnCreateSessionComplete(FName InSessionName, bool InSuccess);
	void OnDestroySessionComplete(FName InSessionName, bool InSuccess);
	void OnFindSessionsComplete(bool InSuccess);
	void CreateSession();

private:
	TSubclassOf<class UUserWidget> MainMenuClass;
	TSubclassOf<class UUserWidget> InGameMenuClass;

	class UCMainMenu* MainMenu;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<class FOnlineSessionSearch> SessionSearch;
};