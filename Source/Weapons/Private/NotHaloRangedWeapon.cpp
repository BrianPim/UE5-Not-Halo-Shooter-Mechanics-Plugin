// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloRangedWeapon.h"

#include "NotHaloWeaponsLogging.h"


// Sets default values
ANotHaloRangedWeapon::ANotHaloRangedWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANotHaloRangedWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANotHaloRangedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANotHaloRangedWeapon::UseWeapon()
{
	Super::UseWeapon();
	
	const FVector SpawnLocation = FVector::ZeroVector;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = HolderPawn;
	PlayerWeaponSpawnParams.Instigator = HolderPawn;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if (ProjectileToSpawn)
	{
		GetWorld()->SpawnActor<ANotHaloProjectile>(ProjectileToSpawn, SpawnLocation, SpawnRotation, PlayerWeaponSpawnParams);
	}
	else
	{
		UE_LOG(NotHaloWeaponsLogging, Error, TEXT("%s has no Projectile assigned to it!"), *WeaponName)
	}
}

