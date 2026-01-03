// Copyright NeoNexus Studios. All Rights Reserved.

#include "Gas/EAAttributeSet.h"
#include "ElementalArsenal.h"
#include "GameplayEffectExtension.h"
#include "GameplayEffect.h"
#include "GameplayTagsManager.h"
#include "EAReactionSettings.h"

UEAAttributeSet::UEAAttributeSet()
	: bIsProcessingReaction(false)
{
}

void UEAAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);

		if (LocalDamage > 0.f)
		{
			// Apply actual health damage
			const float OldHealth = GetHealth();
			SetHealth(FMath::Clamp(OldHealth - LocalDamage, 0.0f, GetMaxHealth()));

			// Try Elemental Reaction
			ProcessElementalReaction(Data);
		}
	}
}

void UEAAttributeSet::ProcessElementalReaction(const FGameplayEffectModCallbackData& Data)
{
	// 0. Recursion Guard: Prevent re-entry on the same target
	if (bIsProcessingReaction)
	{
		return;
	}

	// Use TGuardValue to ensure the flag is reset when we leave this function scope
	TGuardValue<bool> ScopedRecursionGuard(bIsProcessingReaction, true);

	// 1. Get Context
	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* SourceASC = Context.GetOriginalInstigatorAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = &Data.Target;

	if (!SourceASC || !TargetASC) return;

	// 2. Get Tags
	FGameplayTagContainer SourceTags;
	SourceASC->GetOwnedGameplayTags(SourceTags); 

	FGameplayTagContainer TargetTags;
	TargetASC->GetOwnedGameplayTags(TargetTags);

	// 3. Iterate over Rules from Project Settings
	const UEAReactionSettings* Settings = GetDefault<UEAReactionSettings>();
	if (!Settings) return;

	for (const FElementalReactionRule& Rule : Settings->ReactionRules)
	{
		// Recursion Guard: If the effect that just applied damage IS this rule's reaction effect, skip.
		if (Data.EffectSpec.Def && Data.EffectSpec.Def->GetClass() == Rule.ReactionEffect)
		{
			continue;
		}

		// Check if we match the Source and Target tags
		if (Rule.SourceTag.IsValid() && Rule.TargetTag.IsValid() && Rule.ReactionEffect)
		{
			if (SourceTags.HasTag(Rule.SourceTag) && TargetTags.HasTag(Rule.TargetTag))
			{
				// MATCH FOUND!
				UE_LOG(LogElementalArsenal, Log, TEXT("Elemental Reaction Triggered: %s on %s -> Applying %s"), 
					*Rule.SourceTag.ToString(), 
					*Rule.TargetTag.ToString(), 
					*Rule.ReactionEffect->GetName());

				// Apply the configured Gameplay Effect
				FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
				EffectContext.AddSourceObject(SourceASC->GetAvatarActor());

				FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(Rule.ReactionEffect, 1.0f, EffectContext);
				if (SpecHandle.IsValid())
				{
					// Mark this as a reaction to prevent infinite loops
					SpecHandle.Data->AddDynamicAssetTag(FGameplayTag::RequestGameplayTag(FName("Effect.Reaction")));
					
					SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				}
			}
		}
	}
}
