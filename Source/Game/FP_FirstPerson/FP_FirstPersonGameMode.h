// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FP_FirstPersonGameMode.generated.h"

UCLASS(minimalapi)
class AFP_FirstPersonGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFP_FirstPersonGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;	// 컨트롤러를 뗏다가 붙이는 
	
protected:
	void BeginPlay() override;
public:

};



