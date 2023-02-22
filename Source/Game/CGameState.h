#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "CGameState.generated.h"

UENUM(BlueprintType)
enum class ETeamType: uint8
{
	Red, Blue
};
UCLASS()
class GAME_API ACGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
};
