#include "CSpawnPoint.h"
#include "Components/CapsuleComponent.h"
#include "Global.h"

ACSpawnPoint::ACSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateSceneComponent<UCapsuleComponent>(this, &Capsule, "Capsule");
}

void ACSpawnPoint::OnConstruction(const FTransform& Transform)
{
	if (Team == ETeamType::Red)
		Capsule->ShapeColor = FColor::Red;
	else
		Capsule->ShapeColor = FColor::Blue;
}

void ACSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &ACSpawnPoint::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACSpawnPoint::ActorEndOverlap);
}

void ACSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Capsule->UpdateOverlaps();	// 오버랩을 사실상 tick단위로 확인..
}

void ACSpawnPoint::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (HasAuthority() == false) return;
	// AddUnique 속에 Find가 있는데 그냥 써봄
	if(OverlappedActors.Find(OtherActor) < 0)
		OverlappedActors.Add(OtherActor);
}

void ACSpawnPoint::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (HasAuthority() == false) return;
		OverlappedActors.Remove(OtherActor);	// 여긴 그냥 조건없이 지워버리면 떨어지지 않음
}

