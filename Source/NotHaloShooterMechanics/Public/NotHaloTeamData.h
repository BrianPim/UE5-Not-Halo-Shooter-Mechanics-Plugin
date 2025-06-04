// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NotHaloTeamData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
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
