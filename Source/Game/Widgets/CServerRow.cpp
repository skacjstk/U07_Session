#include "CServerRow.h"
#include "CMainMenu.h"
#include "Components/Button.h"

void UCServerRow::Setup(UCMainMenu* InParent, uint32 InIndex)
{
	Parent = InParent;
	Index = InIndex;	// 정보 저장
	RowButton->OnClicked.AddDynamic(this, &UCServerRow::OnClicked);
}

void UCServerRow::OnClicked()
{
	Parent->SetSelectedIndex(Index);
}
