// Fill out your copyright notice in the Description page of Project Settings.


#include "NotHaloPlayerCharacter.h"
#include "NotHaloDummyWeapon.h"
#include "NotHaloGrenade.h"
#include "NotHaloPlayerController.h"
#include "NotHaloPlayerLogging.h"
#include "NotHaloTeamData.h"
#include "NotHaloWeaponBase.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
ANotHaloPlayerCharacter::ANotHaloPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	FirstPersonCamera->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	FirstPersonArms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Player Arms"));
	FirstPersonArms->AttachToComponent(FirstPersonCamera, FAttachmentTransformRules::KeepRelativeTransform);
	FirstPersonArms->SetCastShadow(false);

	GetMesh()->SetCastShadow(true);
}

// Called when the game starts or when spawned
void ANotHaloPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	BaseFOV = FirstPersonCamera->FieldOfView;

	FirstPersonArmsWeaponSocket = FirstPersonArms->GetSocketByName(PrimaryWeaponSocketName);
	ThirdPersonPrimaryWeaponSocket = GetMesh()->GetSocketByName(PrimaryWeaponSocketName);
	ThirdPersonSecondaryWeaponSocket = GetMesh()->GetSocketByName(SecondaryWeaponSocketName);

	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = this;
	PlayerWeaponSpawnParams.Instigator = this;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//TODO Grenade shit
	
	//GrenadeSpawnSocket = GetMesh()->GetSocketByName(GrenadeSpawnSocketName);

	//if (!GrenadeSpawnSocket)
	//{
	//	UE_LOG(NotHaloPlayerLogging, Error, TEXT("Grenade Spawn Socket not found on %s!"), *GetName())
	//}
	
	if (InitialPrimaryWeapon)
	{
		PrimaryWeapon = GetWorld()->SpawnActor<ANotHaloWeaponBase>(InitialPrimaryWeapon, FVector::ZeroVector,
															FRotator::ZeroRotator, PlayerWeaponSpawnParams);
		PrimaryWeapon->SetWeaponHolderPawn(this);

		ThirdPersonPrimaryWeapon = GetWorld()->SpawnActor<ANotHaloDummyWeapon>(FVector::ZeroVector,
																FRotator::ZeroRotator, PlayerWeaponSpawnParams);
		RefreshPrimaryWeaponModel();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("No initial Primary Weapon assigned to the player! Is this intentional?"))
	}

	if (InitialSecondaryWeapon)
	{
		SecondaryWeapon = GetWorld()->SpawnActor<ANotHaloWeaponBase>(InitialSecondaryWeapon, FVector::ZeroVector,
																FRotator::ZeroRotator, PlayerWeaponSpawnParams);
		SecondaryWeapon->SetWeaponHolderPawn(this);
		
		ThirdPersonSecondaryWeapon = GetWorld()->SpawnActor<ANotHaloDummyWeapon>(FVector::ZeroVector,
																FRotator::ZeroRotator, PlayerWeaponSpawnParams);
		RefreshSecondaryWeaponModel();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("No initial Secondary Weapon assigned to the player! Is this intentional?"))
	}
	
	if (GrenadeTypes.Num() > 0)
	{
		for(int i = 0; i < GrenadeTypes.Num(); i++)
		{
			GrenadeMap.Add(GrenadeTypes[i], MaxGrenadeCount); //TODO Handle Grenade Count via Game Mode
		}
	
		SetCurrentGrenadeType(GrenadeTypes[0]);
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("No Grenade Types assigned to the player! Is this intentional?"))
	}

	OnWeaponsInitialized.Broadcast();
}

//PossessedBy only runs on the host
void ANotHaloPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	CLIENT_HandlePossess(NewController);
}

//RPC - Sets up Enhanced Input for the client controlling this player
void ANotHaloPlayerCharacter::CLIENT_HandlePossess_Implementation(AController* NewController)
{
	IsInFirstPersonCamera = IsLocallyControlled();

	RefreshPlayerModel();
	
	if (!IsLocallyControlled()) return;

	if (TObjectPtr<ANotHaloPlayerController> NotHaloController = Cast<ANotHaloPlayerController>(NewController))
	{
		NotHaloController->SetupEnhancedInput();
	}
}

// Called every frame
void ANotHaloPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

