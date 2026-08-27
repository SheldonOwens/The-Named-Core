// MovementIntentProvider.h
// The Named -- movement architecture
//
// This interface exists so ANamedHeroBase never needs to know whether
// movement is being driven by click-to-move, WASD, or anything else we
// add later. Every input method implements this interface and talks to
// the character through it. Nothing downstream (animation, abilities,
// replication) needs to change when we add a second movement mode.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MovementIntentProvider.generated.h"

UINTERFACE(BlueprintType)
class THENAMEDCORE_API UMovementIntentProvider : public UInterface
{
	GENERATED_BODY()
};

class THENAMEDCORE_API IMovementIntentProvider
{
	GENERATED_BODY()

public:
	// Called when the player issues a move command (click destination,
	// or a resolved WASD direction projected out as a target point).
	virtual void RequestMoveToLocation(const FVector& Destination) = 0;

	// Cancels any in-progress movement immediately (e.g. an ability cast
	// that roots the hero, or a new command overriding the old one).
	virtual void CancelMovement() = 0;

	// Whether this provider currently has an active move in progress.
	// Used by animation/ability systems that care about movement state
	// without caring which provider produced it.
	virtual bool IsMoving() const = 0;
};
