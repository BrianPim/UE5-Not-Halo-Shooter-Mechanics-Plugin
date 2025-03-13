// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NotHaloWeapon.generated.h"

//Forward Declarations

UENUM(BlueprintType)
enum class EScopeType: uint8
{
	ShortRange UMETA(Tooltip = "Utilizes Binoculars. De-scopes on firing weapon. E.g. Assault Rifle"),
	MediumRange UMETA(Tooltip = "Single-stage scope. E.g. Battle Rifle"),
	LongRange UMETA(Tooltip = "Two-stage scope. E.g. Sniper Rifle")
};

//Delegate Declarations
//Delegate for integer stats are changed
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWeaponIntStatUpdated,
												int32, OldValue,
												int32, NewValue,
												int32, MaxValue);

//Delegate for weapon being fired.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponFired);

UCLASS(Abstract)
class WEAPONS_API ANotHaloWeapon : public AActor
{
public:
	// Sets default values for this actor's properties
	ANotHaloWeapon();
	
	virtual void Tick(float DeltaTime) override;
	
	//Weapon Functionality
	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void UseWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void StartReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality", meta = (ToolTip = "Forces immediate ammo reload."))
	void ForceReloadWeapon();
	
	UFUNCTION() //Not included as a Blueprint callable to avoid confusion.
	void ReloadWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	float GetCooldownDuration();

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	float GetCooldownRemaining();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void SetCooldownDuration(float NewCooldown);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void StartCooldown();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void ForceFinishCooldown();

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	float GetEffectiveRange();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void SetEffectiveRange(float NewRange);

	//Ammo
	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int GetMagazineAmmoCount();

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int GetReserveAmmoCount();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void SetMagazineAmmoCount(int NewAmmo);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void SetReserveAmmoCount(int NewAmmo);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void AddToMagazineAmmoCount(int DeltaAmmo);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	void AddToReserveAmmoCount(int DeltaAmmo);

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Functionality", meta = (ToolTip = "Hook up weapon functionality to this."))
	FWeaponFired OnWeaponUsed;
	
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
	FWeaponIntStatUpdated OnMagazineAmmoCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
	FWeaponIntStatUpdated OnReserveAmmoCountChanged;
	
protected:
	virtual void BeginPlay() override;

private:
	//Functionality
	static constexpr float BaseUseWeaponCooldown = 0.5f;
	static constexpr float BaseReloadDuration = 1.0f;
	static constexpr float BaseEffectiveRange = 30;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float UseWeaponCooldown = BaseUseWeaponCooldown;
	float UseWeaponCooldownRemaining = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float ReloadDuration = BaseReloadDuration;
	float ReloadDurationRemaining = 0.0f;
	bool Reloading = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float EffectiveRange = BaseEffectiveRange;
	
	//Ammo
	static constexpr int BaseMaxReserveAmmoCount = 15;
	static constexpr int BaseMaxMagazineAmmoCount = 5;
	static constexpr int BaseAmmoConsumedOnUse = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int MaxReserveAmmoCount = BaseMaxReserveAmmoCount;
	int CurrentReserveAmmoCount = BaseMaxReserveAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int MaxMagazineAmmoCount = BaseMaxMagazineAmmoCount;
	int CurrentMagazineAmmoCount = BaseMaxMagazineAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int AmmoConsumedOnUse = BaseAmmoConsumedOnUse;
	
	GENERATED_BODY()
};
