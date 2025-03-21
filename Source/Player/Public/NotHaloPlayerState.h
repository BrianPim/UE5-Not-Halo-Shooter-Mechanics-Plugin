// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloGrenadeData.h"
#include "NotHaloTeamData.h"
#include "GameFramework/PlayerState.h"
#include "NotHaloPlayerState.generated.h"

//Forward Declarations
class ANotHaloWeaponBase;

UCLASS()
class PLAYER_API ANotHaloPlayerState : public APlayerState
{
public:
	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	TSubclassOf<ANotHaloWeaponBase> GetInitialPrimaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetInitialPrimaryWeapon(TSubclassOf<ANotHaloWeaponBase> NewInitialPrimaryWeapon);

	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	TSubclassOf<ANotHaloWeaponBase> GetInitialSecondaryWeapon();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetInitialSecondaryWeapon(TSubclassOf<ANotHaloWeaponBase> NewInitialSecondaryWeapon);

	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	TArray<FNotHaloGrenadeData> GetInitialGrenadeData();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetInitialGrenadeData(TArray<FNotHaloGrenadeData> NewGrenadeData);
	
	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	int GetKills();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetKills(int NewKills);

	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	int GetAssists();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetAssists(int NewAssists);

	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	int GetDeaths();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetDeaths(int NewDeaths);

	UFUNCTION(BlueprintPure, Category = "NotHaloPlayerState")
	FNotHaloTeamData GetTeam();
	UFUNCTION(BlueprintCallable, Category = "NotHaloPlayerState")
	void SetTeam(FNotHaloTeamData NewTeam);
	
protected:

private:
	static constexpr int BaseKills = 0;
	static constexpr int BaseAssists = 0;
	static constexpr int BaseDeaths = 0;

	UPROPERTY()
	TSubclassOf<ANotHaloWeaponBase> InitialPrimaryWeapon = nullptr;
	UPROPERTY()
	TSubclassOf<ANotHaloWeaponBase> InitialSecondaryWeapon = nullptr;

	UPROPERTY()
	TArray<FNotHaloGrenadeData> InitialGrenades = {};

	int Kills = BaseKills;
	int Assists = BaseAssists;
	int Deaths = BaseDeaths;
	
	FNotHaloTeamData CurrentTeam;
	
	GENERATED_BODY()
};
