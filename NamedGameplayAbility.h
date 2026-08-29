// NamedGameplayAbility.h
// The Named -- GAS foundation
//
// Base class for every champion ability (Q/W/E/R/passive). Kept minimal
// on purpose: sets sane defaults for instancing and network execution,
// and provides a BlueprintImplementableEvent hook so VFX/animation
// tie-in stays in Blueprint per the hybrid workflow, while activation
// logic, cost, and cooldown stay in C++.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NamedGameplayAbility.generated.h"

UCLASS()
class THENAMEDCORE_API UNamedGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UNamedGameplayAbility();

protected:
	// Called after the ability successfully activates (cost paid,
	// cooldown applied, server-authoritative checks passed). Hook
	// cosmetic tie-in (VFX, animation montage, sound) here in each
	// champion's ability Blueprint -- this is the
	// BlueprintNativeEvent/BlueprintImplementableEvent bridge point the
	// discipline doc calls for.
	UFUNCTION(BlueprintImplementableEvent, Category = "Ability")
	void OnAbilityActivated();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
