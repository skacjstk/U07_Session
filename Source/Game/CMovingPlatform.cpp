#include "CMovingPlatform.h"
#include "Materials/MaterialInstanceConstant.h"

ACMovingPlatform::ACMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	SetMobility(EComponentMobility::Movable);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	if (meshAsset.Succeeded())
	{
		GetStaticMeshComponent()->SetStaticMesh(meshAsset.Object);
	}
	

	ConstructorHelpers::FObjectFinder<UMaterialInstanceConstant> materialAsset(TEXT("MaterialInstanceConstant'/Game/Materials/MAT_Platform_Red.MAT_Platform_Red'"));
	if (materialAsset.Succeeded())
	{
		GetStaticMeshComponent()->SetMaterial(0, materialAsset.Object);
	}

}
void ACMovingPlatform::BeginPlay()
{
	Super::BeginPlay();
	// 리플리케이션 옵션 켜기
	if (HasAuthority())	// 서버만 리플리케이트 권한
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}
	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
}
void ACMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveTrigger <= 0)
		return;

	if (HasAuthority())
	{
		FVector currentLocation = GetActorLocation();
		
		FVector directionUnNormal = GlobalTargetLocation - GlobalStartLocation;
		float directionSize = directionUnNormal.Size();
		FVector direction = directionUnNormal.GetSafeNormal();
		distance = (GlobalStartLocation - currentLocation).Size();

		if (distance > directionSize)
		{
			FVector temp = GlobalTargetLocation;
			GlobalTargetLocation = GlobalStartLocation;
			GlobalStartLocation = temp;
		}

		currentLocation += Speed * direction * DeltaTime;
		SetActorLocation(currentLocation);
	}
}

void ACMovingPlatform::AddActiveTrigger()
{
	++ActiveTrigger;
}

void ACMovingPlatform::RemoveActiveTrigger()
{
	if (ActiveTrigger > 0)
		--ActiveTrigger;
	else
		ActiveTrigger = 0;
}
