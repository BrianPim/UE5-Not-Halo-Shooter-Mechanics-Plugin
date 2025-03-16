// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloPlayerCharacter.h"
#include "NotHaloGrenade.h"
#include "NotHaloPlayerLogging.h"
#include "NotHaloWeaponBase.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Camera/CameraComponent.h"

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

	PrimaryWeaponSocket = GetMesh()->GetSocketByName(PrimaryWeaponSocketName);
	SecondaryWeaponSocket = GetMesh()->GetSocketByName(SecondaryWeaponSocketName);
	
	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = this;
	PlayerWeaponSpawnParams.Instigator = this;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if (InitialPrimaryWeapon)
	{
		PrimaryWeapon = GetWorld()->SpawnActor<ANotHaloWeaponBase>(InitialPrimaryWeapon, FVector::ZeroVector,
															FRotator::ZeroRotator, PlayerWeaponSpawnParams);
		PrimaryWeapon->SetWeaponHolderPawn(this);
		RefreshPrimaryWeaponSocket();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("No initial Primary Weapon assigned to the player! Is this intentional?"))
	}

	if (InitialSecondaryWeapon)
	{
		SecondaryWeapon = GetWorld()->SpawnActor<ANotHaloWeaponBase>(InitialSecondaryWeapon, FVector::ZeroVector,
																FRotator::ZeroRotator, PlayerWeaponSpawnParams);
		SecondaryWeapon->SetWeaponHolderPawn(this);
		RefreshSecondaryWeaponSocket();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("No initial Secondary Weapon assigned to the player! Is this intentional?"))
	}
	
	if (GrenadeTypes.Num() > 0)
	{
		for(int i = 0; i < GrenadeTypes.Num(); i++)
		{
			GrenadeMap.Add(GrenadeTypes[i], MaxGrenadeCount); //TODO Handle Grenade Count via Game Mode
		}
	
		SetCurrentGrenadeType(GrenadeTypes[0]);
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("No Grenade Types assigned to the player! Is this intentional?"))
	}

	OnWeaponsInitialized.Broadcast();
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

//Damages the Player. Death is handled in UpdateHealth()
void ANotHaloPlayerCharacter::TakeDamage(int Damage)
{
	if (CurrentHealth <= 0)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("%s is already dead!"), *GetName())
		return;
	}
	
	if (CurrentShield > 0)
	{
		int OldShield = CurrentShield;
		UpdateShield(-Damage);

		int OverflowDamage = OldShield - Damage;

		if (OverflowDamage < 0)
		{
			int OldHealth = CurrentHealth;
			UpdateHealth(OverflowDamage);
		}
	}

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s took %d Damage!"), *GetName(), Damage)
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
	if (CurrentHealth <= 0)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("%s is already dead!"), *GetName())
		return;
	}

	int OldValue = CurrentHealth;
	
	CurrentHealth += DeltaHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth, -1, MaxHealth);

	if (CurrentHealth != OldValue)
	{
		OnHealthChanged.Broadcast(OldValue, CurrentHealth, MaxHealth);
	}

	if (CurrentHealth <= 0)
	{
		//Player is dead!
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s has Died!"), *GetName())
		
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
	CurrentShield = FMath::Clamp(CurrentShield, -1, MaxShield);

	if (CurrentShield != OldValue)
	{
		OnShieldChanged.Broadcast(OldValue, CurrentShield, MaxShield);
	}
}

//Weapons
//Returns Primary Weapon
ANotHaloWeaponBase* ANotHaloPlayerCharacter::GetPrimaryWeapon()
{
	return PrimaryWeapon;
}

//Returns Secondary Weapon
ANotHaloWeaponBase* ANotHaloPlayerCharacter::GetSecondaryWeapon()
{
	return SecondaryWeapon;
}

void ANotHaloPlayerCharacter::UseWeapon()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->UseWeapon();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to USE weapon!"));
	}
}

void ANotHaloPlayerCharacter::ReloadWeapon()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->StartReloadWeapon();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to RELOAD weapon!"));
	}
}

//Switches Player's Weapons; Primary becomes Secondary and Secondary becomes Primary
void ANotHaloPlayerCharacter::SwitchWeapon()
{
	if (!PrimaryWeapon)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to SWITCH weapons!"));
		return;
	}

	if (!SecondaryWeapon)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Secondary Weapon is null! Unable to SWITCH weapons!"));
		return;
	}
	
	TObjectPtr<ANotHaloWeaponBase> WeaponToSwitch = PrimaryWeapon;

	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = WeaponToSwitch;

	RefreshPrimaryWeaponSocket();
	RefreshSecondaryWeaponSocket();

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("Switched Weapon from %s to %s"), *SecondaryWeapon->GetWeaponName(), *PrimaryWeapon->GetWeaponName());
	
	OnWeaponChanged.Broadcast();
}

