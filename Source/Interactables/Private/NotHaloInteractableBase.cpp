// Copyright (c) 2025 Brian Pimentel


#include "NotHaloInteractableBase.h"
#include "NotHaloPlayerCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
ANotHaloInteractableBase::ANotHaloInteractableBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereComponent->SetCollisionProfileName(FName("OverlapAllDynamic"));
	SphereComponent->SetSphereRadius(150.0f);
}

// Called when the game starts or when spawned
void ANotHaloInteractableBase::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ANotHaloInteractableBase::OnOverlapStart);
}

// Called every frame
void ANotHaloInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANotHaloInteractableBase::Interact_Implementation(AActor* PlayerActor)
{
	TObjectPtr<ANotHaloPlayerCharacter> PlayerCharacter = Cast<ANotHaloPlayerCharacter>(PlayerActor);

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast to ANotHaloPlayerCharacter!"));
		return;
	}
	
	if (!InteractionEnabled)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player ID %i tried to interact but InteractionEnable is false!"),
																  PlayerCharacter->GetPlayerState()->GetUniqueID());

		return;
	}

	NETMULTICAST_HandleInteraction(PlayerCharacter);
}


void ANotHaloInteractableBase::NETMULTICAST_HandleInteraction_Implementation(ANotHaloPlayerCharacter* PlayerCharacter)
{
	OnInteraction.Broadcast(this, PlayerCharacter);
	
	if (DestroyOnInteract)
	{
		K2_DestroyActor();
	}
}

void ANotHaloInteractableBase::OnOverlapStart(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;

	if (!InteractionEnabled) return;

	if (ANotHaloPlayerCharacter* PlayerCharacter = Cast<ANotHaloPlayerCharacter>(OtherActor))
	{
		NETMULTICAST_HandlePlayerOverlap(PlayerCharacter);
	}
}

void ANotHaloInteractableBase::NETMULTICAST_HandlePlayerOverlap_Implementation(ANotHaloPlayerCharacter* PlayerCharacter)
{
	OnPlayerOverlap.Broadcast(this, PlayerCharacter);
}

bool ANotHaloInteractableBase::GetInteractionEnabled()
{
	return InteractionEnabled;
}

void ANotHaloInteractableBase::SetInteractionEnabled(bool Enable)
{
	InteractionEnabled = Enable;
}





