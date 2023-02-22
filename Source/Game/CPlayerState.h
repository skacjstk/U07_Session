#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CGameState.h"
#include "CPlayerState.generated.h"

UCLASS()
class GAME_API ACPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	ACPlayerState(const FObjectInitializer& ObjectInitializer);

public:	// 게임모드에 자동 등록됨
	UPROPERTY(Replicated)
		float Health;
	// 부모에 Score 라는 변수가 있다.
	UPROPERTY(Replicated)
		int Death;
	UPROPERTY(Replicated)
		ETeamType Team;
};
