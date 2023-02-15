#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IMenuInterface.h"
#include "CMenuWidget.generated.h"

UCLASS()
class GAME_API UCMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FORCEINLINE void SetMenuInterface(IIMenuInterface* InMenuInterface) { MenuInterface = InMenuInterface; }

	void Setup();	// UI가 보일 때의 키 설정
	void Teardown();	// 플레이 모드일 때 키 설정


protected:
	IIMenuInterface* MenuInterface;	// GameInstance를 저장받을 것
};
