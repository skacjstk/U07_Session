// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IMenuInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIMenuInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GAME_API IIMenuInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Host(FString InServerName) = 0;
	virtual void Join(uint32 Index) = 0;
	virtual void LoadMainMenuLevel() = 0;
	virtual void RefreshServerList() = 0;
};