#pragma region Input & Movement
// Called to bind functionality to input
void ANotHaloPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANotHaloPlayerCharacter::AddMovementInput(FVector WorldDirection, float ScaleValue, bool bForce)
{
	Super::AddMovementInput(WorldDirection, ScaleValue, bForce);
}

void ANotHaloPlayerCharacter::Jump()
{
	Super::Jump();
}

void ANotHaloPlayerCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);
}

bool ANotHaloPlayerCharacter::InputAllowed()
{
	return !IsDead;
}

#pragma endregion

#pragma region Health & Shield

void ANotHaloPlayerCharacter::NotHaloApplyDamage(ANotHaloPlayerCharacter* Caster, int Damage)
{
	if (CurrentHealth <= 0)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("%s is already dead!"), *GetName())
		return;
	}

	int OldShield = CurrentShield;

	if (CurrentShield > 0)
	{
		UpdateShield(-Damage);
	}

	int OverflowDamage = OldShield - Damage;

	if (OverflowDamage < 0)
	{
		UpdateHealth(OverflowDamage);
	}

	if (CurrentHealth <= 0)
	{
		Kill(Caster);
	}

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s took %d Damage from %s!"), *GetName(), Damage, *Caster->GetName())
}

void ANotHaloPlayerCharacter::Assassination(ANotHaloPlayerCharacter* Caster)
{
	Kill(Caster);
	UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s was assassinated by %s!"), *GetName(), *Caster->GetName())
}

void ANotHaloPlayerCharacter::Kill(ANotHaloPlayerCharacter* Caster)
{
	Die(false);
	
	UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s was killed by %s!"), *GetName(), *Caster->GetName())

	//First value is Killer, second value is the Victim
	OnPlayerKilled.Broadcast(Caster, this);
}

void ANotHaloPlayerCharacter::Die(bool Suicide)
{
	OriginalMeshRelativeLocation = GetMesh()->GetRelativeLocation();
	OriginalMeshRelativeRotation = GetMesh()->GetRelativeRotation();
	
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	
	if (RespawnAfterDeath)
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ANotHaloPlayerCharacter::Respawn,
											Suicide ? TimeToRespawnAfterSuicide : TimeToRespawnAfterKill, false);
	}

	IsDead = true;
	
	OnPlayerDied.Broadcast();
}

void ANotHaloPlayerCharacter::Respawn()
{
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));
	GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	GetMesh()->SetRelativeLocationAndRotation(OriginalMeshRelativeLocation, OriginalMeshRelativeRotation);
	
	CurrentHealth = MaxHealth;
	CurrentShield = MaxShield;
	IsDead = false;

	SetActorTransform(FTransform::Identity); //TODO set respawn position using game manager
	
	OnPlayerRespawned.Broadcast();
}

//Health
//Returns Current Health
int ANotHaloPlayerCharacter::GetHealth()
{
	return CurrentHealth;
}

//Returns Max Health
int ANotHaloPlayerCharacter::GetMaxHealth()
{
	return MaxHealth;
}

//Update Player's Current Health Value
void ANotHaloPlayerCharacter::UpdateHealth(int DeltaHealth)
{
	if (CurrentHealth <= 0)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("%s is already dead!"), *GetName())
		return;
	}

	int OldValue = CurrentHealth;
	
	CurrentHealth += DeltaHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth, -1, MaxHealth);

	if (CurrentHealth != OldValue)
	{
		OnHealthChanged.Broadcast(OldValue, CurrentHealth, MaxHealth);
	}

	if (CurrentHealth <= 0)
	{
		//Player is dead!
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s has Died!"), *GetName())
		
		OnPlayerDied.Broadcast();
	}
}

//Shield
//Returns Current Shield
int ANotHaloPlayerCharacter::GetShield()
{
	return CurrentShield;
}

//Returns Max Shield
int ANotHaloPlayerCharacter::GetMaxShield()
{
	return MaxShield;
}

//Update Player's Current Shield Value
void ANotHaloPlayerCharacter::UpdateShield(int DeltaShield)
{
	int OldValue = CurrentShield;
	
	CurrentShield += DeltaShield;
	CurrentShield = FMath::Clamp(CurrentShield, -1, MaxShield);

	if (CurrentShield != OldValue)
	{
		OnShieldChanged.Broadcast(OldValue, CurrentShield, MaxShield);
	}
}
#pragma endregion 

