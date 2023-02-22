#include "FP_FirstPersonGameMode.h"
#include "CGameState.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CPlayerState.h"
#include "CSpawnPoint.h"
#include "EngineUtils.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	
	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = AFP_FirstPersonHUD::StaticClass();
	GameStateClass = ACGameState::StaticClass();	// 게임모드에  변수값 이미 있는거 알지?
	PlayerStateClass = ACPlayerState::StaticClass();
}

void AFP_FirstPersonGameMode::BeginPlay()
{
	// LifeCycle상 메모리에 올라가있을 것
	Super::BeginPlay();
	for (TActorIterator<ACSpawnPoint> iter(GetWorld()); iter; ++iter)
	{
		// Todo.
	}
}

void AFP_FirstPersonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(NewPlayer->GetPawn());
	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);
	
	// Pawn 에 State 등록
	if (!!playerState)
	{
		player->SetPlayerState(playerState);
		player->CurrentTeam = playerState->Team;
		player->SetTeamColor(player->CurrentTeam);
	}

}