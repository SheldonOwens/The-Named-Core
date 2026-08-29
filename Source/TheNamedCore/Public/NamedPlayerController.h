// NamedPlayerController.h
// The Named -- input layer
//
// Translates a raw mouse click into a world-space destination and hands
// it to the possessed hero via ANamedHeroBase::RequestMoveToLocation.
// This is deliberately the ONLY place that knows about "click," the
// hero and ClickToMoveComponent only ever see a destination vector --
// keeps the door open for a future spectator/replay controller or a
// WASD-driven controller without touching hero-side code.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NamedPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class THENAMEDCORE_API ANamedPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANamedPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Enhanced Input assets -- assign these in the editor (Blueprint
	// child of this controller, or directly on this class's defaults).
	// Can't be created from C++; an Input Mapping Context and Input
	// Action are data assets you set up in the Content Browser.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ClickMoveAction;

	// How far to trace when resolving a click into a world location.
	// 100000 (1000m) comfortably covers any reasonable MOBA camera
	// distance; only tune this down if trace performance ever becomes
	// a measurable issue, which is unlikely at this scale.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (ClampMin = "1000.0"))
	float ClickTraceDistance = 100000.0f;

	// Handles the click action being triggered. Traces from the cursor
	// into the world and, on a hit, forwards the location to the
	// possessed hero.
	void HandleClickMove(const FInputActionValue& Value);
};
