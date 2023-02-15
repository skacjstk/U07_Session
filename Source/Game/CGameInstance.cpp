#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Widgets/CMainMenu.h"
#include "Widgets/CInGameMenu.h"
#include "Blueprint/UserWidget.h"

UCGameInstance::UCGameInstance(const FObjectInitializer& ObjectInitializer)
{
	GLog->Log("GameInstance Construction Called");

	ConstructorHelpers::FClassFinder<UUserWidget> mainMenuClass(TEXT("/Game/Widgets/WB_MainMenu"));
	if (mainMenuClass.Succeeded()) {
		MainMenuClass = mainMenuClass.Class;
	}
	ConstructorHelpers::FClassFinder<UUserWidget> inGameMenuClass(TEXT("/Game/Widgets/WB_InGameMenu"));
	if (inGameMenuClass.Succeeded()) {
		InGameMenuClass = inGameMenuClass.Class;
	}
}

void UCGameInstance::Init()
{
}

void UCGameInstance::LoadMainMenu()
{
	if (MainMenuClass == nullptr) return;
	
	MainMenu = CreateWidget<UCMainMenu>(this, MainMenuClass);
	if (MainMenu == nullptr) return;

	MainMenu->SetMenuInterface(this);	// 나 자신을 넘겨주기
	MainMenu->Setup();
}

void UCGameInstance::LoadInGameMenu()
{
	if (InGameMenuClass == nullptr) return;

	UCMenuWidget* inGameMenu = CreateWidget<UCMenuWidget>(this, InGameMenuClass);
	if (inGameMenu == nullptr) return;

	inGameMenu->SetMenuInterface(this);	// 나 자신을 넘겨주기
	inGameMenu->Setup();
}

void UCGameInstance::Host()
{
	if(!!MainMenu)
		MainMenu->Teardown();

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	UWorld* world = GetWorld();
	if (world == nullptr) return;
	world->ServerTravel("/Game/Maps/Play?listen");
	engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Host"));
}

void UCGameInstance::Join(const FString& InAddress)
{
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Join to %s"), *InAddress));

	APlayerController* controller = GetFirstLocalPlayerController();	// 서버,클라 에서, 내가 직접 조종하는 컨트롤러 가져오는 거
	if (controller == nullptr) return;

	// UI 인풋모드 세팅을 돌려놓기
	if(!!MainMenu)
		MainMenu->Teardown();

	controller->ClientTravel(InAddress, ETravelType::TRAVEL_Absolute);
	// IP 주소는 거의 절대적 = absolute, SteamOSS도 가상 공인IP 주는 거
}

void UCGameInstance::LoadMainMenuLevel()
{
	APlayerController* controller = GetFirstLocalPlayerController();	// 서버,클라 에서, 내가 직접 조종하는 컨트롤러 가져오는 거
	if (controller == nullptr) return;

	controller->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}
