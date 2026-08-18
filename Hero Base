// NamedHeroBase.cpp

#include "NamedHeroBase.h"
#include "ClickToMoveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

ANamedHeroBase::ANamedHeroBase()
{
	PrimaryActorTick.bCanEverTick = true;

	ClickToMoveComponent = CreateDefaultSubobject<UClickToMoveComponent>(TEXT("ClickToMoveComponent"));

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
