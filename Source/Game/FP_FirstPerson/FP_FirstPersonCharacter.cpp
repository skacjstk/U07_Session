#include "FP_FirstPersonCharacter.h"
#include "FP_FirstPersonGameMode.h"
#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Global.h"
#include "Particles/ParticleSystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "CBullet.h"
#include "CGameState.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/PlayerState.h"
#include "CPlayerState.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);


void AFP_FirstPersonCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	//https://docs.unrealengine.com/4.26/en-US/InteractiveExperiences/Networking/Actors/Properties/
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFP_FirstPersonCharacter, CurrentTeam);	// 어느 클래스의 어떤 변수를 복제할지 등록
}

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0, 0, 64.f));
	Camera->bUsePawnControlRotation = true;

	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FP_Mesh->SetOnlyOwnerSee(true);
	FP_Mesh->SetupAttachment(Camera);
	FP_Mesh->bCastDynamicShadow = false;
	FP_Mesh->CastShadow = false;

	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(FP_Mesh, TEXT("GripPoint"));

	WeaponRange = 5000.0f;
	WeaponDamage = 50.0f;

	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	GetMesh()->SetOwnerNoSee(true);
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetupAttachment(GetMesh(), TEXT("GripPoint"));
	TP_Gun->SetOwnerNoSee(true);

	FP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FP_GunShotParticle"));
	FP_GunShotParticle->bAutoActivate =false;
	FP_GunShotParticle->Activate(false);

	FP_GunShotParticle->SetupAttachment(FP_Gun, "Muzzle");
	FP_GunShotParticle->SetOnlyOwnerSee(true);

	TP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TP_GunShotParticle"));
	TP_GunShotParticle->bAutoActivate = false;
	TP_GunShotParticle->Activate(false);
	TP_GunShotParticle->SetupAttachment(TP_Gun, "Muzzle");
	TP_GunShotParticle->SetOwnerNoSee(true);
}

ACPlayerState* AFP_FirstPersonCharacter::GetSelfPlayerState()
{
	if(SelfPlayerState == nullptr)
		SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());

	return SelfPlayerState;
}

void AFP_FirstPersonCharacter::SetSelfPlayerState(ACPlayerState* NewPlayerState)
{
	CheckFalse(HasAuthority());

	SetPlayerState(NewPlayerState);
	SelfPlayerState = Cast<ACPlayerState>(NewPlayerState);
}

void AFP_FirstPersonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());
	if(GetLocalRole() == ENetRole::ROLE_Authority && !!SelfPlayerState)	// Has_Authority() 대신
		SelfPlayerState->Health = 100.f;
}

void AFP_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if(HasAuthority() == false)
		SetTeamColor(CurrentTeam);	// PostLogin 보다 나중에 호출됨
}

void AFP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFP_FirstPersonCharacter::OnFire);
	PlayerInputComponent->BindAxis("MoveForward", this, &AFP_FirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFP_FirstPersonCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFP_FirstPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFP_FirstPersonCharacter::LookUpAtRate);
}

void AFP_FirstPersonCharacter::PlayDamage_Implementation()
{
	if (!!TP_HitAnimation)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (!!animInstance)
		{
			animInstance->Montage_Play(TP_HitAnimation, 1.5f);
		}
	}
}

void AFP_FirstPersonCharacter::PlayDead_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetPhysicsBlendWeight(0.5f);	// 렉돌의 정도
	GetMesh()->SetSimulatePhysics(true);
}


void AFP_FirstPersonCharacter::OnFire()
{
	CheckTrue(GetSelfPlayerState()->Health <= 0);	

	if (FireAnimation != NULL)
	{
		UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	if (!!FP_GunShotParticle)
	{
		FP_GunShotParticle->Activate(true);
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	FVector ShootDir = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;

	if (PlayerController)
	{
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(StartTrace, CamRot);	// out 파라미터
		ShootDir = CamRot.Vector();	// 유닛벡터:방향

		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);	// 투영벡터 구하기 vector * 내적한 벡터
	}

	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;

	
	UGameplayStatics::ApplyDamage()

	OnServerFire(StartTrace, EndTrace);
}

void AFP_FirstPersonCharacter::OnServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd)
{
	const FHitResult Impact = WeaponTrace(LineStart, LineEnd);
	MulticastFireEffect();	
}

