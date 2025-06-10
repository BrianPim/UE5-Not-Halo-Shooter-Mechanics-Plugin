// Copyright (c) 2025 Brian Pimentel

#pragma once

#include "CoreMinimal.h"
#include "NotHaloInterface_InteractableBase.h"
#include "GameFramework/Actor.h"
#include "NotHaloInteractableBase.generated.h"

//Forward Declarations
class ANotHaloPlayerCharacter;
class USphereComponent;

//Delegate Declarations
//Delegate for player interaction.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteraction,
											ANotHaloInteractableBase*, Interactable,
											ANotHaloPlayerCharacter*, PlayerCharacter);

UCLASS(Abstract)
class INTERACTABLES_API ANotHaloInteractableBase : public AActor, public INotHaloInterface_InteractableBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANotHaloInteractableBase();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void Interact_Implementation(AActor* PlayerActor) override;

	UFUNCTION(BlueprintPure)
	bool GetInteractionEnabled();
	
	UFUNCTION(BlueprintCallable)
	void SetInteractionEnabled(bool Enable);
	
	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Interactable", meta = (ToolTip = "Hook up additional interaction functionality to this."))
	FInteraction OnInteraction;

	UPROPERTY(BlueprintAssignable, Category = "Interactable", meta = (ToolTip = "Hook up additional interaction functionality to this."))
	FInteraction OnPlayerOverlap;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION(NetMulticast, Reliable)
	void NETMULTICAST_HandleInteraction(ANotHaloPlayerCharacter* PlayerCharacter);

	UFUNCTION(NetMulticast, Reliable)
	void NETMULTICAST_HandlePlayerOverlap(ANotHaloPlayerCharacter* PlayerCharacter);
	
	static constexpr bool BaseInteractionEnabled = true;
	static constexpr bool BaseDestroyOnInteract = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable", meta = (AllowPrivateAccess = true))
	TObjectPtr<USphereComponent> SphereComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable", meta = (AllowPrivateAccess = true))
	bool InteractionEnabled = BaseInteractionEnabled;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable", meta = (AllowPrivateAccess = true))
	bool DestroyOnInteract = BaseDestroyOnInteract;

	UFUNCTION()
	void OnOverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
