#include "CMainMenu.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

bool UCMainMenu::Initialize()
{
	bool success = Super::Initialize();
	if (success == false)
		return false;
	if(HostButton == nullptr)	return false;
	HostButton->OnClicked.AddDynamic(this, &UCMainMenu::HostServer);

	if (JoinButton == nullptr)	return false;	// UI 널체크
	JoinButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenJoinMenu);

	if (QuitButton == nullptr)	return false;
	QuitButton->OnClicked.AddDynamic(this, &UCMainMenu::QuitPressed);

	if (CancelJoinMenuButton == nullptr) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	if (ComfirmJoinMenuButton == nullptr) return false;
	ComfirmJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::JoinServer);
	return true;
}

void UCMainMenu::HostServer()
{
	if(!!MenuInterface)
	MenuInterface->Host();
}
void UCMainMenu::JoinServer()
{
	if (!!MenuInterface)
	{
		//IPAddressField->글자 콘탠츠 읽어 넘겨주기
		if (IPAddressField == nullptr) return;
		const FString& address = IPAddressField->GetText().ToString();
		MenuInterface->Join(address);
	}
}

void UCMainMenu::OpenJoinMenu()
{
	if (MenuSwitcher == nullptr) return;
	if (JoinMenu == nullptr) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);
}

void UCMainMenu::OpenMainMenu()
{
	if (MenuSwitcher == nullptr) return;
	if (MainMenu == nullptr) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UCMainMenu::QuitPressed()
{
	UWorld* world = GetWorld();
	if (world == nullptr) return;
	APlayerController* controller = world->GetFirstPlayerController();	
	if (controller == nullptr) return;
	controller->ConsoleCommand("Quit");
}