// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NotHaloDummyWeapon.generated.h"

UCLASS()
class WEAPONS_API ANotHaloDummyWeapon : public AActor
{
public:
	ANotHaloDummyWeapon();

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;
	
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	GENERATED_BODY()
};
