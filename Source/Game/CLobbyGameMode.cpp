#include "CLobbyGameMode.h"
#include "CGameInstance.h"

void ACLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;

	if (NumberOfPlayers >= 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Reached 3 Players!"));

		UWorld* world = GetWorld();
		if (world == nullptr) return;

		world->GetTimerManager().SetTimer(Timer, this, &ACLobbyGameMode::StartGame, 5.f);
	}
}
void ACLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ACLobbyGameMode::StartGame()
{
	UWorld* world = GetWorld();
	bUseSeamlessTravel = true;

	UCGameInstance* gameInstance = Cast<UCGameInstance>(GetGameInstance());
	if (gameInstance == nullptr) return;
	gameInstance->StartSession();
	world->ServerTravel("/Game/Maps/Play?listen");
}