#pragma region Weapons
//Returns Primary Weapon
ANotHaloWeaponBase* ANotHaloPlayerCharacter::GetPrimaryWeapon()
{
	return PrimaryWeapon;
}

//Returns Secondary Weapon
ANotHaloWeaponBase* ANotHaloPlayerCharacter::GetSecondaryWeapon()
{
	return SecondaryWeapon;
}

void ANotHaloPlayerCharacter::UseWeapon()
{
	if (PrimaryWeapon)
	{
		if (PrimaryWeapon->GetWeaponInUse())
		{
			UE_LOG(NotHaloPlayerLogging, Warning, TEXT("Weapon already in use!"));
			return;
		}
		
		if (PrimaryWeapon->GetScopeType() == EScopeType::Binoculars && PrimaryWeapon->GetNumOfScopedZoomStages() > 0)
		{
			UnScope();
		}
		
		PrimaryWeapon->UseWeapon();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to USE weapon!"));
	}
}

void ANotHaloPlayerCharacter::UseWeaponEnd()
{
	if (PrimaryWeapon)
	{
	 	PrimaryWeapon->UseWeaponEnd();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to END use weapon!"));
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("How the hell did you even manage to get here?"));
	}
}

void ANotHaloPlayerCharacter::ReloadWeapon()
{
	if (PrimaryWeapon)
	{
		PrimaryWeapon->StartReloadWeapon();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to RELOAD weapon!"));
	}
}

//Switches Player's Weapons; Primary becomes Secondary and Secondary becomes Primary
void ANotHaloPlayerCharacter::SwitchWeapon()
{
	if (!PrimaryWeapon)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to SWITCH weapons!"));
		return;
	}

	if (!SecondaryWeapon)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Secondary Weapon is null! Unable to SWITCH weapons!"));
		return;
	}

	UnScope();
	
	TObjectPtr<ANotHaloWeaponBase> WeaponToSwitch = PrimaryWeapon;

	PrimaryWeapon = SecondaryWeapon;
	SecondaryWeapon = WeaponToSwitch;

	RefreshPrimaryWeaponModel();
	RefreshSecondaryWeaponModel();

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("Switched Weapon from %s to %s"), *SecondaryWeapon->GetWeaponName(), *PrimaryWeapon->GetWeaponName());
	
	OnWeaponChanged.Broadcast();
}

//Drops Player's current Primary Weapon for a new Weapon
void ANotHaloPlayerCharacter::PickUpNewWeapon(ANotHaloWeaponBase* NewWeapon)
{
	UnScope();
	
	if (PrimaryWeapon)
	{
		TObjectPtr<ANotHaloWeaponBase> WeaponToDrop = PrimaryWeapon;
		WeaponToDrop->SetWeaponHolderPawn(nullptr);
	}
	
	PrimaryWeapon = NewWeapon;
	PrimaryWeapon->SetWeaponHolderPawn(this);

	RefreshPrimaryWeaponModel();
	
	//TODO Drop Weapon
	
	OnWeaponChanged.Broadcast();
}

void ANotHaloPlayerCharacter::RefreshPrimaryWeaponModel()
{
	if (PrimaryWeapon && FirstPersonArmsWeaponSocket && ThirdPersonPrimaryWeaponSocket)
	{
		ThirdPersonPrimaryWeapon->WeaponMesh->SetSkeletalMeshAsset(PrimaryWeapon->WeaponMesh->GetSkeletalMeshAsset());

		int Index = 0;
		for(UMaterialInterface* Material : PrimaryWeapon->WeaponMesh->GetMaterials())
		{
			ThirdPersonPrimaryWeapon->WeaponMesh->SetMaterial(Index, Material);
			Index++;
		}
			
		PrimaryWeapon->WeaponMesh->SetVisibility(IsInFirstPersonCamera);
		ThirdPersonPrimaryWeapon->WeaponMesh->SetVisibility(!IsInFirstPersonCamera);
		
		FirstPersonArmsWeaponSocket->AttachActor(PrimaryWeapon, FirstPersonArms);
		ThirdPersonPrimaryWeaponSocket->AttachActor(ThirdPersonPrimaryWeapon, GetMesh());

		RefreshPlayerModel();
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to refresh Primary Weapon!"))
	}
}

