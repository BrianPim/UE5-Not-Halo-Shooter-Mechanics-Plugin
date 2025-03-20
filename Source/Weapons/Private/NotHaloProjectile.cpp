// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


// Sets default values
ANotHaloProjectile::ANotHaloProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

// Called when the game starts or when spawned
void ANotHaloProjectile::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ANotHaloProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANotHaloProjectile::SetParentWeapon(ANotHaloWeaponBase* Weapon)
{
	ParentWeapon = Weapon;
}

//Damage
//Returns Damage
int ANotHaloProjectile::GetDamage()
{
	return Damage;
}

//Sets Damage
void ANotHaloProjectile::SetDamage(int NewDamage)
{
	Damage = NewDamage;
}

//Physics
//Returns Projectile Direction as a normalized version of Velocity
FVector ANotHaloProjectile::GetProjectileDirection()
{
	return ProjectileMovementComponent->Velocity.GetSafeNormal();
}
//Returns Projectile Velocity
FVector ANotHaloProjectile::GetProjectileVelocity()
{
	return ProjectileMovementComponent->Velocity;
}