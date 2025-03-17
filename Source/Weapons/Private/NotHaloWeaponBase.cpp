// Fill out your copyright notice in the Description page of Project Settings.

#include "NotHaloWeaponBase.h"
#include "NotHaloWeaponsLogging.h"

// Sets default values
ANotHaloWeaponBase::ANotHaloWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = .02f;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void ANotHaloWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	CurrentMagazineAmmoCount = MaxMagazineAmmoCount;
	CurrentReserveAmmoCount = MaxReserveAmmoCount;
}

// Called every .02 seconds
void ANotHaloWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Reloading && HandleReloadWithDuration)
	{
		if (ReloadDurationRemaining > 0.0f)
		{
			ReloadDurationRemaining -= DeltaTime;
		}
		else
		{
			FinishReloadWeapon();
		}
	}

	if (UseWeaponCooldownRemaining > 0.0f)
	{
		UseWeaponCooldownRemaining -= DeltaTime;
	}
	else if (FiringMode == EFiringMode::FullAuto && WeaponInUse)
	{
		//Setting this to false to guarantee UseWeapon() doesn't fire multiple times here
		WeaponInUse = false;
		UseWeapon();
	}
	else if (BurstFireQueue > 0)
	{
		UseWeapon();
	}
}

//Functionality
#pragma region Use Weapon
//Use Weapon
void ANotHaloWeaponBase::UseWeapon()
{
	if (!CanUseWeapon())
		return;

	//In case we interrupted reloading
	Reloading = false;
	
	//Burst Fire Mechanics
	//------------------------------------------------------------------------------
	if (FiringMode == EFiringMode::BurstFire)
	{
		if (!WeaponInUse && BurstFireQueue == 0)
		{
			BurstFireQueue = BurstFireNumberOfShots;
		}
		
		BurstFireQueue--;

		if (BurstFireQueue >  0)
		{
			WeaponInUse = true;
			StartBurstFireCooldown();
		}
		else
		{
			WeaponInUse = false;
			StartCooldown();
		}
	}
	//------------------------------------------------------------------------------

	if (FiringMode == EFiringMode::FullAuto)
	{
		ZoomAllowed = false;
		WeaponInUse = true;
	}

	if (FiringMode != EFiringMode::BurstFire)
	{
		StartCooldown();
	}
	
	AddToMagazineAmmoCount(-AmmoConsumedOnUse);

	//Further weapon functionality handled via Blueprint that references OnWeaponUsed delegate.
	OnWeaponUsed.Broadcast();
	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s has been used! What happens next should be handled through the OnWeaponUsed delegate in it's Blueprint Event Graph."), *WeaponName)
}

void ANotHaloWeaponBase::UseWeaponEnd()
{
	ZoomAllowed = true;
	WeaponInUse = false;
}

//Checks if weapon can be used
bool ANotHaloWeaponBase::CanUseWeapon()
{
	if (UseWeaponCooldownRemaining > 0.0f)
	{
		UE_LOG(NotHaloWeaponsLogging, Warning, TEXT("%s cannot be used while it's on cooldown! Remaining: %f."), *WeaponName, UseWeaponCooldownRemaining);
		return false;
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
		
		return false;
	}

	if (Reloading && !AllowReloadCancel)
	{
		return false;
	}

	return true;
}
#pragma endregion

#pragma region Reload Weapon
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

	if (HandleReloadWithDuration)
	{
		ReloadDurationRemaining = TimeToReload;
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

//It may be useful to have a function that reloads the weapon without needing to go through animation stuff
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

//Reloads by X amount each increment; Handled in the Animation Blueprint
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
#pragma endregion

#pragma region Weapon Cooldown
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

//Start Weapon Cooldown, cannot be used if Active Cooldown > 0
void ANotHaloWeaponBase::StartBurstFireCooldown()
{
	UseWeaponCooldownRemaining = BurstFireTimeBetweenShots;
}

//Force Weapon Cooldown to finish
void ANotHaloWeaponBase::ForceFinishCooldown()
{
	UseWeaponCooldownRemaining = 0;
}
#pragma endregion

#pragma region Other Functionality
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

//Sets Weapon position, removes Holder and detaches from sockets
void ANotHaloWeaponBase::DropWeapon(FVector Position)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetWeaponHolderPawn(nullptr);
	SetActorLocation(Position);
}

//Returns Weapon's Scope Type
EScopeType ANotHaloWeaponBase::GetScopeType()
{
	return ScopeType;
}

//Returns the zoom multiplier at the provided zoom stage
float ANotHaloWeaponBase::GetZoomMultiplerAtIndex(int Index)
{
	if (Index >= ScopedZoomStages.Num())
	{
		UE_LOG(NotHaloWeaponsLogging, Error, TEXT("Provided index %d was larger than the size of the array!"), Index)
		return 1;
	}

	return ScopedZoomStages[Index];
}


int ANotHaloWeaponBase::GetNumOfScopedZoomStages()
{
	return ScopedZoomStages.Num();
}
#pragma endregion

//Ammo
#pragma region Magazine Ammo
//Returns Current Magazine Ammo Count
int ANotHaloWeaponBase::GetMagazineAmmoCount()
{
	return CurrentMagazineAmmoCount;
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

//Adds delta to Current Magazine Ammo Count
void ANotHaloWeaponBase::AddToMagazineAmmoCount(int DeltaAmmo)
{
	int OldAmmo = CurrentMagazineAmmoCount;
	
	CurrentMagazineAmmoCount += DeltaAmmo;
	CurrentMagazineAmmoCount = FMath::Clamp(CurrentMagazineAmmoCount, 0, MaxMagazineAmmoCount);
	
	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentMagazineAmmoCount, MaxMagazineAmmoCount);
}
#pragma endregion 

#pragma region Reserve Ammo
//Returns Current Reserve Ammo Count
int ANotHaloWeaponBase::GetReserveAmmoCount()
{
	return CurrentReserveAmmoCount;
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

//Adds delta to Current Reserve Ammo Count
void ANotHaloWeaponBase::AddToReserveAmmoCount(int DeltaAmmo)
{
	int OldAmmo = CurrentReserveAmmoCount;
	
	CurrentReserveAmmoCount += DeltaAmmo;
	CurrentReserveAmmoCount = FMath::Clamp(CurrentReserveAmmoCount, 0, MaxReserveAmmoCount);

	OnMagazineAmmoCountChanged.Broadcast(OldAmmo, CurrentReserveAmmoCount, MaxReserveAmmoCount);
}
#pragma endregion

//Util
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

	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s is now being held by %s"), *WeaponName, *HolderPawn->GetName());
}

bool ANotHaloWeaponBase::GetWeaponInUse()
{
	return WeaponInUse;
}

bool ANotHaloWeaponBase::GetZoomAllowed()
{
	return ZoomAllowed;
}