void ANotHaloPlayerCharacter::RefreshSecondaryWeaponModel()
{
	if (SecondaryWeapon && FirstPersonArmsWeaponSocket && ThirdPersonSecondaryWeaponSocket)
	{
		ThirdPersonSecondaryWeapon->WeaponMesh->SetSkeletalMeshAsset(SecondaryWeapon->WeaponMesh->GetSkeletalMeshAsset());

		int Index = 0;
		for(UMaterialInterface* Material : SecondaryWeapon->WeaponMesh->GetMaterials())
		{
			ThirdPersonSecondaryWeapon->WeaponMesh->SetMaterial(Index, Material);
			Index++;
		}
		
		SecondaryWeapon->WeaponMesh->SetVisibility(false);
		ThirdPersonSecondaryWeapon->WeaponMesh->SetVisibility(!IsInFirstPersonCamera);
		
		FirstPersonArmsWeaponSocket->AttachActor(SecondaryWeapon, FirstPersonArms);
		ThirdPersonSecondaryWeaponSocket->AttachActor(ThirdPersonSecondaryWeapon, GetMesh());
	}
	else
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to refresh Secondary Weapon!"))
	}
}

void ANotHaloPlayerCharacter::UseScope()
{
	if (!PrimaryWeapon)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to SCOPE!"))
		return;
	}

	if (!PrimaryWeapon->GetZoomAllowed())
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("Not allowed to use the scope right now!"))
		return;
	}

	if (CurrentScopeLevel < PrimaryWeapon->GetNumOfScopedZoomStages())
	{
		FirstPersonCamera->SetFieldOfView(BaseFOV / PrimaryWeapon->GetZoomMultiplierAtIndex(CurrentScopeLevel));
		CurrentScopeLevel++;
	}
	else
	{
		UnScope();
	}
}

void ANotHaloPlayerCharacter::UseScopeCustomZoom(float CustomZoom)
{
	if (!PrimaryWeapon)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Primary Weapon is null! Unable to SCOPE!"))
	}

	FirstPersonCamera->SetFieldOfView(BaseFOV / CustomZoom);

	//If the player uses the zoom input again after a custom zoom is passed we want to return the FOV to its base state
	CurrentScopeLevel = 10000; 
}

void ANotHaloPlayerCharacter::UnScope()
{
	FirstPersonCamera->SetFieldOfView(BaseFOV);
	CurrentScopeLevel = 0;
}
#pragma endregion 

#pragma region Grenades
//Throws Grenade
void ANotHaloPlayerCharacter::ThrowGrenade()
{
	if (!CurrentGrenade)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Current Grenade is null! Unable to THROW grenade!"));
		return;
	}

	if (GrenadeMap[CurrentGrenade] == 0)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("Player has no more grenades!"))
		return;
	}

	if (GrenadeCooldownActive)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("A Grenade cannot be thrown while it's on cooldown."));
		return;
	}
	
	if (!CanThrowGrenade)
	{
		UE_LOG(NotHaloPlayerLogging, Warning, TEXT("CanThrowGrenade is false!"));
		return;
	}

	//TODO give proper location, give vector to travel along
	const FVector SpawnLocation = FVector::ZeroVector;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters PlayerWeaponSpawnParams;
	PlayerWeaponSpawnParams.Owner = this;
	PlayerWeaponSpawnParams.Instigator = this;
	PlayerWeaponSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	TObjectPtr<ANotHaloGrenade> Grenade = GetWorld()->SpawnActor<ANotHaloGrenade>(CurrentGrenade, SpawnLocation,
																					SpawnRotation, PlayerWeaponSpawnParams);
	AddGrenadeCount(CurrentGrenade, -1);

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("%s Remaining: %d"), *CurrentGrenade->GetClass()->GetName(), GrenadeMap[CurrentGrenade])

	if (GrenadeMap[CurrentGrenade] == 0)
	{
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("Player has run out of %ss. Attempting to switch."), *CurrentGrenade->GetClass()->GetName())
		SwitchGrenadeType();
	}

	GrenadeCooldownActive = true;
	GetWorld()->GetTimerManager().SetTimer(GrenadeCooldownTimerHandle, this, &ANotHaloPlayerCharacter::SetGrenadeCooldownComplete,
											ThrowGrenadeCooldown, false);;
}

