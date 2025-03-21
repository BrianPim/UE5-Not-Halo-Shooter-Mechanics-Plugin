// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloPlayerState.h"
#include "NotHaloTeamData.h"
#include "GameFramework/Character.h"
#include "NotHaloPlayerCharacter.generated.h"

struct FNotHaloGrenadeData;
//Forward Declarations
class UInputComponent;
class ANotHaloWeaponBase;
class ANotHaloDummyWeapon;
class ANotHaloGrenade;
class ANotHaloGameModeBase;
class UCameraComponent;

#pragma region Delegate Declarations
//Delegate for integer stats are changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCharacterIntStatUpdated,
												int32, OldValue,
												int32, NewValue,
												int32, MaxValue);

//Delegate for floating point stats are changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCharacterFloatStatUpdated,
												float, OldValue,
												float, NewValue,
												float, MaxValue);

//Delegate for using weapon, melee, etc.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerUpdate);


//Delegate for team change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTeamUpdated,
												FNotHaloTeamData, OldValue,
												FNotHaloTeamData, NewValue);

//Delegate for score change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FScoreUpdated,
												int32, OldValue,
												int32, NewValue,
												bool, Success);

//Delegate for score change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlayerInteraction,
												ANotHaloPlayerCharacter*, Caster,
												ANotHaloPlayerCharacter*, OtherPlayer);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCastHit, FHitResult, Hit);
#pragma endregion 

UCLASS(Abstract)
class PLAYER_API ANotHaloPlayerCharacter : public ACharacter
{
public:
	// Sets default values for this character's properties
	ANotHaloPlayerCharacter();

	//General Input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Player|Input")
	bool InputAllowed();

	//Movement
	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1, bool bForce = false) override;
	virtual void Jump() override;
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Movement")
	float WalkSpeed = 400.0f;
	
#pragma region Public Health & Shield Functions
	//Apply Damage
	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void NotHaloApplyDamage(ANotHaloPlayerCharacter* Caster, int Damage);

	//Insta-kill
	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void Assassination(ANotHaloPlayerCharacter* Caster);

	//Records kill data and then handles the rest through Die()
	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void Kill(ANotHaloPlayerCharacter* Caster);

	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void Die(bool Suicide);
	
	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void Respawn();

	//Health
	UFUNCTION(BlueprintPure, Category = "Player|Health & Shield")
	int GetHealth();

	UFUNCTION(BlueprintPure, Category = "Player|Health & Shield")
	int GetMaxHealth();

	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void UpdateHealth(int DeltaHealth);

	//Shield
	UFUNCTION(BlueprintPure, Category = "Player|Health & Shield")
	int GetShield();

	UFUNCTION(BlueprintPure, Category = "Player|Health & Shield")
	int GetMaxShield();

	UFUNCTION(BlueprintCallable, Category = "Player|Health & Shield")
	void UpdateShield(int DeltaShield);
#pragma endregion
	
#pragma region Public Weapon Functions
	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	ANotHaloWeaponBase* GetPrimaryWeapon();

	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	ANotHaloWeaponBase* GetSecondaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void UseWeapon();

	void UseWeaponEnd();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void SwitchWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void PickUpNewWeapon(ANotHaloWeaponBase* NewWeapon);
	
	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void RefreshPrimaryWeaponModel();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void RefreshSecondaryWeaponModel();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void UseScope();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void UseScopeCustomZoom(float CustomZoom);

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void UnScope();
	
	UPROPERTY(BlueprintReadOnly, Category = "Player|Weapons")
	const USkeletalMeshSocket* FirstPersonArmsWeaponSocket = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Player|Weapons")
	const USkeletalMeshSocket* ThirdPersonPrimaryWeaponSocket = nullptr;
	UPROPERTY(BlueprintReadOnly, Category = "Player|Weapons")
	const USkeletalMeshSocket* ThirdPersonSecondaryWeaponSocket = nullptr;
#pragma endregion

