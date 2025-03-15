// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloWeaponBase.h"
#include "NotHaloWeaponsLogging.h"

// Sets default values
ANotHaloWeaponBase::ANotHaloWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.1f; //We don't need tick to run every frame

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void ANotHaloWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANotHaloWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (UseWeaponCooldownRemaining > 0.0f)
	{
		UseWeaponCooldownRemaining -= DeltaTime;
	}
}

//Functionality
//Use Weapon
void ANotHaloWeaponBase::UseWeapon()
{
	if (UseWeaponCooldownRemaining > 0.0f)
	{
		UE_LOG(NotHaloWeaponsLogging, Warning, TEXT("%s cannot be used while it's on cooldown! Remaining: %f."), *WeaponName, UseWeaponCooldownRemaining);
		return;
	}

	if (GetMagazineAmmoCount() <= 0)
	{
		if (GetReserveAmmoCount() <= 0)
		{
			UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s is completely out of ammo!"), *WeaponName);
		}
		else
		{
			UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s has no ammo in the magazine. Reloading."), *WeaponName);

			StartReloadWeapon();
		}
		
		return;
	}

	//Further weapon functionality handled via Blueprint that references OnWeaponUsed delegate.
	OnWeaponUsed.Broadcast();
	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s has been used! What happens next should be handled through the OnWeaponUsed delegate in it's Blueprint Event Graph."), *WeaponName);

	AddToMagazineAmmoCount(-AmmoConsumedOnUse);
	StartCooldown();
}

//Starts the reload process
void ANotHaloWeaponBase::StartReloadWeapon()
{
	if (Reloading)
	{
		UE_LOG(NotHaloWeaponsLogging, Warning, TEXT("%s is already in the middle of being reloaded."), *WeaponName);
		return;
	}

	if (CurrentReserveAmmoCount <= 0)
	{
		UE_LOG(NotHaloWeaponsLogging, Warning, TEXT("%s cannot be reloaded if there's no Ammo in reserve."), *WeaponName);
		return;
	}

	if (CurrentMagazineAmmoCount >= MaxMagazineAmmoCount)
	{
		UE_LOG(NotHaloWeaponsLogging, Warning, TEXT("%s cannot be reloaded while it's magazine is full."), *WeaponName);
		return;
	}

	Reloading = true;

	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("Starting Weapon Reload for %s."), *WeaponName);
	
	OnReloadStarted.Broadcast();
}

//Reload Weapon
void ANotHaloWeaponBase::FinishReloadWeapon()
{
	Reloading = false;
	
	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s has been reloaded!"), *WeaponName);

	OnReloadFinished.Broadcast();
}

void ANotHaloWeaponBase::ReloadWeapon()
{
	int DeltaAmmo = MaxMagazineAmmoCount - CurrentMagazineAmmoCount;

	if (DeltaAmmo > CurrentReserveAmmoCount)
	{
		DeltaAmmo = CurrentReserveAmmoCount;
	}
	
	AddToMagazineAmmoCount(DeltaAmmo);
	AddToReserveAmmoCount(-DeltaAmmo);
	
	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s has been reloaded!"), *WeaponName);
}

//Returns the Weapon's Cooldown duration
float ANotHaloWeaponBase::GetCooldownDuration()
{
	return UseWeaponCooldown;
}

//Returns the current duration remaining on the Weapon's Cooldown
float ANotHaloWeaponBase::GetCooldownRemaining()
{
	return UseWeaponCooldownRemaining;
}

//Sets a custom duration for Weapon Cooldowns
void ANotHaloWeaponBase::SetCooldownDuration(float NewCooldown)
{
	UseWeaponCooldown = NewCooldown;
	UseWeaponCooldown = FMath::Clamp(UseWeaponCooldown, 0, BIG_NUMBER);
}

//Start Weapon Cooldown, cannot be used if Active Cooldown > 0
void ANotHaloWeaponBase::StartCooldown()
{
	UseWeaponCooldownRemaining = UseWeaponCooldown;
}

//Force Weapon Cooldown to finish
void ANotHaloWeaponBase::ForceFinishCooldown()
{
	UseWeaponCooldownRemaining = 0;
}

