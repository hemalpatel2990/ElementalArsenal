// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Abilities/GameplayAbility.h"

class SElementalAbilityWizard : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SElementalAbilityWizard) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	/** UI Callbacks */
	FReply OnCreateClicked();
	void OnPathSelected(const FString& NewPath);
	void OnNameChanged(const FText& NewText);
	bool IsCreateEnabled() const;

	/** Data */
	FString TargetPath;
	FString AbilityName;
	UClass* ParentClass;
	
	bool bCreateCost = true;
	bool bCreateCooldown = true;
	bool bCreateDamage = true;

	/** Helper to create a single asset */
	UObject* CreateAsset(const FString& AssetName, const FString& FolderPath, UClass* Class, UFactory* Factory);

	/** Helper to set up the properties of the created ability (Linking GEs) */
	void ConfigureAbility(UBlueprint* AbilityBlueprint, UBlueprint* CostBP, UBlueprint* CooldownBP);
};