#pragma region Public Grenades & Melee Functions
	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void ThrowGrenade();

	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void SetCanThrowGrenade(bool NewCanThrowGrenade);

	void SetGrenadeCooldownComplete();
	
	UFUNCTION(BlueprintPure, Category = "Player|Grenades")
	TSubclassOf<ANotHaloGrenade> GetCurrentGrenadeType();

	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void SetCurrentGrenadeType(TSubclassOf<ANotHaloGrenade> NewGrenadeType);

	UFUNCTION(BlueprintPure, Category = "Player|Grenades")
	int GetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType);
	
	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void SwitchGrenadeType();

	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void SetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType, int NewGrenades);
	
	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void AddGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType, int DeltaGrenades);

	UFUNCTION(BlueprintPure, Category = "Player|Grenades")
	float GetThrowGrenadeCooldown();

	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void SetThrowGrenadeCooldown(float NewCooldown);

	//Max number of grenades that the Player can hold of a specific Type
	UPROPERTY(EditAnywhere, Category = "Player|Grenades")
	int MaxGrenadeCount = BaseMaxGrenadeCount;

	//Max number of grenades that the Player can hold of a specific Type
	UPROPERTY(EditAnywhere, Category = "Player|Grenades")
	float ThrowGrenadeCooldown = BaseThrowGrenadeCooldown;

	//Melee
	UFUNCTION(BlueprintCallable, Category = "Player|Melee")
	void PerformMelee();

#pragma endregion

#pragma region Public Teams & Scoring Functions
	
	UFUNCTION()
	void SetTeam(FNotHaloTeamData NewTeam);

	UFUNCTION(BlueprintPure, Category = "Player|Teams & Scoring")
	int GetScore();

	UFUNCTION(BlueprintCallable, Category = "Player|Teams & Scoring")
	void SetScore(int DeltaScore);

	UFUNCTION(BlueprintCallable, Category = "Player|Teams & Scoring")
	void AddKills(int DeltaKills);

	UFUNCTION(BlueprintCallable, Category = "Player|Teams & Scoring")
	void AddAssists(int DeltaAssists);

	UFUNCTION(BlueprintCallable, Category = "Player|Teams & Scoring")
	void AddDeaths(int DeltaDeaths);
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "Player|Mesh")
	void RefreshPlayerModel();

#pragma region Delegates
	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FCharacterIntStatUpdated OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FCharacterIntStatUpdated OnShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FPlayerUpdate OnPlayerDied;

	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FPlayerUpdate OnPlayerRespawned;

	//First value is Killer, second value is the Victim
	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FPlayerInteraction OnPlayerKilled;

	UPROPERTY(BlueprintAssignable, Category = "Player|Weapons")
	FPlayerUpdate OnWeaponsInitialized;

	UPROPERTY(BlueprintAssignable, Category = "Player|Weapons")
	FPlayerUpdate OnWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Grenades")
	FPlayerUpdate OnGrenadesInitialized;

	UPROPERTY(BlueprintAssignable, Category = "Player|Grenades")
	FPlayerUpdate OnGrenadeTypeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Melee")
	FPlayerCastHit OnMeleeHit;

	UPROPERTY(BlueprintAssignable, Category = "Player|Grenades")
	FCharacterIntStatUpdated OnGrenadeCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Teams & Scoring")
	FTeamUpdated OnTeamChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Teams & Scoring")
	FCharacterIntStatUpdated OnScoreChanged;
#pragma endregion
	
protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	UPROPERTY()
	TObjectPtr<ANotHaloGameModeBase> GameMode;
	UPROPERTY()
	TObjectPtr<ANotHaloPlayerState> NotHaloPlayerState;
	
	UFUNCTION(Client, Reliable)
	void CLIENT_HandlePossess(AController* NewController);
	
	//Player Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	float BaseFOV;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera", meta = (AllowPrivateAccess = "true"))
	bool IsInFirstPersonCamera = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> FirstPersonArms;

