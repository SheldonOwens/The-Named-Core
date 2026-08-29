// NamedGameplayAbility.cpp

#include "NamedGameplayAbility.h"

UNamedGameplayAbility::UNamedGameplayAbility()
{
	// InstancedPerActor: each hero gets their own instance of an ability
	// (so per-cast state, e.g. a channel timer, doesn't leak between
	// heroes). This is the standard choice for kit abilities; NonInstanced
	// is faster but can't hold per-activation state, which most of our
	// kits (Kessen marks, channels, stacking effects) will need.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// LocalPredicted: the ability plays immediately on the casting
	// client for responsiveness, then the server confirms or corrects.
	// This is what makes ability activation feel instant in a
	// competitive game instead of waiting on a server round-trip.
	// Server remains authoritative -- if the client's prediction is
	// wrong, GAS rolls it back.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UNamedGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OnAbilityActivated();
}
