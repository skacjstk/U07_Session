#include "CMenuWidget.h"

void UCMenuWidget::Setup()
{
	AddToViewport();
	bIsFocusable = true;

	// Input 옵션 설정
	FInputModeUIOnly inputMode;

	UWorld* world = GetWorld();
	if (world == nullptr) return;

	APlayerController* controller = world->GetFirstPlayerController();
	if (controller == nullptr) return;

	controller->SetInputMode(inputMode);
	controller->bShowMouseCursor = true;
}

void UCMenuWidget::Teardown()
{
	RemoveFromParent();
	bIsFocusable = false;

	FInputModeGameOnly inpuMode;

	UWorld* world = GetWorld();
	if (world == nullptr) return;

	APlayerController* controller = world->GetFirstPlayerController();
	if (controller == nullptr) return;
	controller->SetInputMode(inpuMode);
	controller->bShowMouseCursor = false;
}
