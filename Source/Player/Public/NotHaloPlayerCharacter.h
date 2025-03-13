// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NotHaloPlayerCharacter.generated.h"

//Forward Declarations
class UInputComponent;
class ANotHaloWeapon;
class ANotHaloGrenade;

//Team assignment Enum, in case we want to have team-based game modes
UENUM(BlueprintType)
enum class EPlayerTeam: uint8
{
	None UMETA(Tooltip = "Player is not on a team."),
	BlueTeam UMETA(Tooltip = "Player is on the Blue team."),
	RedTeam UMETA(Tooltip = "Player is on the Red team.")
};

//Delegate Declarations
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

//Delegate for when the player dies
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerIsDead);

//Delegate for weapon change
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponsUpdated);

//Delegate for grenade change
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGrenadeUpdated);

//Delegate for team change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FTeamUpdated,
												EPlayerTeam, OldValue,
												EPlayerTeam, NewValue,
												bool, IsSuccess);

//Delegate for score change
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FScoreUpdated,
												int32, OldValue,
												int32, NewValue,
												bool, Success);

UCLASS(Abstract)
class PLAYER_API ANotHaloPlayerCharacter : public ACharacter
{
public:
	// Sets default values for this character's properties
	ANotHaloPlayerCharacter();

	virtual void AddMovementInput(FVector WorldDirection, float ScaleValue = 1, bool bForce = false) override;
	virtual void Jump() override;
	virtual void Crouch(bool bClientSimulation = false) override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Movement", meta = (AllowPrivateAccess = "true"))
	float WalkSpeed = 400.0f;

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

	//Weapons
	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	ANotHaloWeapon* GetPrimaryWeapon();

	UFUNCTION(BlueprintPure, Category = "Player|Weapons")
	ANotHaloWeapon* GetSecondaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void UseWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void SwitchWeapon();

	UFUNCTION(BlueprintCallable, Category = "Player|Weapons")
	void PickUpNewWeapon(ANotHaloWeapon* NewWeapon);

	//Grenades
	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void ThrowGrenade();
	
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

	UFUNCTION(BlueprintPure, Category = "Player|Grenades")
	float GetThrowGrenadeCooldownRemaining();

	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void SetThrowGrenadeCooldown(float NewCooldown);

	UFUNCTION(BlueprintCallable, Category = "Player|Grenades")
	void StartThrowGrenadeCooldown();

	//Teams & Scoring
	UFUNCTION(BlueprintCallable, Category = "Player|Teams & Scoring")
	void SetTeam(EPlayerTeam NewTeam);

	UFUNCTION(BlueprintCallable, Category = "Player|Teams & Scoring")
	void SetScore(int DeltaScore);

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FCharacterIntStatUpdated OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FCharacterIntStatUpdated OnShieldChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Player|Health & Shield")
	FPlayerIsDead OnPlayerDied;

	UPROPERTY(BlueprintAssignable, Category = "Player|Weapons")
	FWeaponsUpdated OnWeaponChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Grenades")
	FGrenadeUpdated OnGrenadeTypeChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Grenades")
	FCharacterIntStatUpdated OnGrenadeCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Teams & Scoring")
	FTeamUpdated OnTeamChanged;

	UPROPERTY(BlueprintAssignable, Category = "Player|Teams & Scoring")
	FCharacterIntStatUpdated OnScoreChanged;

protected:
	virtual void BeginPlay() override;

private:
	//Health
	static constexpr int BaseMaxHealthValue = 100;
	int MaxHealth = BaseMaxHealthValue;
	int CurrentHealth = BaseMaxHealthValue;

	//Shield
	static constexpr int BaseMaxShieldValue = 100;
	int MaxShield = BaseMaxShieldValue;
	int CurrentShield = BaseMaxShieldValue;

	//Weapons

	//Player's initial Primary Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapons", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ANotHaloWeapon> InitialPrimaryWeapon = nullptr;
	TObjectPtr<ANotHaloWeapon> PrimaryWeapon = nullptr; //TODO Handle Initial Weapon via Game Mode
	//Player's initial Secondary Weapon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Weapons", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ANotHaloWeapon> InitialSecondaryWeapon = nullptr; //TODO Handle Initial Weapon via Game Mode
	TObjectPtr<ANotHaloWeapon> SecondaryWeapon = nullptr;

	//Grenades
	TSubclassOf<ANotHaloGrenade> CurrentGrenade = nullptr;

	//Grenade Types that the Player can use
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Grenades", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<ANotHaloGrenade>> Grenades;
	
	static constexpr int BaseMaxGrenadeCount = 2;
	static constexpr int BaseInitialGrenadeCount = 2;
	static constexpr float BaseThrowGrenadeCooldown = 1;

	//Max number of grenades that the Player can hold of a specific Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Grenades", meta = (AllowPrivateAccess = "true"))
	int MaxGrenadeCount = BaseMaxGrenadeCount;

	//Max number of grenades that the Player can hold of a specific Type
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Grenades", meta = (AllowPrivateAccess = "true"))
	float ThrowGrenadeCooldown = BaseThrowGrenadeCooldown;
	float ThrowGrenadeCooldownRemaining = 0.0f;

	TMap<TSubclassOf<ANotHaloGrenade>, int> GrenadeMap {};

	//Teams & Scoring
	static constexpr EPlayerTeam BaseDefaultTeam = EPlayerTeam::None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Teams & Scoring", meta = (AllowPrivateAccess = "true"))
	EPlayerTeam CurrentTeam = BaseDefaultTeam;

	static constexpr int BaseDefaultScore = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Teams & Scoring", meta = (AllowPrivateAccess = "true"))
	int CurrentScore = BaseDefaultScore;
	
	GENERATED_BODY()
};
