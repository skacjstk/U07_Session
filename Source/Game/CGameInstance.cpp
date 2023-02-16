#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Widgets/CMainMenu.h"
#include "Widgets/CInGameMenu.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = TEXT("GameSession");	// 북미서버, 아시아서버 같은거라 보면 된다.

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
	IOnlineSubsystem* oss = IOnlineSubsystem::Get();
	if (!!oss)
	{
		UE_LOG(LogTemp, Error, TEXT("Oss Pointer Found. Name : %s"), *oss->GetSubsystemName().ToString());
		SessionInterface = oss->GetSessionInterface();	// find, create 등등이 있음 
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnDestroySessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Oss Not Found"));
	}
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
	if (SessionInterface.IsValid())
	{
		auto existingSession = SessionInterface->GetNamedSession(SESSION_NAME);	// 세션 객체 주소 리턴 
		if (!!existingSession)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UCGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings sessionSettings;
		SessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
		// 방장번호, 
	}
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

void UCGameInstance::OnCreateSessionComplete(FName InSessionName, bool InSuccess)
{
	if (InSuccess == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Create Session"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("SessionName: %s"),*InSessionName.ToString());
	if (!!MainMenu)
		MainMenu->Teardown();

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Host"));

	UWorld* world = GetWorld();
	if (world == nullptr) return;
	world->ServerTravel("/Game/Maps/Play?listen");
}

void UCGameInstance::OnDestroySessionComplete(FName InSessionName, bool InSuccess)
{
	if (InSuccess == true)
		CreateSession();
}
