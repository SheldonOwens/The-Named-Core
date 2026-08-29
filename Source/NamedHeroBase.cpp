// NamedHeroBase.cpp

#include "NamedHeroBase.h"
#include "ClickToMoveComponent.h"
#include "NamedAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"

ANamedHeroBase::ANamedHeroBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ClickToMoveComponent = CreateDefaultSubobject<UClickToMoveComponent>(TEXT("ClickToMoveComponent"));

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	// Mixed: predicted local activation with server correction, matches
	// the LocalPredicted policy set on UNamedGameplayAbility.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UNamedAttributeSet>(TEXT("AttributeSet"));

	// Baseline CharacterMovementComponent setup. These are also
	// Blueprint-tunable on child classes via Class Defaults, same as
	// the ClickToMoveComponent properties -- this is the other half of
	// the "feel" knob set (raw speed/accel), separate from path
	// following behavior (corner smoothing, rotation).
	if (UCharacterMovementComponent* CharMovement = GetCharacterMovement())
	{
		CharMovement->bOrientRotationToMovement = false; // we handle rotation ourselves in ClickToMoveComponent
		CharMovement->RotationRate = FRotator(0.0f, 0.0f, 0.0f);
		CharMovement->bUseControllerDesiredRotation = false;
		CharMovement->MaxWalkSpeed = 600.0f;
		CharMovement->MaxAcceleration = 2048.0f;
		CharMovement->BrakingDecelerationWalking = 2048.0f;
		CharMovement->GroundFriction = 8.0f;
	}
}

void ANamedHeroBase::BeginPlay()
{
	Super::BeginPlay();
}

UAbilitySystemComponent* ANamedHeroBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ANamedHeroBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server-side ability actor info init. Runs when the server assigns
	// a controller to this pawn.
	InitializeAbilitySystem();
}

void ANamedHeroBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Client-side ability actor info init. GAS requires both this and
	// PossessedBy to call InitAbilityActorInfo -- skipping either one is
	// the classic cause of "works on server, broken on remote clients."
	InitializeAbilitySystem();
}

void ANamedHeroBase::InitializeAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ANamedHeroBase::RequestMoveToLocation(const FVector& Destination)
{
	switch (MovementInputMode)
	{
	case EMovementInputMode::ClickToMove:
		if (ClickToMoveComponent)
		{
			ClickToMoveComponent->RequestMoveToLocation(Destination);
		}
		break;

	case EMovementInputMode::Direct:
		// Placeholder for future DirectInputMoveComponent. Intentionally
		// unimplemented -- see MovementIntentProvider.h for the interface
		// a new component would fulfill.
		break;
	}
}

void ANamedHeroBase::CancelMovement()
{
	if (ClickToMoveComponent)
	{
		ClickToMoveComponent->CancelMovement();
	}
}

bool ANamedHeroBase::IsHeroMoving() const
{
	return ClickToMoveComponent ? ClickToMoveComponent->IsMoving() : false;
}
