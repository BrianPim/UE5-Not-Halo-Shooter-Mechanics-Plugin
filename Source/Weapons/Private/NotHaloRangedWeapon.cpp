// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloRangedWeapon.h"

#include "NotHaloWeaponsLogging.h"
#include "Engine/SkeletalMeshSocket.h"


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

	ProjectileSpawnSocket = WeaponMesh->GetSocketByName(ProjectileSpawnSocketName);

	if (!ProjectileSpawnSocket)
	{
		UE_LOG(NotHaloWeaponsLogging, Error, TEXT("Projectile Spawn Socket not found on %s!"), *WeaponName)
	}
}

// Called every frame
void ANotHaloRangedWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANotHaloRangedWeapon::UseWeapon()
{
	if (!CanUseWeapon())
		return;
	
	Super::UseWeapon();

	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = HolderPawn;
	PlayerWeaponSpawnParams.Instigator = HolderPawn;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	if (ProjectileToSpawn)
	{
		GetWorld()->SpawnActor<ANotHaloProjectile>(ProjectileToSpawn, ProjectileSpawnSocket->GetSocketTransform(WeaponMesh), PlayerWeaponSpawnParams);
	}
	else
	{
		UE_LOG(NotHaloWeaponsLogging, Error, TEXT("%s has no Projectile assigned to it!"), *WeaponName)
	}
}

