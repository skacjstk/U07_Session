#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"

UCGameInstance::UCGameInstance(const FObjectInitializer& ObjectInitializer)
{
	GLog->Log("GameInstance Construction Called");

	ConstructorHelpers::FClassFinder<UUserWidget> mainMenuClass(TEXT("/Game/Widgets/WB_MainMenu"));
	if (mainMenuClass.Succeeded()) {
		MainMenuClass = mainMenuClass.Class;
	}
}

void UCGameInstance::Init()
{
}

void UCGameInstance::LoadMainMenu()
{
	if (MainMenuClass == nullptr) return;
	
	UUserWidget* mainMenu = CreateWidget<UUserWidget>(this, MainMenuClass);
	if (mainMenu == nullptr) return;
	mainMenu->AddToViewport();

	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;

	// Input 옵션 설정
	FInputModeUIOnly inputMode;
	mainMenu->bIsFocusable = true;
	inputMode.SetWidgetToFocus(mainMenu->TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);	// 마우스가 게임화면을 벗어나도 되는지 안되는지


	controller->SetInputMode(inputMode);
	controller->bShowMouseCursor = true;

}

void UCGameInstance::Host()
{
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

	controller->ClientTravel(InAddress, ETravelType::TRAVEL_Absolute);
	// IP 주소는 거의 절대적 = absolute, SteamOSS도 가상 공인IP 주는 거
}
