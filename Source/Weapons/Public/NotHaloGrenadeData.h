// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NotHaloGrenade.h"
#include "UObject/Object.h"
#include "NotHaloGrenadeData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct WEAPONS_API FNotHaloGrenadeData
{
public:
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ANotHaloGrenade> GrenadeType;
	UPROPERTY(EditDefaultsOnly)
	int MaxGrenadeCount = 2;
	UPROPERTY(EditDefaultsOnly)
	int StartingGrenadeCount = 2;
protected:
private:

};
