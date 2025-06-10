// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NotHaloInterface_InteractableBase.generated.h"

// This class does not need to be modified.
UINTERFACE()
class INTERFACES_API UNotHaloInterface_InteractableBase : public UInterface
{
	GENERATED_BODY()
};

class INTERFACES_API INotHaloInterface_InteractableBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(AActor* PlayerActor);
};