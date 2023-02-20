#include "CMainMenu.h"
#include "CServerRow.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

UCMainMenu::UCMainMenu(const FObjectInitializer& ObjectInitializer)
{
	// ServerRow클래스Ref 가져오기
	ConstructorHelpers::FClassFinder<UUserWidget> serverRowClass(TEXT("/Game/Widgets/WB_ServerRow"));
	if (serverRowClass.Succeeded())
		ServerRowClass = serverRowClass.Class;
}

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

void UCMainMenu::SetServerList(TArray<FServerData> InServerDatas)
{
	ServerList->ClearChildren();
	// ServerRow 만들기
	uint32 i = 0;
	for (const FServerData& serverData : InServerDatas)
	{
		UCServerRow* row = CreateWidget<UCServerRow>(this, ServerRowClass);
		if (row == nullptr) return;

		row->ServerName->SetText(FText::FromString(serverData.HostUserName));
		row->HostUser->SetText(FText::FromString(serverData.HostUserName));
		FString fractionText = FString::Printf(TEXT("%d/%d"), serverData.CurrentPlayers, serverData.MaxPlayers);
		row->ConnectionFraction->SetText(FText::FromString(fractionText));
		row->Setup(this, i++);
		ServerList->AddChild(row);
	}

}

void UCMainMenu::SetSelectedIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UCMainMenu::JoinServer()
{	
	if (SelectedIndex.IsSet() && !!MenuInterface)
	{
		UE_LOG(LogTemp, Display, TEXT("Selected Index: %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());	// 선택한 Index를 int형으로 넘겨주기
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Selected Index Not Set"));
	}
	//	//IPAddressField->글자 콘탠츠 읽어 넘겨주기
	//	if (IPAddressField == nullptr) return;
	//	const FString& address = IPAddressField->GetText().ToString();
	
}

void UCMainMenu::OpenJoinMenu()	// 메인메뉴에서 Join 버튼 눌렀을 때
{
	if (MenuSwitcher == nullptr) return;
	if (JoinMenu == nullptr) return;
	MenuSwitcher->SetActiveWidget(JoinMenu);

	if (MenuInterface != nullptr)	// 세션 검색 시작 
	{
		MenuInterface->RefreshServerList();
	}
}

void UCMainMenu::OpenMainMenu()
{
	if (MenuSwitcher == nullptr) return;
	if (MainMenu == nullptr) return;
	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UCMainMenu::UpdateChildren()
{	
	// OnClicked를 이용하는게 맞음. 근데 핵심은 Session 이니 넘어가자구
	for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
	{
		UCServerRow* serverRow = Cast<UCServerRow>(ServerList->GetChildAt(i));
		if (!!serverRow)
		{
			serverRow->bSelected = (SelectedIndex.IsSet()) && (SelectedIndex.GetValue() == i);
		}
	}
}

void UCMainMenu::QuitPressed()
{
	UWorld* world = GetWorld();
	if (world == nullptr) return;
	APlayerController* controller = world->GetFirstPlayerController();	
	if (controller == nullptr) return;
	controller->ConsoleCommand("Quit");
}