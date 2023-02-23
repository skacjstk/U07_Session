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
	
	virtual void Tick(float DeltaTime) override;
	void Spawn(class AFP_FirstPersonCharacter* InPlayer);

	void Respawn(class AFP_FirstPersonCharacter* InPlayer);
protected:
	void BeginPlay() override;
private:
	TArray<class AFP_FirstPersonCharacter*> RedTeamCharacters;
	TArray<class AFP_FirstPersonCharacter*> BlueTeamCharacters;
	TArray<class AFP_FirstPersonCharacter*> ToBeSpawns;	// 막혀있지 않을 때 스폰시키려고

	TArray<class ACSpawnPoint*> RedTeamSpawners;
	TArray<class ACSpawnPoint*> BlueTeamSpawners;
};



