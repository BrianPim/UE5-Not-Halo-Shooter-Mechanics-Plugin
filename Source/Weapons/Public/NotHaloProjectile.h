﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloWeaponBase.h"
#include "GameFramework/Actor.h"
#include "NotHaloProjectile.generated.h"

//Forward Declarations
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS(Abstract)
class WEAPONS_API ANotHaloProjectile : public AActor
{
public:
	// Sets default values for this actor's properties
	ANotHaloProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void SetParentWeapon(ANotHaloWeaponBase* Weapon);

	int GetDamage();
	void SetDamage(int NewDamage);
	FVector GetProjectileDirection();
	FVector GetProjectileVelocity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Weapon
	UPROPERTY(BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ANotHaloWeaponBase> ParentWeapon;
	
	//Damage
	static constexpr int BaseDamage = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	int Damage;
	
	GENERATED_BODY()
};
