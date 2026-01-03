// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Abilities/GameplayAbility.h"

class ELEMENTALARSENALEDITOR_API SElementalAbilityWizard : public SCompoundWidget
{
public:
	
	SElementalAbilityWizard();
	
	SLATE_BEGIN_ARGS(SElementalAbilityWizard) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	/** UI Callbacks */
	FReply OnCreateClicked();
	void OnPathSelected(const FString& NewPath);
	void OnNameChanged(const FText& NewText);
	bool IsCreateEnabled() const;

	/** Class Picker Callbacks */
	const UClass* GetParentClass() const { return ParentClass; }
	void OnParentClassChanged(const UClass* NewClass) { ParentClass = const_cast<UClass*>(NewClass); }

	const UClass* GetCostClass() const { return CostClass; }
	void OnCostClassChanged(const UClass* NewClass) { CostClass = const_cast<UClass*>(NewClass); }

	const UClass* GetCooldownClass() const { return CooldownClass; }
	void OnCooldownClassChanged(const UClass* NewClass) { CooldownClass = const_cast<UClass*>(NewClass); }

	const UClass* GetDamageClass() const { return DamageClass; }
	void OnDamageClassChanged(const UClass* NewClass) { DamageClass = const_cast<UClass*>(NewClass); }

	/** Tag Picker Callbacks */
	void OnAbilityTagsChanged(const FGameplayTagContainer& NewTags) const;

	/** Data */
	FString TargetPath;
	FString AbilityName;
	
	UClass* ParentClass;
	UClass* CostClass;
	UClass* CooldownClass;
	UClass* DamageClass;
	
	TSharedPtr<FGameplayTagContainer> AbilityTags;

	bool bCreateCost = true;
	bool bCreateCooldown = true;
	bool bCreateDamage = true;

	/** Helper to create a single asset */
	UObject* CreateAsset(const FString& AssetName, const FString& FolderPath, UClass* Class, UFactory* Factory);

	/** Helper to set up the properties of the created ability (Linking GEs) */
	void ConfigureAbility(const UBlueprint* AbilityBlueprint, const UBlueprint* CostBP, const UBlueprint* CooldownBP, UBlueprint* DamageBP) const;
};
