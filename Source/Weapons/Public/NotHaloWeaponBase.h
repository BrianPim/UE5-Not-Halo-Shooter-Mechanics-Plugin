// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NotHaloWeaponBase.generated.h"

UENUM(BlueprintType)
enum class EScopeType: uint8
{
	Binoculars UMETA(Tooltip = "Utilizes Binoculars. De-scopes on firing weapon. E.g. Assault Rifle"),
	Scope UMETA(Tooltip = "Scoped weapon, defined in the details panel. E.g. Battle Rifle, Sniper Rifle"),
};

UENUM(BlueprintType)
enum class EFiringMode: uint8
{
	SemiAuto UMETA(Tooltip = "One weapon use per input down."),
	BurstFire UMETA(Tooltip = "X uses per input down in rapid succession, defined in the details panel."),
	FullAuto UMETA(Tooltip = "Weapon is used until input is released or ammo runs out.")
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

	//Only used if FiringMode = FullAuto
	void UseWeaponEnd();

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
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void SetCooldownDuration(float NewCooldown);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void StartCooldown();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void EndCooldown();

	//TODO Potentially make this a private function
	void EndBurstFireCooldown();

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	float GetEffectiveRange();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void SetEffectiveRange(float NewRange);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Functionality")
	void DropWeapon(FVector Position);

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	EScopeType GetScopeType();

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	float GetZoomMultiplierAtIndex(int Index);

	UFUNCTION(BlueprintPure, Category = "Weapon|Functionality")
	int GetNumOfScopedZoomStages();

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

	//Mesh

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Mesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	//Util
	
	UFUNCTION(BlueprintPure, Category = "Weapon|Util")
	FString GetWeaponName();

	UFUNCTION(BlueprintPure, Category = "Weapon|Util")
	APawn* GetWeaponHolderPawn();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Util")
	void SetWeaponHolderPawn(APawn* NewHolder);

	UFUNCTION(BlueprintPure, Category = "Weapon|Util")
	bool GetWeaponInUse();

	UFUNCTION(BlueprintPure, Category = "Weapon|Util")
	bool GetZoomAllowed();

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
	static constexpr int BaseBurstFireNumberOfShots = 3;
	
	static constexpr float BaseUseWeaponCooldown = 0.25f;
	static constexpr float BaseEffectiveRange = 30.0f;
	static constexpr float BaseZoom = 2.0f;
	static constexpr float BaseBurstFireTimeBetweenShots = 0.1f;

	static constexpr EScopeType BaseScopeType = EScopeType::Binoculars;
	static constexpr EFiringMode BaseFiringMode = EFiringMode::SemiAuto;

	bool ZoomAllowed = true;
	bool WeaponInUse = false;
	
	int BurstFireQueue = 0;
	
	//Time until the player is allowed to use the weapon again
	//For Burst Fire weapons this time starts AFTER the burst is complete
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	float UseWeaponCooldown = BaseUseWeaponCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	bool HeadshotsAllowed = false;

	UPROPERTY(EditAnywhere, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	EScopeType ScopeType = BaseScopeType;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	TArray<float> ScopedZoomStages = { BaseZoom };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	EFiringMode FiringMode = BaseFiringMode;
	
	UPROPERTY(EditAnywhere, Category = "Weapon|Functionality", meta = (EditCondition = "FiringMode == EFiringMode::BurstFire"))
	int BurstFireNumberOfShots = BaseBurstFireNumberOfShots;

	UPROPERTY(EditAnywhere, Category = "Weapon|Functionality", meta = (EditCondition = "FiringMode == EFiringMode::BurstFire"))
	float BurstFireTimeBetweenShots = BaseBurstFireTimeBetweenShots;
	
	FTimerHandle WeaponCooldownTimerHandle;
	FTimerHandle BurstFireTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	bool WeaponCooldownIsActive;
	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Functionality", meta = (AllowPrivateAccess = "true"))
	bool BurstFireCooldownIsActive;
	
	//Reloading
	static constexpr float BaseTimeToReload = 1.0f;
	static constexpr bool BaseAllowReloadCancel = false;
	static constexpr bool BaseHandleReloadWithDuration = true;

	//Allows for reload to be cancelled with other actions, E.g. using weapon
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Reloading", meta = (AllowPrivateAccess = "true"))
	bool AllowReloadCancel = BaseAllowReloadCancel;

	//Set to false if reload is being handled elsewhere, E.g. through an animation montage notify
	UPROPERTY(EditAnywhere, Category = "Weapon|Reloading")
	bool HandleReloadWithDuration = BaseHandleReloadWithDuration;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Reloading", meta = (AllowPrivateAccess = "HandleReloadWithDuration == true", EditCondition = "HandleReloadWithDuration == true"))
	float TimeToReload = BaseTimeToReload;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon|Reloading", meta = (AllowPrivateAccess = "true"))
	float EffectiveRange = BaseEffectiveRange;
	
	FTimerHandle ReloadTimerHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon|Reloading", meta = (AllowPrivateAccess = "true"))
	bool WeaponReloadIsActive;

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
	
	GENERATED_BODY()
};
