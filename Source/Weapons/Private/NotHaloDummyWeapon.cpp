// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloDummyWeapon.h"


// Sets default values
ANotHaloDummyWeapon::ANotHaloDummyWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
	RootComponent = WeaponMesh;

	WeaponMesh->SetCastHiddenShadow(true);
}

// Called when the game starts or when spawned
void ANotHaloDummyWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANotHaloDummyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