#pragma region Private Health & Shield Variables
	//Health
	static constexpr int BaseMaxHealthValue = 100;
	int MaxHealth = BaseMaxHealthValue;
	int CurrentHealth = BaseMaxHealthValue;

	UPROPERTY(BlueprintReadOnly, Category = "Player|Health & Shield", meta = (AllowPrivateAccess = "true"))
	bool RespawnAfterDeath = true;
	bool IsDead = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Health & Shield", meta = (AllowPrivateAccess = "true"))
	FName HeadshotBoneName;

	//Shield
	static constexpr int BaseMaxShieldValue = 100;
	int MaxShield = BaseMaxShieldValue;
	int CurrentShield = BaseMaxShieldValue;

	//Respawn
	static constexpr float BaseTimeToRespawnAfterKill = 5.0f;
	static constexpr float BaseTimeToRespawnAfterSuicide = 7.0f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Player|Health & Shield", meta = (AllowPrivateAccess = "true"))
	float TimeToRespawnAfterKill = BaseTimeToRespawnAfterKill;
	UPROPERTY(BlueprintReadOnly, Category = "Player|Health & Shield", meta = (AllowPrivateAccess = "true"))
	float TimeToRespawnAfterSuicide = BaseTimeToRespawnAfterSuicide;

	FTimerHandle RespawnTimerHandle;
#pragma endregion
	
#pragma region Private Weapon Variables
	UFUNCTION(Server, Reliable)
	void SERVER_SetInitialWeapons();
	
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetInitialWeapons(TSubclassOf<ANotHaloWeaponBase> NewInitialPrimaryWeapon, TSubclassOf<ANotHaloWeaponBase> NewInitialSecondaryWeapon);

	void InitializeWeapons();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Weapons", meta = (AllowPrivateAccess = "true"))
	FName FirstPersonWeaponSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Weapons", meta = (AllowPrivateAccess = "true"))
	FName PrimaryWeaponSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Weapons", meta = (AllowPrivateAccess = "true"))
	FName SecondaryWeaponSocketName;
	
	//Player's initial Primary Weapon
	TObjectPtr<ANotHaloWeaponBase> PrimaryWeapon = nullptr;
	//Player's initial Secondary Weapon
	TObjectPtr<ANotHaloWeaponBase> SecondaryWeapon = nullptr;

	TObjectPtr<ANotHaloDummyWeapon> ThirdPersonPrimaryWeapon = nullptr;
	TObjectPtr<ANotHaloDummyWeapon> ThirdPersonSecondaryWeapon = nullptr;

	int CurrentScopeLevel = 0;

#pragma endregion
	
#pragma region Private Grenade & Melee Variables
	UFUNCTION(Server, Reliable)
	void SERVER_SetupGrenadeTypes();
	UFUNCTION(NetMulticast, Reliable)
	void MULTICAST_SetupGrenadeTypes(const TArray<FNotHaloGrenadeData>& GrenadeData);

	void InitializeGrenades();
	
	TSubclassOf<ANotHaloGrenade> CurrentGrenade = nullptr;
	
	static constexpr int BaseMaxGrenadeCount = 2;
	static constexpr int BaseInitialGrenadeCount = 2; //TODO Handle this through engine
	static constexpr float BaseThrowGrenadeCooldown = 1;
	
	TMap<TSubclassOf<ANotHaloGrenade>, int> GrenadeMap {};

	FTimerHandle GrenadeCooldownTimerHandle;

	bool CanThrowGrenade = true;
	bool GrenadeCooldownActive = false;

	//Left Hand socket is used when throwing grenades. If left empty or socket can't be found, grenade will spawn from center of player.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Mesh", meta = (AllowPrivateAccess = "true"))
	FName GrenadeSpawnSocketName;
	
	UPROPERTY(BlueprintReadOnly, Category = "Player|Mesh", meta = (AllowPrivateAccess = "true"))
	const USkeletalMeshSocket* GrenadeSpawnSocket = nullptr;

	//Melee
	static constexpr float BaseMeleeRange = 150.0f;
	static constexpr float BaseMeleeDamage = 100.0f;
	static constexpr float BaseMeleeForce = 100.0f;
	static constexpr float BaseAssassinationThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Melee", meta = (AllowPrivateAccess = "true"))
	float MeleeRange = BaseMeleeRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Melee", meta = (AllowPrivateAccess = "true"))
	float MeleeDamage = BaseMeleeDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Melee", meta = (AllowPrivateAccess = "true"))
	float MeleeForce = BaseMeleeForce;

	// Between -1 and 1.
	// 1 = Melee directly behind other player
	//-1 = Melee directly in front of other player
	// If set to -1, all melee attacks will be assassinations (instant-kills)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player|Melee", meta = (AllowPrivateAccess = "true"))
	float AssassinationThreshold = BaseAssassinationThreshold;
#pragma endregion

	GENERATED_BODY()
};
