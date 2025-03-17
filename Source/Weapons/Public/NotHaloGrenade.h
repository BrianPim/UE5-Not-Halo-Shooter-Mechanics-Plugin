// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NotHaloGrenade.generated.h"

//Forward Declarations
class UProjectileMovementComponent;

//Delegate Declarations
//Delegate for grenade detonating.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGrenadeDetonated);

UCLASS(Abstract)
class WEAPONS_API ANotHaloGrenade : public AActor
{
	GENERATED_BODY()

public:
	ANotHaloGrenade();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Grenade|Util")
	FString GetGrenadeName() const;

	UFUNCTION(BlueprintCallable, Category = "Grenade|Functionality")
	void DetonateGrenade();

	//Mesh
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> GrenadeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovementComponent;

	//Delegates
	FGrenadeDetonated OnGrenadeDetonation;

protected:
	virtual void BeginPlay() override;

private:
	const FString GrenadeName = GetName();
	
	//Functionality
	static constexpr bool BaseSticksToSurfaces = false;
	static constexpr float BaseGrenadeFuseDuration = 2.0f;
	static constexpr float BaseGrenadeThrowForce = 5.0f;
	
	bool Detonated = false;
	
	//Determines if the Grenade sticks to surfaces, including other players
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Functionality", meta = (AllowPrivateAccess = "true"))
	bool SticksToSurfaces = BaseSticksToSurfaces;
	bool StuckToSurface = false;

	//Duration until the Grenade explodes. If SticksToSurfaces is set to be true, then the
	//fuse duration won't start until the Grenade attaches to a surface.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Functionality", meta = (AllowPrivateAccess = "true"))
	float GrenadeFuseDuration = BaseGrenadeFuseDuration;
	float FuseDurationRemaining = GrenadeFuseDuration;

	//Force added to the Grenade, in an externally provided aimed direction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Functionality", meta = (AllowPrivateAccess = "true"))
	float GrenadeThrowForce = BaseGrenadeThrowForce;

	//Damage
	static constexpr float BaseGrenadeDamage = 100.0f;
	static constexpr float BaseGrenadeExplosionRadius = 5.0f;

	//Grenade Damage at the center of the explosion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Damage", meta = (AllowPrivateAccess = "true"))
	float GrenadeDamage = BaseGrenadeDamage;

	//Explosion Radius; Damage falls off the further the player is from the center of the explosion.
	//
	//E.g. A Grenade Damage value of 100 and Explosion Radius of 5 would cause the player take 50 Damage
	//if they are 2.5 units from the center of the explosion.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Damage", meta = (AllowPrivateAccess = "true"))
	float GrenadeExplosionRadius = BaseGrenadeExplosionRadius;
};