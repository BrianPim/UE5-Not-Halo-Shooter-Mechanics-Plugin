// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloTeamData.h"
#include "GameFramework/GameModeBase.h"
#include "NotHaloGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NOTHALOSHOOTERMECHANICS_API ANotHaloGameModeBase : public AGameModeBase
{
public:
protected:
private:
	UPROPERTY(EditAnywhere)
	TArray<FNotHaloTeamData> Teams = {};

	static constexpr bool BaseAssassinationsAllowed = true;

	UPROPERTY(EditAnywhere)
	bool AssassinationsAllowed = BaseAssassinationsAllowed;
	
	GENERATED_BODY()
};
