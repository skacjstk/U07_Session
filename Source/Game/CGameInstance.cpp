#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/CMainMenu.h"
#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = TEXT("GameSession");	// 북미서버, 아시아서버 같은거라 보면 된다.
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");
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
			// 찾기 관련
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCGameInstance::OnFindSessionsComplete);			
			//세선 접속 성공 바인딩
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("OSS Not Found"));
	}
	GEngine->OnNetworkFailure().AddUObject(this, &UCGameInstance::OnNetworkFailure);

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

void UCGameInstance::Host(FString InServerName)
{
	DesiredServerName = InServerName;
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

		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			sessionSettings.bIsLANMatch = true;
		}
		else
		{
			sessionSettings.bIsLANMatch = false;
		}

		sessionSettings.NumPublicConnections = 10;	// 인원수 제한
		sessionSettings.bShouldAdvertise = true;	
		sessionSettings.bUsesPresence = true;	// STEAM OSS 이후 사용: LobbySession을 가져오기 위해
		FString name = "TestValue";
		sessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		SessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
		// 방장번호, 
	}
}

void UCGameInstance::OnNetworkFailure(UWorld* InWorld, UNetDriver* InNewDriver, ENetworkFailure::Type InType, const FString& ErrorString)
{
	LoadMainMenuLevel();	// 네트워크 실패시 ClientTravel
}

void UCGameInstance::StartSession()
{
	if (SessionInterface.IsValid())
		SessionInterface->StartSession(SESSION_NAME);	// 이러면 검색이 안된다.
}

void UCGameInstance::Join(uint32 Index)
{
	if (SessionInterface.IsValid() == false) return;
	if (SessionSearch.IsValid() == false) return;

	if (!!MainMenu)
		MainMenu->Teardown();
	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UCGameInstance::LoadMainMenuLevel()
{
	APlayerController* controller = GetFirstLocalPlayerController();	// 서버,클라 에서, 내가 직접 조종하는 컨트롤러 가져오는 거
	if (controller == nullptr) return;

	controller->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UCGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Start Find Sessions"));
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
//		SessionSearch->bIsLanQuery = true;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());	// 세션을 검색하라는 명령 및 결과를 SessionSearch에 저장
	}
}

void UCGameInstance::OnCreateSessionComplete(FName InSessionName, bool InSuccess)
{
	//	UE_LOG(LogTemp, Warning, TEXT("Create Session"));
	if (InSuccess == false)
	{
		UE_LOG(LogTemp, Error, TEXT("Could Not Create Session!!"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("SessionName : %s"), *InSessionName.ToString());

	if (!!MainMenu)
		MainMenu->Teardown();

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Host"));

	UWorld* world = GetWorld();
	if (world == nullptr) return;
	world->ServerTravel("/Game/Maps/Lobby?listen");
}

void UCGameInstance::OnDestroySessionComplete(FName InSessionName, bool InSuccess)
{
	//	UE_LOG(LogTemp, Warning, TEXT("Destroied Session"));
	if (InSuccess == true)
		CreateSession();
}
void UCGameInstance::OnFindSessionsComplete(bool InSuccess)
{
	if (InSuccess == true && SessionSearch.IsValid() && MainMenu != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Finished Find Sessions"));

		TArray<FServerData> serverDatas;

		for (FOnlineSessionSearchResult& searchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Session Id: %s"), *searchResult.GetSessionIdStr());
			UE_LOG(LogTemp, Warning, TEXT("Ping : %d"), searchResult.PingInMs);
			FServerData data;
			data.Name = searchResult.GetSessionIdStr();
			data.MaxPlayers = searchResult.Session.SessionSettings.NumPublicConnections;
			data.CurrentPlayers = data.MaxPlayers - searchResult.Session.NumOpenPublicConnections;	// 최대 유저수 - 비어있는 슬롯 수
			data.HostUserName = searchResult.Session.OwningUserName;	// 소유자의 PC 이름
			
			FString serverName;
			if (searchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, serverName))
			{
				data.Name = serverName;
			}else
			{
				UE_LOG(LogTemp, Warning, TEXT("Session Not Found"));
			}		
			serverDatas.Add(data);
		}
		MainMenu->SetServerList(serverDatas);	// 검색 완료된 세션 ID들을 넘겨주기
	}
}

void UCGameInstance::OnJoinSessionComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type InResult)
{
	if (SessionInterface.IsValid() == false) return;

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	FString address;	//포트번호는 따로 받아야
	if (SessionInterface->GetResolvedConnectString(InSessionName, address) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get IP Address"));
		return;
	}
	engine->AddOnScreenDebugMessage(0, 2, FColor::Green, FString::Printf(TEXT("Join to %s"), *address));

	APlayerController* controller = GetFirstLocalPlayerController();	// 서버,클라 에서, 내가 직접 조종하는 컨트롤러 가져오는 거
	if (controller == nullptr) return;

	controller->ClientTravel(address, ETravelType::TRAVEL_Absolute);
	// IP 주소는 거의 절대적 = absolute, SteamOSS도 가상 공인IP 주는 거
}