void ANotHaloPlayerCharacter::SetCanThrowGrenade(bool NewCanThrowGrenade)
{
	CanThrowGrenade = NewCanThrowGrenade;
}

void ANotHaloPlayerCharacter::SetGrenadeCooldownComplete()
{
	UE_LOG(NotHaloPlayerLogging, Display, TEXT("Grenade Cooldown complete!"));
	GrenadeCooldownActive = false;
}

//Sets Current Grenade Type
void ANotHaloPlayerCharacter::SetCurrentGrenadeType(TSubclassOf<ANotHaloGrenade> NewGrenadeType)
{
	if (!NewGrenadeType)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Provided Grenade Type is null! Unable to SET grenade type!"));
		return;
	}
	
	CurrentGrenade = NewGrenadeType;

	UE_LOG(NotHaloPlayerLogging, Display, TEXT("Player switched to Grenade Type: %s"), *CurrentGrenade->GetName())

	OnGrenadeTypeChanged.Broadcast();
}

//Returns Current Grenade Type
TSubclassOf<ANotHaloGrenade> ANotHaloPlayerCharacter::GetCurrentGrenadeType()
{
	if (!CurrentGrenade)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Provided Grenade Type is null! Unable to RETURN grenade type!"));
		return nullptr;
	}
	
	return CurrentGrenade;
}

//Returns Current Grenade Count of provided Grenade Type
int ANotHaloPlayerCharacter::GetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType)
{
	if (!GrenadeMap[GrenadeType])
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Grenade type not found! Make sure to assign it inside of the Player Character Blueprint!"));
		return 0;
	}
	
	return GrenadeMap[GrenadeType];
}

//Switches Player's Grenade Type. E.g. From Frag to Plasma and Plasma to Frag.
void ANotHaloPlayerCharacter::SwitchGrenadeType()
{
	int GrenadeArrayLength = GrenadeMap.Num();

	if (GrenadeArrayLength < 2)
	{
		UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to switch GrenadeTypes! Check if there's at least two Grenade entries in the Player Character Blueprint!"))
		return;
	}
	
	int Index = 0;
	int NextIndex = 0;
	int TotalGrenadeCount = 0;

	//Initial loop to find the index of the Current Grenade
	//Also checking total grenade count here
	//Function will exit early if the Player Character has no GrenadeTypes
	for (auto& Grenade : GrenadeMap)
	{
		if (Grenade.Key == CurrentGrenade)
		{
			NextIndex = Index + 1;
		}

		Index++;
		TotalGrenadeCount += Grenade.Value;
	}
	
	if (TotalGrenadeCount == 0)
	{
		UE_LOG(NotHaloPlayerLogging, Display, TEXT("Unable to switch GrenadeTypes! Player has no grenades left!"))
		return;
	}
	
	//Second loop, looking for a valid Grenade Type to switch to
	for (int i = 0; i < GrenadeArrayLength; i++)
	{
		if (NextIndex >= GrenadeArrayLength)
		{
			NextIndex = 0;
		}
		
		//If true, valid Grenade type To switch to has been found and the function can be ended early
		if (GrenadeMap[GrenadeTypes[NextIndex]] > 0)
		{
			SetCurrentGrenadeType(GrenadeTypes[NextIndex]);
			return;
		}

		NextIndex++;
	}

	UE_LOG(NotHaloPlayerLogging, Error, TEXT("Unable to switch GrenadeTypes! Could not find valid Grenade type to switch to!"))
}

//Sets Count of provided Grenade Type
void ANotHaloPlayerCharacter::SetGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType, int NewGrenades)
{
	int OldValue = GrenadeMap[GrenadeType];

	GrenadeMap[GrenadeType] = NewGrenades;
	GrenadeMap[GrenadeType] = FMath::Clamp(GrenadeMap[GrenadeType], 0, MaxGrenadeCount);
	
	OnGrenadeCountChanged.Broadcast(OldValue, GrenadeMap[GrenadeType], MaxGrenadeCount);
}

