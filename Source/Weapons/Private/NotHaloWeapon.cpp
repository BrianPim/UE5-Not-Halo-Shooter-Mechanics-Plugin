// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloWeapon.h"

#include "NotHaloWeaponsLogging.h"

// Sets default values
ANotHaloWeapon::ANotHaloWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANotHaloWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANotHaloWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UseWeaponCooldownRemaining > 0.0f)
	{
		UseWeaponCooldownRemaining -= DeltaTime;
	}

	if (Reloading)
	{
		if (ReloadDurationRemaining > 0.0f)
		{
			ReloadDurationRemaining -= DeltaTime;
		}
		else
		{
			Reloading = false;
			ReloadWeapon();
		}
	}
}

//Functionality
//Use Weapon
void ANotHaloWeapon::UseWeapon()
{
	if (UseWeaponCooldownRemaining > 0.0f)
	{
		UE_LOG(NotHaloWeapons, Warning, TEXT("A Weapon cannot be used while it's on cooldown."));
		return;
	}

	if (GetMagazineAmmoCount() <= 0)
	{
		if (GetReserveAmmoCount() <= 0)
		{
			UE_LOG(NotHaloWeapons, Display, TEXT("Weapon is completely out of ammo!"));
		}
		else
		{
			UE_LOG(NotHaloWeapons, Display, TEXT("Weapon has no ammo in the magazine. Reloading."));

			StartReloadWeapon();
		}
		
		return;
	}

	//Further weapon functionality handled via Blueprint that references OnWeaponUsed delegate.
	OnWeaponUsed.Broadcast();
	UE_LOG(NotHaloWeapons, Display, TEXT("Weapon has been used! What happens next should be handled through a delegate in the Weapon's Blueprint."));

	AddToMagazineAmmoCount(-AmmoConsumedOnUse);
	StartCooldown();
}

//Starts the reload process
void ANotHaloWeapon::StartReloadWeapon()
{
	if (Reloading)
	{
		UE_LOG(NotHaloWeapons, Warning, TEXT("Weapon is already in the middle of being reloaded."));
		return;
	}

	if (CurrentMagazineAmmoCount >= MaxMagazineAmmoCount)
	{
		UE_LOG(NotHaloWeapons, Warning, TEXT("A Weapon cannot be reloaded if the magazine is full."));
		return;
	}

	UE_LOG(NotHaloWeapons, Display, TEXT("Starting Weapon reload."));
	
	//TODO Play Reload Animation
	
	ReloadDurationRemaining = ReloadDuration;
	Reloading = true;
}

//Force Weapon to be reloaded without delay
void ANotHaloWeapon::ForceReloadWeapon()
{
	ReloadWeapon();
}

//Reload Weapon
void ANotHaloWeapon::ReloadWeapon()
{
	if (CurrentReserveAmmoCount <= 0)
	{
		UE_LOG(NotHaloWeapons, Warning, TEXT("A Weapon cannot be reloaded if there's no Ammo in reserve."));
		return;
	}

	if (CurrentMagazineAmmoCount >= MaxMagazineAmmoCount)
	{
		UE_LOG(NotHaloWeapons, Warning, TEXT("A Weapon cannot be reloaded if the magazine is full."));
		return;
	}

	int OldMagazineAmmoCount = CurrentMagazineAmmoCount;
	int DeltaAmmo = MaxMagazineAmmoCount - OldMagazineAmmoCount;

	if (DeltaAmmo > CurrentReserveAmmoCount)
	{
		DeltaAmmo = CurrentReserveAmmoCount;
	}
		
	AddToMagazineAmmoCount(DeltaAmmo);
	AddToReserveAmmoCount(-DeltaAmmo);
	
	UE_LOG(NotHaloWeapons, Display, TEXT("Weapon has been reloaded!"));
}

//Returns the Weapon's Cooldown duration
float ANotHaloWeapon::GetCooldownDuration()
{
	return UseWeaponCooldown;
}

//Returns the current duration remaining on the Weapon's Cooldown
float ANotHaloWeapon::GetCooldownRemaining()
{
	return UseWeaponCooldownRemaining;
}

//Sets a custom duration for Weapon Cooldowns
void ANotHaloWeapon::SetCooldownDuration(float NewCooldown)
{
	UseWeaponCooldown = NewCooldown;
	UseWeaponCooldown = FMath::Clamp(UseWeaponCooldown, 0, BIG_NUMBER);
}

//Start Weapon Cooldown, cannot be used if Active Cooldown > 0
void ANotHaloWeapon::StartCooldown()
{
	UseWeaponCooldownRemaining = UseWeaponCooldown;
}

//Force Weapon Cooldown to finish
void ANotHaloWeapon::ForceFinishCooldown()
{
	UseWeaponCooldownRemaining = 0;
}

//Returns Effective Range
float ANotHaloWeapon::GetEffectiveRange()
{
	return EffectiveRange;
}

//Sets Effective Range
void ANotHaloWeapon::SetEffectiveRange(float NewRange)
{
	EffectiveRange = NewRange;
}

//Ammo
//Returns Current Magazine Ammo Count
int ANotHaloWeapon::GetMagazineAmmoCount()
{
	return CurrentMagazineAmmoCount;
}

//Returns Current Reserve Ammo Count
int ANotHaloWeapon::GetReserveAmmoCount()
{
	return CurrentReserveAmmoCount;
}

//Sets Current Magazine Ammo Count
void ANotHaloWeapon::SetMagazineAmmoCount(int NewAmmo)
{
	int OldAmmo = CurrentMagazineAmmoCount;
	
	CurrentMagazineAmmoCount = NewAmmo;
	CurrentMagazineAmmoCount = FMath::Clamp(CurrentMagazineAmmoCount, 0, MaxMagazineAmmoCount);

	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentMagazineAmmoCount, MaxMagazineAmmoCount);
}

//Sets Current Reserve Ammo Count
void ANotHaloWeapon::SetReserveAmmoCount(int NewAmmo)
{
	int OldAmmo = CurrentReserveAmmoCount;
	
	CurrentReserveAmmoCount = NewAmmo;
	CurrentReserveAmmoCount = FMath::Clamp(CurrentReserveAmmoCount, 0, MaxReserveAmmoCount);

	OnReserveAmmoCountChanged.Broadcast(OldAmmo, CurrentReserveAmmoCount, MaxReserveAmmoCount);
}

//Adds delta to Current Magazine Ammo Count
void ANotHaloWeapon::AddToMagazineAmmoCount(int DeltaAmmo)
{
	int OldAmmo = CurrentMagazineAmmoCount;
	
	CurrentMagazineAmmoCount += DeltaAmmo;
	CurrentMagazineAmmoCount = FMath::Clamp(CurrentMagazineAmmoCount, 0, MaxMagazineAmmoCount);

	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentMagazineAmmoCount, MaxMagazineAmmoCount);
}

//Adds delta to Current Reserve Ammo Count
void ANotHaloWeapon::AddToReserveAmmoCount(int DeltaAmmo)
{
	int OldAmmo = CurrentReserveAmmoCount;
	
	CurrentReserveAmmoCount += DeltaAmmo;
	CurrentReserveAmmoCount = FMath::Clamp(CurrentReserveAmmoCount, 0, MaxReserveAmmoCount);

	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentReserveAmmoCount, MaxReserveAmmoCount);
}

