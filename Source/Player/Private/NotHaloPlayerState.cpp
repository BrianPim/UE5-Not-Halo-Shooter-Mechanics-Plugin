// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloPlayerState.h"
#include "NotHaloTeamData.h"
#include "NotHaloWeaponBase.h"

TSubclassOf<ANotHaloWeaponBase> ANotHaloPlayerState::GetInitialPrimaryWeapon()
{
	return InitialPrimaryWeapon;
}

void ANotHaloPlayerState::SetInitialPrimaryWeapon(TSubclassOf<ANotHaloWeaponBase> NewInitialPrimaryWeapon)
{
	InitialPrimaryWeapon = NewInitialPrimaryWeapon;
}

TSubclassOf<ANotHaloWeaponBase> ANotHaloPlayerState::GetInitialSecondaryWeapon()
{
	return InitialSecondaryWeapon;
}

void ANotHaloPlayerState::SetInitialSecondaryWeapon(TSubclassOf<ANotHaloWeaponBase> NewInitialSecondaryWeapon)
{
	InitialSecondaryWeapon = NewInitialSecondaryWeapon;
}

TArray<FNotHaloGrenadeData> ANotHaloPlayerState::GetInitialGrenadeData()
{
	return InitialGrenades;
}

void ANotHaloPlayerState::SetInitialGrenadeData(TArray<FNotHaloGrenadeData> NewGrenadeData)
{
	InitialGrenades = {};

	for (FNotHaloGrenadeData Data : NewGrenadeData)
	{
		InitialGrenades.Add(Data);
	}
}

int ANotHaloPlayerState::GetKills()
{
	return Kills;
}

void ANotHaloPlayerState::SetKills(int NewKills)
{
	Kills = NewKills;
}

int ANotHaloPlayerState::GetAssists()
{
	return Assists;
}

void ANotHaloPlayerState::SetAssists(int NewAssists)
{
	Assists = NewAssists;
}

int ANotHaloPlayerState::GetDeaths()
{
	return Deaths;
}

void ANotHaloPlayerState::SetDeaths(int NewDeaths)
{
	Deaths = NewDeaths;
}

FNotHaloTeamData ANotHaloPlayerState::GetTeam()
{
	return CurrentTeam;
}

void ANotHaloPlayerState::SetTeam(FNotHaloTeamData NewTeam)
{
	CurrentTeam = NewTeam;
}