//Adds to Count of provided Grenade Type
void ANotHaloPlayerCharacter::AddGrenadeCount(TSubclassOf<ANotHaloGrenade> GrenadeType, int DeltaGrenades)
{
	int OldValue = GrenadeMap[GrenadeType];
	GrenadeMap[GrenadeType] += DeltaGrenades;
	GrenadeMap[GrenadeType] = FMath::Clamp(GrenadeMap[GrenadeType], 0, MaxGrenadeCount);
	
	OnGrenadeCountChanged.Broadcast(OldValue, GrenadeMap[GrenadeType], MaxGrenadeCount);
}

float ANotHaloPlayerCharacter::GetThrowGrenadeCooldown()
{
	return ThrowGrenadeCooldown;
}

void ANotHaloPlayerCharacter::SetThrowGrenadeCooldown(float NewCooldown)
{
	ThrowGrenadeCooldown = NewCooldown;
	ThrowGrenadeCooldown = FMath::Clamp(ThrowGrenadeCooldown, 0, BIG_NUMBER);
}

#pragma endregion 

//Melee
void ANotHaloPlayerCharacter::PerformMelee()
{
	UE_LOG(NotHaloPlayerLogging, Display, TEXT("Melee"));

	UWorld* World = GetWorld();

	FVector PlayerForwardVector = FirstPersonCamera->GetForwardVector();

	FVector StartPosition = FirstPersonCamera->GetComponentLocation();
	FVector EndPosition = StartPosition + (PlayerForwardVector * MeleeRange);

	World->DebugDrawTraceTag = FName(TEXT("MeleeTrace"));

	FCollisionQueryParams MeleeTraceParams = FCollisionQueryParams(FName(TEXT("MeleeTrace")), true, this);
	
	FHitResult Hit;
	World->LineTraceSingleByChannel(Hit, StartPosition, EndPosition, ECC_Camera, MeleeTraceParams);

	if (Hit.bBlockingHit)
	{
		AActor* HitActor = Hit.GetActor();
		
		if (HitActor)
		{
			UE_LOG(NotHaloPlayerLogging, Display, TEXT("Hit!"));

			if (ANotHaloPlayerCharacter* ValidPlayerCharacter = Cast<ANotHaloPlayerCharacter>(HitActor))
			{
				float DotProduct = FVector::DotProduct(ValidPlayerCharacter->GetActorForwardVector().GetSafeNormal(),
													   PlayerForwardVector.GetSafeNormal());

				UE_LOG(NotHaloPlayerLogging, Display, TEXT("Dot Product: %f"), DotProduct);
				
				if (DotProduct >= ValidPlayerCharacter->AssassinationThreshold)
				{
					ValidPlayerCharacter->Assassination(this);
				}
				else
				{
					ValidPlayerCharacter->NotHaloApplyDamage(this, MeleeDamage);
				}
			}
		}
	}

	OnMeleeHit.Broadcast(Hit);
}

#pragma region Teams & Scoring
//Sets Player's current Team
void ANotHaloPlayerCharacter::SetTeam(FNotHaloTeamData NewTeam)
{
	FNotHaloTeamData OldTeam = CurrentTeam;
	CurrentTeam = NewTeam;

	OnTeamChanged.Broadcast(OldTeam, CurrentTeam, &CurrentTeam == &NewTeam);
}

//Sets Player's current Score
int ANotHaloPlayerCharacter::GetScore()
{
	return CurrentScore;
}


//Sets Player's current Score
void ANotHaloPlayerCharacter::SetScore(int DeltaScore)
{
	int OldScore = CurrentScore;
	CurrentScore += DeltaScore;

	OnScoreChanged.Broadcast(OldScore, CurrentScore, CurrentScore == OldScore + DeltaScore);
}

void ANotHaloPlayerCharacter::AddKills(int DeltaKills)
{
	Kills += DeltaKills;
}

void ANotHaloPlayerCharacter::AddAssists(int DeltaAssists)
{
	Assists += DeltaAssists;
}

void ANotHaloPlayerCharacter::AddDeaths(int DeltaDeaths)
{
	Deaths += DeltaDeaths;
}
#pragma endregion

//Mesh
void ANotHaloPlayerCharacter::RefreshPlayerModel()
{
	GetMesh()->SetVisibility(!IsInFirstPersonCamera);
	FirstPersonArms->SetVisibility(IsInFirstPersonCamera);
}