//Drops Player's current Primary Weapon for a new Weapon
void ANotHaloPlayerCharacter::PickUpNewWeapon(ANotHaloWeaponBase* NewWeapon)
{
	if (PrimaryWeapon)
	{
		TObjectPtr<ANotHaloWeaponBase> WeaponToDrop = PrimaryWeapon;
		WeaponToDrop->SetWeaponHolderPawn(nullptr);
	}
	
	PrimaryWeapon = NewWeapon;
	PrimaryWeapon->SetWeaponHolderPawn(this);

	RefreshPrimaryWeaponSocket();
	
	//TODO Drop Weapon
	
	OnWeaponChanged.Broadcast();
}

void ANotHaloPlayerCharacter::RefreshPrimaryWeaponSocket()
{
	if (PrimaryWeaponSocket)
	{
		PrimaryWeaponSocket->AttachActor(PrimaryWeapon, GetMesh());
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to refresh Primary Weapon Socket!"))
	}
}

void ANotHaloPlayerCharacter::RefreshSecondaryWeaponSocket()
{
	if (SecondaryWeaponSocket)
	{
		SecondaryWeaponSocket->AttachActor(SecondaryWeapon, GetMesh());
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to refresh Secondary Weapon Socket!"))
	}
}


//GrenadeTypes
//Throws Grenade
void ANotHaloPlayerCharacter::ThrowGrenade()
{
	if (!CurrentGrenade)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Current Grenade is null! Unable to THROW grenade!"));
		return;
	}

	if (ThrowGrenadeCooldownRemaining > 0.0f)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("A Grenade cannot be thrown while it's on cooldown."));
		return;
	}

	if (GrenadeMap[CurrentGrenade] == 0)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("Player has no more grenades!"))
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

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s Remaining: %d"), *CurrentGrenade->GetClass()->GetName(), GrenadeMap[CurrentGrenade])

	if (GrenadeMap[CurrentGrenade] == 0)
	{
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("Player has run out of %ss. Attempting to switch."), *CurrentGrenade->GetClass()->GetName())
		SwitchGrenadeType();
	}
	
	StartThrowGrenadeCooldown();
}

//Sets Current Grenade Type
void ANotHaloPlayerCharacter::SetCurrentGrenadeType(TSubclassOf<ANotHaloGrenade> NewGrenadeType)
{
	if (!NewGrenadeType)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Provided Grenade Type is null! Unable to SET grenade type!"));
		return;
	}
	
	CurrentGrenade = NewGrenadeType;

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("Player switched to Grenade Type: %s"), *CurrentGrenade->GetName())

	OnGrenadeTypeChanged.Broadcast();
}

//Returns Current Grenade Type
TSubclassOf<ANotHaloGrenade> ANotHaloPlayerCharacter::GetCurrentGrenadeType()
{
	if (!CurrentGrenade)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Provided Grenade Type is null! Unable to RETURN grenade type!"));
		return nullptr;
	}
	
	return CurrentGrenade;
}

//Returns Current Grenade Count of provided Grenade Type
int ANotHaloPlayerCharacter::GetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType)
{
	if (!GrenadeMap[GrenadeType])
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Grenade type not found! Make sure to assign it inside of the Player Character Blueprint!"));
		return 0;
	}
	
	return GrenadeMap[GrenadeType];
}

//Switches Player's Grenade Type. E.g. From Frag to Plasma and Plasma to Frag.
void ANotHaloPlayerCharacter::SwitchGrenadeType()
{
	int GrenadeArrayLength = GrenadeMap.Num();

	if (GrenadeArrayLength < 2)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to switch GrenadeTypes! Check if there's at least two Grenade entries in the Player Character Blueprint!"))
		return;
	}
	
	int Index = 0;
	int NextIndex = 0;
	int TotalGrenadeCount = 0;

	//Initial loop to find the index of the Current Grenade
	//Also checking total grenade count here
	//Function will exit early if the Player Character has no GrenadeTypes
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
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("Unable to switch GrenadeTypes! Player has no grenades left!"))
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
		if (GrenadeMap[GrenadeTypes[NextIndex]] > 0)
		{
			SetCurrentGrenadeType(GrenadeTypes[NextIndex]);
			return;
		}

		NextIndex++;
	}

	UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to switch GrenadeTypes! Could not find valid Grenade type to switch to!"))
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






