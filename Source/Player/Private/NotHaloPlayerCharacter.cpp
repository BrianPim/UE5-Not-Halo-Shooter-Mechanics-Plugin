// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloPlayerCharacter.h"

#include "NotHaloGrenade.h"
#include "NotHaloPlayerLogging.h"
#include "NotHaloWeapon.h"

// Sets default values
ANotHaloPlayerCharacter::ANotHaloPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANotHaloPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//TODO Handle Weapon Positioning on Player
	const FVector SpawnLocation = FVector::ZeroVector;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = this;
	PlayerWeaponSpawnParams.Instigator = this;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	PrimaryWeapon = GetWorld()->SpawnActor<ANotHaloWeapon>(InitialPrimaryWeapon, SpawnLocation, SpawnRotation, PlayerWeaponSpawnParams);
	SecondaryWeapon = GetWorld()->SpawnActor<ANotHaloWeapon>(InitialSecondaryWeapon, SpawnLocation, SpawnRotation, PlayerWeaponSpawnParams);

	for(int i = 0; i < Grenades.Num(); i++)
	{
		GrenadeMap.Add(Grenades[i], MaxGrenadeCount); //TODO Handle Grenade Count via Game Mode
	}
	
	SetCurrentGrenadeType(Grenades[0]);
}

// Called every frame
void ANotHaloPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ThrowGrenadeCooldownRemaining > 0.0f)
	{
		ThrowGrenadeCooldownRemaining -= DeltaTime;
	}
}

// Called to bind functionality to input
void ANotHaloPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANotHaloPlayerCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void ANotHaloPlayerCharacter::Jump()
{
	Super::Jump();
}

void ANotHaloPlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
}

//Health
//Returns Current Health
int ANotHaloPlayerCharacter::GetHealth()
{
	return CurrentHealth;
}

//Returns Max Health
int ANotHaloPlayerCharacter::GetMaxHealth()
{
	return MaxHealth;
}

//Update Player's Current Health Value
void ANotHaloPlayerCharacter::UpdateHealth(int DeltaHealth)
{
	//DEAD IS DEAD
	if (CurrentHealth <= 0.f) return;

	int OldValue = CurrentHealth;
	
	CurrentHealth += DeltaHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth, -1.f, MaxHealth);

	if (CurrentHealth != OldValue)
	{
		OnHealthChanged.Broadcast(OldValue, CurrentHealth, MaxHealth);
	}

	if (CurrentHealth <= 0.f)
	{
		//Player is dead!
		OnPlayerDied.Broadcast();
	}
}

//Shield
//Returns Current Shield
int ANotHaloPlayerCharacter::GetShield()
{
	return CurrentShield;
}

//Returns Max Shield
int ANotHaloPlayerCharacter::GetMaxShield()
{
	return MaxShield;
}

//Update Player's Current Shield Value
void ANotHaloPlayerCharacter::UpdateShield(int DeltaShield)
{
	int OldValue = CurrentShield;
	
	CurrentShield += DeltaShield;
	CurrentShield = FMath::Clamp(CurrentShield, -1.f, MaxShield);

	if (CurrentShield != OldValue)
	{
		OnShieldChanged.Broadcast(OldValue, CurrentShield, MaxShield);
	}
}

//Weapons
//Returns Primary Weapon
ANotHaloWeapon* ANotHaloPlayerCharacter::GetPrimaryWeapon()
{
	return PrimaryWeapon;
}

//Returns Secondary Weapon
ANotHaloWeapon* ANotHaloPlayerCharacter::GetSecondaryWeapon()
{
	return SecondaryWeapon;
}

void ANotHaloPlayerCharacter::UseWeapon()
{
	checkf(PrimaryWeapon, TEXT("Primary Weapon is null! Unable to USE weapon!"));
	PrimaryWeapon->UseWeapon();
}

void ANotHaloPlayerCharacter::ReloadWeapon()
{
	checkf(PrimaryWeapon, TEXT("Primary Weapon is null! Unable to RELOAD weapon!"));
	PrimaryWeapon->StartReloadWeapon();
}

