// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NotHaloTeamData.generated.h"

/**
 * 
 */
USTRUCT()
struct NOTHALOSHOOTERMECHANICS_API FNotHaloTeamData
{
public:
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	FName TeamName = "BaseTeam";
	UPROPERTY(EditDefaultsOnly)
	FColor TeamColor = FColor::White;
protected:
private:

};
