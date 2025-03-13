// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NotHaloProjectile.generated.h"

//Forward Declarations
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class WEAPONS_API ANotHaloProjectile : public AActor
{
public:
	// Sets default values for this actor's properties
	ANotHaloProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	int GetHealthDamage();
	void SetHealthDamage(int NewHealthDamage);
	int GetShieldDamage();
	void SetShieldDamage(int NewShieldDamage);
	FVector GetProjectileDirection();
	FVector GetProjectileVelocity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	//Functionality
	static constexpr float BaseLifeDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Functionality", meta = (AllowPrivateAccess = "true"))
	float LifeDuration = BaseLifeDuration;
	float LifeDurationRemaining = BaseLifeDuration;
	
	//Damage
	static constexpr int BaseHealthDamage = 10;
	static constexpr int BaseShieldDamage = 10;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Damage", meta = (AllowPrivateAccess = "true"))
	int HealthDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Damage", meta = (AllowPrivateAccess = "true"))
	int ShieldDamage;

	//Physics
	static constexpr float BaseProjectileSpeed = 5000;
	static constexpr float BaseProjectileGravityScale = 0;
	
	GENERATED_BODY()
};