//Switches Player's Weapons; Primary becomes Secondary and Secondary becomes Primary
void ANotHaloPlayerCharacter::SwitchWeapon()
{
	checkf(PrimaryWeapon, TEXT("Primary Weapon is null! Unable to SWITCH weapons!"));
	checkf(SecondaryWeapon, TEXT("Secondary Weapon is null! Unable to SWITCH weapons!"));
	TObjectPtr<ANotHaloWeapon> WeaponToSwitch = PrimaryWeapon;

	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = WeaponToSwitch;
	
	OnWeaponChanged.Broadcast();
}

//Drops Player's current Primary Weapon for a new Weapon
void ANotHaloPlayerCharacter::PickUpNewWeapon(ANotHaloWeapon* NewWeapon)
{
	TObjectPtr<ANotHaloWeapon> WeaponToDrop = PrimaryWeapon;
	PrimaryWeapon = NewWeapon;

	//TODO Drop Weapon
	
	OnWeaponChanged.Broadcast();
}

//Grenades
//Throws Grenade
void ANotHaloPlayerCharacter::ThrowGrenade()
{
	checkf(CurrentGrenade, TEXT("Current Grenade is null! Unable to THROW grenade!"));

	if (ThrowGrenadeCooldownRemaining > 0.0f)
	{
		UE_LOG(NotHaloPlayer, Warning, TEXT("A Grenade cannot be thrown while it's on cooldown."));
		return;
	}

	if (GrenadeMap[CurrentGrenade] <= 0)
	{
		UE_LOG(NotHaloPlayer, Display, TEXT("Player has no more grenades!"))
		return;
	}

	//TODO give proper location, give vector to travel along
	const FVector SpawnLocation = FVector::ZeroVector;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = this;
	PlayerWeaponSpawnParams.Instigator = this;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	TObjectPtr<ANotHaloGrenade> Grenade = GetWorld()->SpawnActor<ANotHaloGrenade>(CurrentGrenade, SpawnLocation,
																					SpawnRotation, PlayerWeaponSpawnParams);
	AddGrenadeCount(CurrentGrenade, -1);

	UE_LOG(NotHaloPlayer, Warning, TEXT("Grenades of Current Grenade Type Remaining: %d"), GrenadeMap[CurrentGrenade])

	if (GrenadeMap[CurrentGrenade] == 0)
	{
		UE_LOG(NotHaloPlayer, Warning, TEXT("Player has run out of Grenades of the Current Type. Attempting to switch."))
		SwitchGrenadeType();
	}

	
	StartThrowGrenadeCooldown();
}

//Sets Current Grenade Type
void ANotHaloPlayerCharacter::SetCurrentGrenadeType(TSubclassOf<ANotHaloGrenade> NewGrenadeType)
{
	checkf(NewGrenadeType, TEXT("Provided Grenade Type is null! Unable to SET grenade type!"));
	CurrentGrenade = NewGrenadeType;

	UE_LOG(NotHaloPlayer, Display, TEXT("Player switched to Grenade Type: %s"), *CurrentGrenade->GetName())

	OnGrenadeTypeChanged.Broadcast();
}

//Returns Current Grenade Type
TSubclassOf<ANotHaloGrenade> ANotHaloPlayerCharacter::GetCurrentGrenadeType()
{
	checkf(CurrentGrenade, TEXT("Current Grenade is null! Unable to RETURN grenade type!"));
	return CurrentGrenade;
}

//Returns Current Grenade Count of provided Grenade Type
int ANotHaloPlayerCharacter::GetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType)
{
	checkf(GrenadeMap[GrenadeType], TEXT("Grenade type not found! Make sure to assign it inside of the Player Character Blueprint!"))
	return GrenadeMap[GrenadeType];
}

