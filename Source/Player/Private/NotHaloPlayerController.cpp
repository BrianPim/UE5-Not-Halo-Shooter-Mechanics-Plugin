// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NotHaloPlayerCharacter.h"
#include "NotHaloPlayerLogging.h"

void ANotHaloPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	//Store a reference to the Player's Pawn
	PlayerCharacter = Cast<ANotHaloPlayerCharacter>(aPawn);
	checkf(PlayerCharacter, TEXT("ANotHaloPlayerController derived classes should only possess ANotHaloPlayerCharacter derived pawns"));
	
	//Store a reference to the HUD
	// PlayerHud = Cast<AHudBB>(GetHUD());
	// checkf(PlayerHud, TEXT("Unable to get reference to Hud"));

	//Get a reference to the EnhancedInputComponent
	EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	checkf(EnhancedInputComponent, TEXT("Unable to get reference to the EnhancedInputComponent"));

	//Get local player subsystem
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> InputSubsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	checkf(InputSubsystem, TEXT("Unable to get reference to the EnhancedInputLocalPlayerSubsystem"));

	//Wipe existing mappings, and add our mapping
	checkf(InputMappingContext, TEXT("InputMappingContext was not specified"))
	InputSubsystem->ClearAllMappings();
	InputSubsystem->AddMappingContext(InputMappingContext, 0);

	//Bind input actions
	//Only attempt to bind if valid value was provided
	if (ActionMove)
	{
		EnhancedInputComponent->BindAction(ActionMove, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleMove);
	}

	if (ActionLook)
	{
		EnhancedInputComponent->BindAction(ActionLook, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleLook);
	}

	if (ActionJump)
	{
		EnhancedInputComponent->BindAction(ActionJump, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleJump);
	}

	if (ActionCrouch)
	{
		EnhancedInputComponent->BindAction(ActionCrouch, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleCrouch);
	}

	if (ActionUseWeapon)
	{
		EnhancedInputComponent->BindAction(ActionUseWeapon, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleUseWeapon);
	}

	if (ActionUseWeaponEnd)
	{
		EnhancedInputComponent->BindAction(ActionUseWeaponEnd, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleUseWeaponEnd);
	}

	if (ActionSwitchWeapon)
	{
		EnhancedInputComponent->BindAction(ActionSwitchWeapon, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleSwitchWeapon);
	}

	if (ActionReloadWeapon)
	{
		EnhancedInputComponent->BindAction(ActionReloadWeapon, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleReloadWeapon);
	}

	if (ActionUseGrenade)
	{
		EnhancedInputComponent->BindAction(ActionUseGrenade, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleUseGrenade);
	}

	if (ActionSwitchGrenade)
	{
		EnhancedInputComponent->BindAction(ActionSwitchGrenade, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleSwitchGrenade);
	}

	if (ActionMelee)
	{
		EnhancedInputComponent->BindAction(ActionMelee, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleMelee);
	}

	if (ActionScope)
	{
		EnhancedInputComponent->BindAction(ActionScope, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleScope);
	}

	if (ActionInteract)
	{
		EnhancedInputComponent->BindAction(ActionInteract, ETriggerEvent::Triggered, this, &ANotHaloPlayerController::HandleInteract);
	}
}

void ANotHaloPlayerController::OnUnPossess()
{
	//Unbind
	EnhancedInputComponent->ClearActionBindings();

	//Call parent method
	Super::OnUnPossess();
}

void ANotHaloPlayerController::HandleLook(const FInputActionValue& ActionValue)
{
	const FVector2D LookAxisVector = ActionValue.Get<FVector2D>();

	//Add yaw and pitch input to controller
	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void ANotHaloPlayerController::HandleMove(const FInputActionValue& ActionValue)
{
	const FVector2D MovementVector = ActionValue.Get<FVector2D>();

	//Add movement to Player's Character Pawn
	if (PlayerCharacter)
	{
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), MovementVector.Y);
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), MovementVector.X);
	}
}

void ANotHaloPlayerController::HandleJump()
{
	//Make the Player's Character Pawn jump, disable crouch if active
	if (PlayerCharacter)
	{
		PlayerCharacter->UnCrouch();
		PlayerCharacter->Jump();
	}
}

void ANotHaloPlayerController::HandleCrouch()
{
	if (PlayerCharacter)
	{
		if (PlayerCharacter->bIsCrouched)
		{
			PlayerCharacter->UnCrouch();
		}
		else
		{
			PlayerCharacter->Crouch();
		}
	}
}

void ANotHaloPlayerController::HandleUseWeapon()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UseWeapon();
	}
}

void ANotHaloPlayerController::HandleUseWeaponEnd()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UseWeaponEnd();
	}
}

void ANotHaloPlayerController::HandleSwitchWeapon()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SwitchWeapon();
	}
}

void ANotHaloPlayerController::HandleReloadWeapon()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->ReloadWeapon();
	}
}

void ANotHaloPlayerController::HandleUseGrenade()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->ThrowGrenade();
	}
}

void ANotHaloPlayerController::HandleSwitchGrenade()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->SwitchGrenadeType();
	}
}

void ANotHaloPlayerController::HandleMelee()
{
	if (PlayerCharacter)
	{
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("Melee"));
	}
}

void ANotHaloPlayerController::HandleScope()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->UseScope();
	}
}

void ANotHaloPlayerController::HandleInteract()
{
	if (PlayerCharacter)
	{
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("Interact"));
	}
}
