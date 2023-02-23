#include "FP_FirstPersonGameMode.h"
#include "CGameState.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "CPlayerState.h"
#include "CSpawnPoint.h"
#include "EngineUtils.h"
#include "Global.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
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
	// World의 T 타입에 맞는 객체 배열을 반복자로 받아오기
	for (TActorIterator<ACSpawnPoint> iter(GetWorld()); iter; ++iter)
	{
		if ((*iter)->GetTeam() == ETeamType::Red)
			RedTeamSpawners.Add(*iter);
		else
			BlueTeamSpawners.Add(*iter);
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

		if (BlueTeamCharacters.Num() >= RedTeamCharacters.Num())
		{
			RedTeamCharacters.Add(player);
			playerState->Team = ETeamType::Red;
		}
		else
		{
			BlueTeamCharacters.Add(player);
			playerState->Team = ETeamType::Blue;
		}
		player->CurrentTeam = playerState->Team;
		player->SetTeamColor(player->CurrentTeam);
		Spawn(player);
	}
}

void AFP_FirstPersonGameMode::Tick(float DeltaTime)
{
	// 델리게이트가 정석인데 복잡해서 그냥 Tick 씀
	if (ToBeSpawns.Num() > 0)
	{
		for (AFP_FirstPersonCharacter* player : ToBeSpawns)
		{
			Spawn(player);
		}
	}
}

void AFP_FirstPersonGameMode::Spawn(class AFP_FirstPersonCharacter* InPlayer)
{
	TArray<ACSpawnPoint*>* targetTeam = nullptr;
	if (InPlayer->CurrentTeam == ETeamType::Red)
	{
		targetTeam = &RedTeamSpawners;
	}
	else
	{
		targetTeam = &BlueTeamSpawners;
	}

	for (ACSpawnPoint* point : *targetTeam)
	{
		if (point->IsBlocked() == false)
		{
			InPlayer->SetActorLocation(point->GetActorLocation());
			InPlayer->SetActorRotation(point->GetActorRotation());
			point->UpdateOverlaps();

			if (ToBeSpawns.Find(InPlayer) >= 0)	// 있으면 index 반환
				ToBeSpawns.Remove(InPlayer);
			return;
		}
		
	}
	// 다 막혀버렸을 대
	if(ToBeSpawns.Find(InPlayer) < 0)	// 없으면 -1 반환
		ToBeSpawns.Add(InPlayer);

}

void AFP_FirstPersonGameMode::Respawn(class AFP_FirstPersonCharacter* InPlayer)
{
	AController* controller = InPlayer->GetController();
	InPlayer->DetachFromControllerPendingDestroy();	// 컨트롤러가 Detach된 이유를 알려줄 수 있음
	
	AFP_FirstPersonCharacter* player =Cast<AFP_FirstPersonCharacter>(GetWorld()->SpawnActor(DefaultPawnClass));	// 다시 소환
	if (!!player)
	{
		controller->Possess(player);	// 빙의 
		ACPlayerState* playerState = Cast<ACPlayerState>(player->GetController()->PlayerState);
		player->CurrentTeam = playerState->Team;
		player->SetSelfPlayerState(playerState);
		player->SetTeamColor(playerState->Team);
		Spawn(player);	// 기본 설정 및 최종 Spawn
	}
}