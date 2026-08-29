// NamedAttributeSet.h
// The Named -- GAS foundation
//
// Base attribute set every hero uses. Deliberately lean for Phase 0/M1:
// just enough attributes to prove the damage pipeline works end to end
// with a placeholder ability. Expand only when a real system (an item,
// a kit) actually needs a new attribute -- don't scope ahead of need.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NamedAttributeSet.generated.h"

// Standard GAS boilerplate: generates Get/Set/Init accessors for an
// attribute. GAMEPLAYATTRIBUTE_* macros are defined by AttributeSet.h
// (engine-provided), this just chains them together per-property.
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class THENAMEDCORE_API UNamedAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UNamedAttributeSet();

	// --- Health ---
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNamedAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Health", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UNamedAttributeSet, MaxHealth)

	// --- Offense ---
	// Flat, pre-mitigation attack damage. Champion kits scale ability
	// damage off this; items add to it later.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Offense", ReplicatedUsing = OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UNamedAttributeSet, AttackDamage)

	// --- Defense ---
	// Reduces incoming physical damage. Mitigation formula lives in the
	// damage pipeline (GameplayEffect execution calculation), not here --
	// this attribute set only stores the numbers.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Defense", ReplicatedUsing = OnRep_Armor)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UNamedAttributeSet, Armor)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Defense", ReplicatedUsing = OnRep_MagicResist)
	FGameplayAttributeData MagicResist;
	ATTRIBUTE_ACCESSORS(UNamedAttributeSet, MagicResist)

	// --- Meta attribute ---
	// Incoming damage is written here by GameplayEffects, then consumed
	// in PostGameplayEffectExecute to actually subtract from Health.
	// Standard GAS pattern -- keeps damage application server-authoritative
	// and lets us apply mitigation/clamping in one place before Health
	// ever changes. This also satisfies the doc's multiplayer-safety
	// principle: damage always resolves through this server-side path,
	// never a client-local calculation.
	UPROPERTY(BlueprintReadOnly, Category = "Attributes|Meta")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UNamedAttributeSet, IncomingDamage)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

protected:
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	virtual void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);

	UFUNCTION()
	virtual void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	virtual void OnRep_MagicResist(const FGameplayAttributeData& OldMagicResist);
};
