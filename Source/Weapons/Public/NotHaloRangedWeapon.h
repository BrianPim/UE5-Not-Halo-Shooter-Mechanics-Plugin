// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloProjectile.h"
#include "NotHaloWeaponBase.h"
#include "NotHaloRangedWeapon.generated.h"

UCLASS()
class WEAPONS_API ANotHaloRangedWeapon : public ANotHaloWeaponBase
{
public:
	// Sets default values for this actor's properties
	ANotHaloRangedWeapon();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void UseWeapon() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon|Ranged Weapon")
	TSubclassOf<ANotHaloProjectile> ProjectileToSpawn;
	
	
	GENERATED_BODY()
};
