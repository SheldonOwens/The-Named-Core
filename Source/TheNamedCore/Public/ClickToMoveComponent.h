// ClickToMoveComponent.h
// The Named -- movement architecture
//
// Owns nav-mesh path following for the hero. We compute the path
// ourselves (rather than relying on AAIController::MoveToLocation)
// so every part of the feel -- corner smoothing, click responsiveness,
// rotation speed, acceleration -- is something we control and can
// tune live in Blueprint, not something buried in engine defaults.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MovementIntentProvider.h"
#include "ClickToMoveComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS(ClassGroup = (Movement), meta = (BlueprintSpawnableComponent))
class THENAMED_API UClickToMoveComponent : public UActorComponent, public IMovementIntentProvider
{
	GENERATED_BODY()

public:
	UClickToMoveComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// -- IMovementIntentProvider --
	virtual void RequestMoveToLocation(const FVector& Destination) override;
	virtual void CancelMovement() override;
	virtual bool IsMoving() const override;

	// Convenience entry point: raycasts from screen/world click location,
	// projects to nav mesh, and requests the move. Called by the hero's
	// input handling (or directly from a PlayerController).
	UFUNCTION(BlueprintCallable, Category = "Movement|ClickToMove")
	void HandleClickAtWorldLocation(const FVector& ClickWorldLocation);

protected:
	// ---------------------------------------------------------------
	// TUNABLE FEEL -- these are the knobs. Adjust on the Blueprint
	// child of ANamedHeroBase (or a per-hero override) without
	// recompiling. Defaults below are starting points, not final values.
	// ---------------------------------------------------------------

	// How close the character must be to the current path point before
	// we advance to the next one. Too small = hero can stall/oscillate
	// at corners. Too large = corners get visibly cut, feels sloppy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Tuning", meta = (ClampMin = "1.0"))
	float PathPointAcceptanceRadius = 40.0f;

	// Radius used for the *final* destination point specifically.
	// Usually smaller than corner acceptance so the hero stops precisely
	// where the player clicked rather than short of it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Tuning", meta = (ClampMin = "1.0"))
	float FinalDestinationAcceptanceRadius = 15.0f;

	// How quickly the hero rotates to face its current movement
	// direction, in degrees/second. This is one of the biggest
	// contributors to click-to-move "feel" -- too slow feels floaty
	// and unresponsive, too fast feels twitchy/robotic.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Tuning", meta = (ClampMin = "45.0"))
	float RotationInterpSpeed = 720.0f;

	// If true, a new click immediately cancels and replaces the current
	// path (standard MOBA behavior). If false, clicks queue up. Keep
	// true unless you have a specific design reason otherwise.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Tuning")
	bool bNewClickInterruptsCurrentMove = true;

	// Minimum distance (cm) between the hero's current location and a
	// new click for it to register as a new move command. Prevents
	// jitter from double-clicks or accidental micro-clicks on the same
	// spot re-triggering pathing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Tuning", meta = (ClampMin = "0.0"))
	float MinimumClickDistance = 20.0f;

	// Max distance the nav-mesh projection will search from the clicked
	// point. If the click lands somewhere unreachable (off nav mesh,
	// behind geometry), we snap to the nearest valid point within this
	// range instead of silently failing.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Tuning", meta = (ClampMin = "0.0"))
	float NavMeshProjectionExtent = 500.0f;

	// ---------------------------------------------------------------

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwningCharacter = nullptr;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent = nullptr;

	// Current path as a series of world-space points from nav query.
	TArray<FVector> CurrentPath;

	// Index into CurrentPath we're currently walking toward.
	int32 CurrentPathIndex = 0;

	bool bHasActiveMove = false;

	// Builds a path from the character's current location to Destination
	// using the navigation system. Returns false if no path was found.
	bool BuildPathTo(const FVector& Destination);

	// Advances CurrentPathIndex and drives CharacterMovementComponent
	// input toward the current target point. Called every tick while
	// bHasActiveMove is true.
	void TickPathFollowing(float DeltaTime);

	// Smoothly rotates the character to face MoveDirection.
	void ApplyRotation(const FVector& MoveDirection, float DeltaTime);
};
