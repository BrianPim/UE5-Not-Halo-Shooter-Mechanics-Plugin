// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloGrenade.h"
#include "NotHaloWeaponsLogging.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ANotHaloGrenade::ANotHaloGrenade()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = GrenadeMesh;
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement Component"));
}

// Called when the game starts or when spawned
void ANotHaloGrenade::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s Initialized!"), *GrenadeName);
}

// Called every frame
void ANotHaloGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Detonated)
	{
		if (FuseDurationRemaining > 0.0f && (!SticksToSurfaces || (SticksToSurfaces && StuckToSurface)))
		{
			FuseDurationRemaining -= DeltaTime;
		}
		else
		{
			DetonateGrenade();
			Detonated = true;
		}
	}
}

FString ANotHaloGrenade::GetGrenadeName() const
{
	return GrenadeName;
}

void ANotHaloGrenade::DetonateGrenade()
{
	OnGrenadeDetonation.Broadcast();

	UE_LOG(NotHaloWeaponsLogging, Display, TEXT("%s Detonated!"), *GrenadeName);
	
	Destroy();
}

