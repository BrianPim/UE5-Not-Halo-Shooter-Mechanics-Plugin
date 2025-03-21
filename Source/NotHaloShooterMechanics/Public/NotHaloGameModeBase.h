// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloTeamData.h"
#include "NotHaloGrenadeData.h"
#include "NotHaloWeaponBase.h"
#include "GameFramework/GameModeBase.h"
#include "NotHaloGameModeBase.generated.h"

UCLASS(Abstract)
class NOTHALOSHOOTERMECHANICS_API ANotHaloGameModeBase : public AGameModeBase
{
public:
	UPROPERTY(EditAnywhere)
	TArray<FNotHaloTeamData> Teams = {};

	UPROPERTY(EditAnywhere)
	TSubclassOf<ANotHaloWeaponBase> InitialPrimaryWeapon = nullptr;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ANotHaloWeaponBase> InitialSecondaryWeapon = nullptr;
	
	UPROPERTY(EditAnywhere)
	TArray<FNotHaloGrenadeData> Grenades = {};

	UPROPERTY(EditAnywhere)
	bool AssassinationsAllowed = BaseAssassinationsAllowed;
protected:
private:
	static constexpr bool BaseAssassinationsAllowed = true;

	GENERATED_BODY()
};
