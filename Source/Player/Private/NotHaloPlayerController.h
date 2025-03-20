// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputLibrary.h"
#include "NotHaloPlayerController.generated.h"

//Forward Declarations
class ANotHaloPlayerCharacter;

UCLASS(Abstract)
class PLAYER_API ANotHaloPlayerController : public APlayerController
{
public:
	//Input Action to map to movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Character Movement")
	TObjectPtr<UInputAction> ActionMove = nullptr;

	//Input Action to map to look
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Movement")
	TObjectPtr<UInputAction> ActionLook = nullptr;

	//Input Action to map to jump
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Movement")
	TObjectPtr<UInputAction> ActionJump = nullptr;

	//Input Action to map to crouch
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Movement")
	TObjectPtr<UInputAction> ActionCrouch = nullptr;
	
	//Input Action to map to use weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionUseWeapon = nullptr;

	//Input Action to map to the end of use weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionUseWeaponEnd = nullptr;

	//Input Action to map to switch weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionSwitchWeapon = nullptr;

	//Input Action to map to reload weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionReloadWeapon = nullptr;

	//Input Action to map to use weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionUseGrenade = nullptr;

	//Input Action to map to switch weapon
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionSwitchGrenade = nullptr;

	//Input Action to map to melee
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionMelee = nullptr;

	//Input Action to map to scope (aim down sights)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Weapons")
	TObjectPtr<UInputAction> ActionScope = nullptr;

	//Input Action to map to interact
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input|Player Interaction")
	TObjectPtr<UInputAction> ActionInteract = nullptr;

	//Input Mapping Context to use
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Input")
	TObjectPtr<UInputMappingContext> InputMappingContext = nullptr;

	void SetupEnhancedInput();

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	void HandleLook(const FInputActionValue& ActionValue);
	void HandleMove(const FInputActionValue& ActionValue);
	void HandleJump();
	void HandleCrouch();
	void HandleUseWeapon();
	void HandleUseWeaponEnd();
	void HandleSwitchWeapon();
	void HandleReloadWeapon();
	void HandleUseGrenade();
	void HandleSwitchGrenade();
	void HandleMelee();
	void HandleScope();
	void HandleInteract();
private:
	//Used to store a reference to the InputComponent cast to an EnhancedInputComponent
	UPROPERTY()
	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = nullptr;

	//Used to store a reference to the pawn we are controlling
	UPROPERTY()
	TObjectPtr<ANotHaloPlayerCharacter> PlayerCharacter = nullptr;

	//Used to store reference to Hud
	//UPROPERTY()
	//TObjectPtr<AHudBB> PlayerHud = nullptr;

	FTimerHandle CharacterInitializationTimerHandle;
	
	GENERATED_BODY()
};
