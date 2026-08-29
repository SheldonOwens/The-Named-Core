// NamedAttributeSet.cpp

#include "NamedAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UNamedAttributeSet::UNamedAttributeSet()
{
	// Placeholder defaults for Phase 0 testing. Real base values will
	// come from per-champion data (Blueprint child class defaults or a
	// data table) once kits exist -- these exist so a bare test
	// character has *something* sensible to fight with.
	Health.SetBaseValue(500.0f);
	Health.SetCurrentValue(500.0f);
	MaxHealth.SetBaseValue(500.0f);
	MaxHealth.SetCurrentValue(500.0f);
	AttackDamage.SetBaseValue(50.0f);
	AttackDamage.SetCurrentValue(50.0f);
	Armor.SetBaseValue(0.0f);
	Armor.SetCurrentValue(0.0f);
	MagicResist.SetBaseValue(0.0f);
	MagicResist.SetCurrentValue(0.0f);
}

void UNamedAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNamedAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNamedAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNamedAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNamedAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UNamedAttributeSet, MagicResist, COND_None, REPNOTIFY_Always);
}

void UNamedAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Clamp Health to [0, MaxHealth] whenever something tries to change
	// it directly (as opposed to via IncomingDamage -- see
	// PostGameplayEffectExecute below for that path).
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, MaxHealth.GetCurrentValue());
	}
}

void UNamedAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// This is the actual damage pipeline entry point. A GameplayEffect
	// (fired by an ability, or later, an item/buff) writes a value into
	// IncomingDamage. We catch that here, apply it to Health, and reset
	// IncomingDamage to zero so it doesn't linger as a stale value.
	//
	// Deliberately NOT doing armor/magic-resist mitigation math here yet --
	// that belongs in a GameplayEffect Execution Calculation once we
	// have more than one damage type to differentiate (physical vs
	// magic vs pure). For Phase 0, IncomingDamage is treated as
	// already-final damage. This is a known, intentional simplification,
	// not an oversight -- flagging it so it's not mistaken for the real
	// mitigation formula later.
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float DamageDone = GetIncomingDamage();
		SetIncomingDamage(0.0f);

		if (DamageDone > 0.0f)
		{
			const float NewHealth = FMath::Clamp(GetHealth() - DamageDone, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
		}
	}
}

void UNamedAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNamedAttributeSet, Health, OldHealth);
}

void UNamedAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNamedAttributeSet, MaxHealth, OldMaxHealth);
}

void UNamedAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNamedAttributeSet, AttackDamage, OldAttackDamage);
}

void UNamedAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNamedAttributeSet, Armor, OldArmor);
}

void UNamedAttributeSet::OnRep_MagicResist(const FGameplayAttributeData& OldMagicResist)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNamedAttributeSet, MagicResist, OldMagicResist);
}
