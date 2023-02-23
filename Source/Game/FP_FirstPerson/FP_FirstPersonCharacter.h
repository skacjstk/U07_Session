#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FP_FirstPersonCharacter.generated.h"

class UInputComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class USoundBase;
class UAnimMontage;

UCLASS(config = Game)
class AFP_FirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//FP
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Mesh;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* FP_Gun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	UPROPERTY(VisibleDefaultsOnly, Category = Gameplay)
		class UParticleSystemComponent* FP_GunShotParticle;

	UPROPERTY(VisibleDefaultsOnly, Category = Gameplay)
		class UParticleSystemComponent* TP_GunShotParticle;

	//TP
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* TP_Gun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* TP_HitAnimation;
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		float BaseLookUpRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		float WeaponDamage;
public:
	AFP_FirstPersonCharacter();

	class ACPlayerState* GetSelfPlayerState();
	void SetSelfPlayerState(class ACPlayerState* NewPlayerState);	// 서버에서만 접근이 가능해야함
protected:
	virtual void PossessedBy(AController* NewController) override;	// PostLogin 보다 먼저 호출된다
	void BeginPlay() override;
	void OnFire();

	// remote procedure call
	UFUNCTION(Server, Reliable)
		void OnServerFire(const FVector& LineStart, const FVector& LineEnd);
	void OnServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd);
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastFireEffect();
	void MulticastFireEffect_Implementation();
private:
	UFUNCTION()
		void Respawn();

public:	// 색 바꾸기
	UFUNCTION(NetMulticast, Reliable)
		void SetTeamColor(ETeamType InTeamType);
	void SetTeamColor_Implementation(ETeamType InTeamType);

protected:
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	UFUNCTION(NetMulticast, Unreliable)
		void PlayDamage();
	void PlayDamage_Implementation();
	UFUNCTION(NetMulticast, Unreliable)
		void PlayDead();
	void PlayDead_Implementation();
public:
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return FP_Mesh; }
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return Camera; }

public:
	UPROPERTY(Replicated)
		ETeamType CurrentTeam;
private:
	class UMaterialInstanceDynamic* DynamicMaterial;
	class ACPlayerState* SelfPlayerState;	// 나 자신의 대한 상태

	/*
	UFUNCTION(Reliable, Server)
		void OnServer();	// 서버 호출
	UFUNCTION(NetMulticast, Reliable)
		void OnNetMulticast();
	UFUNCTION(Client, Reliable)
		void OnClient();

	int32 RandomValue_NoReplicated;
	UPROPERTY(Replicated)
		int32 RandomValue_Replicated;
	*/
};

