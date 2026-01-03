// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "Engine/DeveloperSettings.h"
#include "Gas/EAReactionTypes.h"

#include "EAReactionSettings.generated.h"

/**
 * Global settings for the Elemental Arsenal plugin.
 * Configurable in Project Settings -> Game -> Elemental Arsenal.
 */
UCLASS(Config = Game, DefaultConfig, meta = (DisplayName = "Elemental Arsenal"))
class ELEMENTALARSENAL_API UEAReactionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// List of all active elemental reactions in the game
	UPROPERTY(Config, EditAnywhere, Category = "Reactions", meta = (TitleProperty = "ReactionEffect"))
	TArray<FElementalReactionRule> ReactionRules;
};
