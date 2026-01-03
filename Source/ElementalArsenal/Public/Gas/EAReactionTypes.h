// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "EAReactionTypes.generated.h"

/**
 * Defines a single elemental reaction rule.
 * Example: If Source has "Element.Fire" and Target has "Status.Oil", apply "GE_Explosion".
 */
USTRUCT(BlueprintType)
struct FElementalReactionRule
{
	GENERATED_BODY()

	// The tag the attacker must have (e.g., Element.Fire)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction")
	FGameplayTag SourceTag;

	// The tag the victim must have (e.g., Status.Oil)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction")
	FGameplayTag TargetTag;

	// The Gameplay Effect to apply when this combination is met (e.g., GE_Explosion)
	// This GE should ideally have a GameplayCue attached to it for the VFX!
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reaction")
	TSubclassOf<UGameplayEffect> ReactionEffect;
};
