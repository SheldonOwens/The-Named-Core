// NamedHeroBase.h
// The Named -- hero root class
//
// This is the C++ foundation every champion Blueprint will be a child
// of (e.g. BP_Hero_Tomoe). It owns the movement system but stays
// agnostic about *which* movement mode is active, so adding WASD later
// means writing a new component, not touching this class or any
// champion Blueprint built on top of it.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NamedHeroBase.generated.h"

class UClickToMoveComponent;

UENUM(BlueprintType)
enum class EMovementInputMode : uint8
{
	ClickToMove UMETA(DisplayName = "Click to Move"),
	Direct      UMETA(DisplayName = "Direct (WASD) -- not yet implemented")
};

UCLASS()
class THENAMED_API ANamedHeroBase : public ACharacter
{
	GENERATED_BODY()

public:
	ANamedHeroBase();

	// Which movement mode this hero is currently using. Exposed so we
	// can toggle it per-hero or globally once Direct mode exists.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	EMovementInputMode MovementInputMode = EMovementInputMode::ClickToMove;

	// Entry point for click-to-move input, called from the PlayerController.
	// Routes through the active movement component so callers never need
	// to know which mode is active.
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void RequestMoveToLocation(const FVector& Destination);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void CancelMovement();

	UFUNCTION(BlueprintPure, Category = "Movement")
	bool IsHeroMoving() const;

protected:
	virtual void BeginPlay() override;

	// The active click-to-move component. In the future, a Direct
	// (WASD) component will live alongside this one and
	// MovementInputMode decides which is authoritative -- neither the
	// PlayerController nor any champion Blueprint needs to change when
	// that happens.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UClickToMoveComponent> ClickToMoveComponent;
};
