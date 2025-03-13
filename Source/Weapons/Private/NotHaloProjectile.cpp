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
	ProjectileMovementComponent->InitialSpeed = BaseProjectileSpeed;
	ProjectileMovementComponent->MaxSpeed = BaseProjectileSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = BaseProjectileGravityScale;
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

//Damage
//Returns Damage done to Health
int ANotHaloProjectile::GetHealthDamage()
{
	return HealthDamage;
}

//Sets Health Damage
void ANotHaloProjectile::SetHealthDamage(int NewHealthDamage)
{
	HealthDamage = NewHealthDamage;
}

//Returns Damage done to Shield
int ANotHaloProjectile::GetShieldDamage()
{
	return ShieldDamage;
}

//Sets Shield Damage
void ANotHaloProjectile::SetShieldDamage(int NewShieldDamage)
{
	ShieldDamage = NewShieldDamage;
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