void AFP_FirstPersonCharacter::MulticastFireEffect_Implementation()
{
	if (!!TP_FireAnimation)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (!!animInstance)
		{
			animInstance->Montage_Play(TP_FireAnimation, 1.f);
		}
	}
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	if (!!TP_GunShotParticle)
	{
		TP_GunShotParticle->Activate(true);
	}

	GetWorld()->SpawnActor<ACBullet>(ACBullet::StaticClass(), FP_Gun->GetSocketLocation("Muzzle"), GetControlRotation());
}

void AFP_FirstPersonCharacter::SetTeamColor_Implementation(ETeamType InTeamType)
{
	FLinearColor color;
	if (InTeamType == ETeamType::Red)
		color = FLinearColor(0.8f, 0.0f, 0.0f);
	else	
		color = FLinearColor(0.0f, 0.0f, 0.8f);

	if (DynamicMaterial == nullptr)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), nullptr);	// DMI 만들기
	}
	DynamicMaterial->SetVectorParameterValue("BodyColor", color);
	GetMesh()->SetMaterial(0, DynamicMaterial);
	FP_Mesh->SetMaterial(0, DynamicMaterial);
}

void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFP_FirstPersonCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AFP_FirstPersonCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

FHitResult AFP_FirstPersonCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);
	
	CheckFalseResult(Hit.IsValidBlockingHit(), Hit);
	AFP_FirstPersonCharacter* other = Cast<AFP_FirstPersonCharacter>(Hit.GetActor());

	if (other != nullptr && other->GetSelfPlayerState()->Team != GetSelfPlayerState()->Team && other->GetSelfPlayerState()->Health > 0)
	{
		FDamageEvent e;
		other->TakeDamage(WeaponDamage, e, GetController(), this);
	}

	return Hit;
}

float AFP_FirstPersonCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	CheckTrueResult(DamageCauser == this, DamageAmount);
	SelfPlayerState->Health -= DamageAmount;

	if (SelfPlayerState->Health <= 0)
	{
		PlayDead();
		++SelfPlayerState->Death;
		AFP_FirstPersonCharacter* other = Cast< AFP_FirstPersonCharacter>(DamageCauser);
		if (!!other)
			other->SelfPlayerState->Score += 1.f;

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &AFP_FirstPersonCharacter::Respawn, 3.f, false);


		return DamageAmount;
	}

	PlayDamage();
	return DamageAmount;

}

void AFP_FirstPersonCharacter::Respawn()
{	
	CheckFalse(HasAuthority());
	// 서버로써의 게임모드 가져오기
	SelfPlayerState->Health = 100.f;	// 풀피로 채워놓고 
	Cast<AFP_FirstPersonGameMode>(GetWorld()->GetAuthGameMode())->Respawn(this);
			// 컨트롤러를 붙였다가 떼기 위해
	Destroy(true);
	// 렉돌은 원복이 안돼 ( MeshComponent 를 다시 해야)
		
}








/*

void AFP_FirstPersonCharacter::OnServer_Implementation()
{
//	CLog::Print("Only Called OnServer");
	OnNetMulticast();
	OnClient();
	++RandomValue_Replicated;

}
void AFP_FirstPersonCharacter::OnNetMulticast_Implementation()
{
//	CLog::Print("Net Multicast Called");
	//if (GetLocalRole() == ENetRole::ROLE_Authority)
	//	CLog::Print("Role_AUthority In NetMulticast", -1, 10.f, FColor::Purple);

	//if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	//	CLog::Print("ROLE_AutonomousProxy In NetMulticast", -1, 10.f, FColor::Purple);

	//if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	//	CLog::Print("ROLE_SimulatedProxy In NetMulticast", -1, 10.f, FColor::Purple);

	CLog::Print("No Rep: " + FString::FromInt(RandomValue_NoReplicated));
	CLog::Print("Rep: " + FString::FromInt(RandomValue_Replicated));
}

void AFP_FirstPersonCharacter::OnClient_Implementation()
{
//	CLog::Print("Client Called");

	//if (GetLocalRole() == ENetRole::ROLE_Authority)
	//	CLog::Print("Role_AUthority In Client");

	//if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	//	CLog::Print("ROLE_AutonomousProxy In Client");

	//if (GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
	//	CLog::Print("ROLE_SimulatedProxy In Client");

	RandomValue_NoReplicated++;
}
*/