//Switches Player's Grenade Type. E.g. From Frag to Plasma and Plasma to Frag.
void ANotHaloPlayerCharacter::SwitchGrenadeType()
{
	int GrenadeArrayLength = GrenadeMap.Num();

	if (GrenadeArrayLength < 2)
	{
		UE_LOG(NotHaloPlayer, Warning, TEXT("Unable to switch Grenades! Check if there's at least two Grenade entries in the Player Character Blueprint!"))
		return;
	}
	
	int Index = 0;
	int NextIndex = 0;
	int TotalGrenadeCount = 0;

	//Initial loop to find the index of the Current Grenade
	//Also checking total grenade count here
	//Function will exit early if the Player Character has no Grenades
	for (auto& Grenade : GrenadeMap)
	{
		if (Grenade.Key == CurrentGrenade)
		{
			NextIndex = Index + 1;
		}

		Index++;
		TotalGrenadeCount += Grenade.Value;
	}
	
	if (TotalGrenadeCount == 0)
	{
		UE_LOG(NotHaloPlayer, Display, TEXT("Unable to switch Grenades! Player has no grenades left!"))
		return;
	}
	
	//Second loop, looking for a valid Grenade Type to switch to
	for (int i = 0; i < GrenadeArrayLength; i++)
	{
		if (NextIndex >= GrenadeArrayLength)
		{
			NextIndex = 0;
		}
		
		//If true, valid Grenade type To switch to has been found and the function can be ended early
		if (GrenadeMap[Grenades[NextIndex]] > 0)
		{
			SetCurrentGrenadeType(Grenades[NextIndex]);
			return;
		}

		NextIndex++;
	}

	UE_LOG(NotHaloPlayer, Warning, TEXT("Unable to switch Grenades! Could not find valid Grenade type!"))
}

//Sets Count of provided Grenade Type
void ANotHaloPlayerCharacter::SetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType, int NewGrenades)
{
	int OldValue = GrenadeMap[GrenadeType];

	GrenadeMap[GrenadeType] = NewGrenades;
	GrenadeMap[GrenadeType] = FMath::Clamp(GrenadeMap[GrenadeType], 0, MaxGrenadeCount);
	
	OnGrenadeCountChanged.Broadcast(OldValue, GrenadeMap[GrenadeType], MaxGrenadeCount);
}

//Adds to Count of provided Grenade Type
void ANotHaloPlayerCharacter::AddGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType, int DeltaGrenades)
{
	int OldValue = GrenadeMap[GrenadeType];
	GrenadeMap[GrenadeType] += DeltaGrenades;
	GrenadeMap[GrenadeType] = FMath::Clamp(GrenadeMap[GrenadeType], 0, MaxGrenadeCount);
	
	OnGrenadeCountChanged.Broadcast(OldValue, GrenadeMap[GrenadeType], MaxGrenadeCount);
}

float ANotHaloPlayerCharacter::GetThrowGrenadeCooldown()
{
	return ThrowGrenadeCooldown;
}

void ANotHaloPlayerCharacter::SetThrowGrenadeCooldown(float NewCooldown)
{
	ThrowGrenadeCooldown = NewCooldown;
	ThrowGrenadeCooldown = FMath::Clamp(ThrowGrenadeCooldown, 0, BIG_NUMBER);
}

float ANotHaloPlayerCharacter::GetThrowGrenadeCooldownRemaining()
{
	return ThrowGrenadeCooldownRemaining;
}

void ANotHaloPlayerCharacter::StartThrowGrenadeCooldown()
{
	ThrowGrenadeCooldownRemaining = ThrowGrenadeCooldown;
}

//Teams & Scoring
//Sets Player's current Team
void ANotHaloPlayerCharacter::SetTeam(EPlayerTeam NewTeam)
{
	EPlayerTeam OldTeam = CurrentTeam;
	CurrentTeam = NewTeam;

	OnTeamChanged.Broadcast(OldTeam, CurrentTeam, CurrentTeam == NewTeam);
}

//Sets Player's current Score
void ANotHaloPlayerCharacter::SetScore(int DeltaScore)
{
	int OldScore = CurrentScore;
	CurrentScore += DeltaScore;

	OnScoreChanged.Broadcast(OldScore, CurrentScore, CurrentScore == OldScore + DeltaScore);
}