//Returns Effective Range
float ANotHaloWeaponBase::GetEffectiveRange()
{
	return EffectiveRange;
}

//Sets Effective Range
void ANotHaloWeaponBase::SetEffectiveRange(float NewRange)
{
	EffectiveRange = NewRange;
}

//Ammo
//Returns Current Magazine Ammo Count
int ANotHaloWeaponBase::GetMagazineAmmoCount()
{
	return CurrentMagazineAmmoCount;
}

//Returns Current Reserve Ammo Count
int ANotHaloWeaponBase::GetReserveAmmoCount()
{
	return CurrentReserveAmmoCount;
}

//Sets Current Magazine Ammo Count
void ANotHaloWeaponBase::SetMagazineAmmoCount(int NewAmmo)
{
	int OldAmmo = CurrentMagazineAmmoCount;
	
	CurrentMagazineAmmoCount = NewAmmo;
	CurrentMagazineAmmoCount = FMath::Clamp(CurrentMagazineAmmoCount, 0, MaxMagazineAmmoCount);

	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s Current Magazine Ammo Count: %d"), *WeaponName, CurrentMagazineAmmoCount);
	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentMagazineAmmoCount, MaxMagazineAmmoCount);
}

//Sets Current Reserve Ammo Count
void ANotHaloWeaponBase::SetReserveAmmoCount(int NewAmmo)
{
	int OldAmmo = CurrentReserveAmmoCount;
	
	CurrentReserveAmmoCount = NewAmmo;
	CurrentReserveAmmoCount = FMath::Clamp(CurrentReserveAmmoCount, 0, MaxReserveAmmoCount);

	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s Current Reserve Ammo Count: %d"), *WeaponName, CurrentReserveAmmoCount);
	OnReserveAmmoCountChanged.Broadcast(OldAmmo, CurrentReserveAmmoCount, MaxReserveAmmoCount);
}

//Adds delta to Current Magazine Ammo Count
void ANotHaloWeaponBase::AddToMagazineAmmoCount(int DeltaAmmo)
{
	int OldAmmo = CurrentMagazineAmmoCount;
	
	CurrentMagazineAmmoCount += DeltaAmmo;
	CurrentMagazineAmmoCount = FMath::Clamp(CurrentMagazineAmmoCount, 0, MaxMagazineAmmoCount);

	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentMagazineAmmoCount, MaxMagazineAmmoCount);
}


void ANotHaloWeaponBase::IncrementalReload()
{
	int DeltaAmmo = AmmoAddedPerReloadAnimLoop;

	if (DeltaAmmo <= 0)
	{
		DeltaAmmo = MaxMagazineAmmoCount - CurrentMagazineAmmoCount;
	}

	if (DeltaAmmo > CurrentReserveAmmoCount)
	{
		DeltaAmmo = CurrentReserveAmmoCount;
	}
	
	AddToMagazineAmmoCount(DeltaAmmo);
	AddToReserveAmmoCount(-DeltaAmmo);

	if (CurrentMagazineAmmoCount >= MaxMagazineAmmoCount || CurrentReserveAmmoCount <= 0)
	{
		FinishReloadWeapon();
	}
}

//Adds delta to Current Reserve Ammo Count
void ANotHaloWeaponBase::AddToReserveAmmoCount(int DeltaAmmo)
{
	int OldAmmo = CurrentReserveAmmoCount;
	
	CurrentReserveAmmoCount += DeltaAmmo;
	CurrentReserveAmmoCount = FMath::Clamp(CurrentReserveAmmoCount, 0, MaxReserveAmmoCount);

	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentReserveAmmoCount, MaxReserveAmmoCount);
}

//Returns Name of Weapon as an FString
FString ANotHaloWeaponBase::GetWeaponName()
{
	return *WeaponName;
}

APawn* ANotHaloWeaponBase::GetWeaponHolderPawn()
{
	return HolderPawn;
}

void ANotHaloWeaponBase::SetWeaponHolderPawn(APawn* NewHolder)
{
	HolderPawn = NewHolder;
}


