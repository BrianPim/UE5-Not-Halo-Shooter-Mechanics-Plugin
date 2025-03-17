// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NotHaloWeaponBase.generated.h"

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
class WEAPONS_API ANotHaloWeaponBase : public AActor
{
public:
	// Sets default values for this actor's properties
	ANotHaloWeaponBase();
	
	virtual void Tick(float DeltaTime) override;

	//Weapon Functionality
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	virtual void UseWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	bool CanUseWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void StartReloadWeapon();

	//Intended to be hooked up to an AnimNotify
	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void IncrementalReload();

	//Called by IncrementalReload when complete
	UFUNCTION()
	void FinishReloadWeapon();

	//Not in use by default, but instantly handling reloading in a single function may be a desirable feature
	UFUNCTION()
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

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void DropWeapon(FVector Position);

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

	//Util
	
	UFUNCTION(BlueprintPure, Category = "Weapon|Util")
	FString GetWeaponName();

	UFUNCTION(BlueprintPure, Category = "Weapon|Util")
	APawn* GetWeaponHolderPawn();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Util")
	void SetWeaponHolderPawn(APawn* NewHolder);

	//Mesh

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	//Delegates
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Functionality", meta = (ToolTip = "Hook up weapon functionality to this."))
	FWeaponFired OnWeaponUsed;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Functionality")
	FWeaponFired OnReloadStarted;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Functionality")
	FWeaponFired OnReloadFinished;
	
	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
	FWeaponIntStatUpdated OnMagazineAmmoCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Ammo")
	FWeaponIntStatUpdated OnReserveAmmoCountChanged;
	
protected:
	virtual void BeginPlay() override;
	const FString WeaponName = GetName();
	UPROPERTY()
	TObjectPtr<APawn> HolderPawn = nullptr;

private:
	//Functionality
	static constexpr float BaseUseWeaponCooldown = 0.25f;
	static constexpr float BaseEffectiveRange = 30.0f;
	static constexpr float BaseTimeToReload = 1.0f;

	static constexpr bool BaseAllowReloadCancel = false;
	static constexpr bool BaseHandleReloadWithDuration = true;

	//Time until the player is allowed to use the weapon again
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float UseWeaponCooldown = BaseUseWeaponCooldown;
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float UseWeaponCooldownRemaining = 0.0f;

	//Allows for reload to be cancelled with other actions, E.g. using weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	bool AllowReloadCancel = BaseAllowReloadCancel;

	//Set to false if reload is being handled elsewhere, E.g. through an animation montage notify
	UPROPERTY(EditAnywhere, Category = "Weapon|Functionality")
	bool HandleReloadWithDuration = BaseHandleReloadWithDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "HandleReloadWithDuration == true", EditCondition = "HandleReloadWithDuration == true"))
	float TimeToReload = BaseTimeToReload;
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "HandleReloadWithDuration == true", EditCondition = "HandleReloadWithDuration == true"))
	float ReloadDurationRemaining = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float EffectiveRange = BaseEffectiveRange;
	
	//Ammo
	static constexpr int BaseMaxReserveAmmoCount = 15;
	static constexpr int BaseMaxMagazineAmmoCount = 5;
	static constexpr int BaseAmmoConsumedOnUse = 1;
	static constexpr int BaseAmmoAddedPerReloadAnimLoop = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int MaxReserveAmmoCount = BaseMaxReserveAmmoCount;
	int CurrentReserveAmmoCount = BaseMaxReserveAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int MaxMagazineAmmoCount = BaseMaxMagazineAmmoCount;
	int CurrentMagazineAmmoCount = BaseMaxMagazineAmmoCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int AmmoConsumedOnUse = BaseAmmoConsumedOnUse;

	//For weapons with a looping Reload animation (E.g. loading shotgun shells)
	//Set to 0 if not applicable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo", meta = (AllowPrivateAccess = "true"))
	int AmmoAddedPerReloadAnimLoop = BaseAmmoAddedPerReloadAnimLoop;
	bool Reloading = false;
	
	GENERATED_BODY()
};
