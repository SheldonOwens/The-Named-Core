// NamedHeroBase.h
// The Named -- hero root class
//
// This is the C++ foundation every champion Blueprint will be a child
// of (e.g. BP_Hero_Tomoe). It owns the movement system but stays
// agnostic about *which* movement mode is active, so adding WASD later
// means writing a new component, not touching this class or any
// champion Blueprint built on top of it.
//
// Also owns the GAS AbilitySystemComponent and base AttributeSet.
// Champion-specific abilities are granted here later (or by a
// champion's Blueprint child class); this base class just sets up the
// plumbing every hero needs regardless of kit.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "NamedHeroBase.generated.h"

class UClickToMoveComponent;
class UAbilitySystemComponent;
class UNamedAttributeSet;

UENUM(BlueprintType)
enum class EMovementInputMode : uint8
{
	ClickToMove UMETA(DisplayName = "Click to Move"),
	Direct      UMETA(DisplayName = "Direct (WASD) -- not yet implemented")
};

UCLASS()
class THENAMEDCORE_API ANamedHeroBase : public ACharacter, public IAbilitySystemInterface
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

	// -- IAbilitySystemInterface --
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "Abilities")
	UNamedAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	// The active click-to-move component. In the future, a Direct
	// (WASD) component will live alongside this one and
	// MovementInputMode decides which is authoritative -- neither the
	// PlayerController nor any champion Blueprint needs to change when
	// that happens.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UClickToMoveComponent> ClickToMoveComponent;

	// GAS core components. Owned here (not on PlayerState) since heroes
	// in The Named don't need ability state to persist across
	// possession changes -- if that assumption changes later (e.g.
	// spectator swap, respawn-as-new-pawn design), this is the class to
	// revisit.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UNamedAttributeSet> AttributeSet;

private:
	// Shared setup path for both server (PossessedBy) and client
	// (OnRep_PlayerState) ability actor info initialization -- GAS
	// requires this be called on both ends, and it's easy to
	// accidentally only handle one, which produces the classic "abilities
	// work for the server player but not remote clients" bug.
	void InitializeAbilitySystem();
};

