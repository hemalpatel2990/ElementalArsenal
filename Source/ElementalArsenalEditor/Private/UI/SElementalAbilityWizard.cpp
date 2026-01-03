// Copyright NeoNexus Studios. All Rights Reserved.

#include "UI/SElementalAbilityWizard.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Factories/BlueprintFactory.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "PropertyCustomizationHelpers.h"
#include "GameplayTagsEditorModule.h"

#define LOCTEXT_NAMESPACE "SElementalAbilityWizard"

SElementalAbilityWizard::SElementalAbilityWizard()
{
	ParentClass = nullptr;
	CostClass = nullptr;
	CooldownClass = nullptr;
	DamageClass = nullptr;
}

void SElementalAbilityWizard::Construct(const FArguments& InArgs)
{
	ParentClass = UGameplayAbility::StaticClass();
	CostClass = UGameplayEffect::StaticClass();
	CooldownClass = UGameplayEffect::StaticClass();
	DamageClass = UGameplayEffect::StaticClass();
	
	AbilityTags = MakeShared<FGameplayTagContainer>();
	TargetPath = "/Game"; // Default path

	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	FPathPickerConfig PathPickerConfig;
	PathPickerConfig.DefaultPath = TargetPath;
	PathPickerConfig.OnPathSelected = FOnPathSelected::CreateSP(this, &SElementalAbilityWizard::OnPathSelected);

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		.Padding(10)
		[
			SNew(SVerticalBox)

			// Title
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 0, 0, 15)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("WizardTitle", "Elemental Ability Wizard"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
			]

			// --- Core Settings ---
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
			[
				SNew(STextBlock).Text(LOCTEXT("CoreSettings", "Core Settings")).Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]

			// Name Input
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 10, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("NameLabel", "Ability Name:"))
				]
				+ SHorizontalBox::Slot().FillWidth(1.0f)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("NameHint", "e.g., FireBall"))
					.OnTextChanged(this, &SElementalAbilityWizard::OnNameChanged)
				]
			]

			// Parent Ability Class
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(0, 0, 10, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("ParentClassLabel", "Parent Ability:"))
				]
				+ SHorizontalBox::Slot().FillWidth(1.0f)
				[
					SNew(SClassPropertyEntryBox)
					.MetaClass(UGameplayAbility::StaticClass())
					.SelectedClass(this, &SElementalAbilityWizard::GetParentClass)
					.OnSetClass(this, &SElementalAbilityWizard::OnParentClassChanged)
				]
			]

			// Ability Tags
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(LOCTEXT("TagsLabel", "Ability Tags:"))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					IGameplayTagsEditorModule::Get().MakeGameplayTagContainerWidget(
						FOnSetGameplayTagContainer::CreateSP(this, &SElementalAbilityWizard::OnAbilityTagsChanged),
						AbilityTags,
						FString()
					)
				]
			]

			// Path Selection
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 10)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight()
				[
					SNew(STextBlock).Text(LOCTEXT("PathLabel", "Destination Folder:"))
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
				[
					ContentBrowserModule.Get().CreatePathPicker(PathPickerConfig)
				]
			]

			// --- Components Configuration ---
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 15, 0, 5)
			[
				SNew(STextBlock).Text(LOCTEXT("CompSettings", "Components Configuration")).Font(FCoreStyle::GetDefaultFontStyle("Bold", 12))
			]

			// Cooldown Config
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
			[
				SNew(SExpandableArea)
				.InitiallyCollapsed(false)
				.HeaderContent()
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) { bCreateCooldown = (NewState == ECheckBoxState::Checked); })
					[
						SNew(STextBlock).Text(LOCTEXT("CreateCooldown", "Create Cooldown GE"))
					]
				]
				.BodyContent()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10, 0, 10, 0)
					[
						SNew(STextBlock).Text(LOCTEXT("CDClassLabel", "Parent Class:"))
					]
					+ SHorizontalBox::Slot().FillWidth(1.0f)
					[
						SNew(SClassPropertyEntryBox)
						.MetaClass(UGameplayEffect::StaticClass())
						.SelectedClass(this, &SElementalAbilityWizard::GetCooldownClass)
						.OnSetClass(this, &SElementalAbilityWizard::OnCooldownClassChanged)
					]
				]
			]

			// Cost Config
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
			[
				SNew(SExpandableArea)
				.InitiallyCollapsed(false)
				.HeaderContent()
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) { bCreateCost = (NewState == ECheckBoxState::Checked); })
					[
						SNew(STextBlock).Text(LOCTEXT("CreateCost", "Create Cost GE"))
					]
				]
				.BodyContent()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10, 0, 10, 0)
					[
						SNew(STextBlock).Text(LOCTEXT("CostClassLabel", "Parent Class:"))
					]
					+ SHorizontalBox::Slot().FillWidth(1.0f)
					[
						SNew(SClassPropertyEntryBox)
						.MetaClass(UGameplayEffect::StaticClass())
						.SelectedClass(this, &SElementalAbilityWizard::GetCostClass)
						.OnSetClass(this, &SElementalAbilityWizard::OnCostClassChanged)
					]
				]
			]

			// Damage Config
			+ SVerticalBox::Slot().AutoHeight().Padding(0, 5)
			[
				SNew(SExpandableArea)
				.InitiallyCollapsed(false)
				.HeaderContent()
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) { bCreateDamage = (NewState == ECheckBoxState::Checked); })
					[
						SNew(STextBlock).Text(LOCTEXT("CreateDamage", "Create Damage GE"))
					]
				]
				.BodyContent()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot().AutoWidth().VAlign(VAlign_Center).Padding(10, 0, 10, 0)
					[
						SNew(STextBlock).Text(LOCTEXT("DmgClassLabel", "Parent Class:"))
					]
					+ SHorizontalBox::Slot().FillWidth(1.0f)
					[
						SNew(SClassPropertyEntryBox)
						.MetaClass(UGameplayEffect::StaticClass())
						.SelectedClass(this, &SElementalAbilityWizard::GetDamageClass)
						.OnSetClass(this, &SElementalAbilityWizard::OnDamageClassChanged)
					]
				]
			]

			// Create Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 30)
			.HAlign(HAlign_Right)
			[
				SNew(SButton)
				.Text(LOCTEXT("CreateBtn", "Generate Assets"))
				.IsEnabled(this, &SElementalAbilityWizard::IsCreateEnabled)
				.OnClicked(this, &SElementalAbilityWizard::OnCreateClicked)
			]
		]
	];
}

void SElementalAbilityWizard::OnAbilityTagsChanged(const FGameplayTagContainer& NewTags) const
{
	if (AbilityTags.IsValid())
	{
		*AbilityTags = NewTags;
	}
}

void SElementalAbilityWizard::OnPathSelected(const FString& NewPath)
{
	TargetPath = NewPath;
}

void SElementalAbilityWizard::OnNameChanged(const FText& NewText)
{
	AbilityName = NewText.ToString();
}

bool SElementalAbilityWizard::IsCreateEnabled() const
{
	return !AbilityName.IsEmpty() && !TargetPath.IsEmpty() && ParentClass != nullptr;
}

FReply SElementalAbilityWizard::OnCreateClicked()
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	
	// Factories
	UBlueprintFactory* BPFactory = NewObject<UBlueprintFactory>();
	BPFactory->BlueprintType = BPTYPE_Normal;

	// 1. Create Gameplay Effects
	UBlueprint* CooldownBP = nullptr;
	if (bCreateCooldown)
	{
		BPFactory->ParentClass = CooldownClass ? CooldownClass : UGameplayEffect::StaticClass();
		CooldownBP = Cast<UBlueprint>(CreateAsset("GE_" + AbilityName + "_Cooldown", TargetPath, UBlueprint::StaticClass(), BPFactory));
	}

	UBlueprint* CostBP = nullptr;
	if (bCreateCost)
	{
		BPFactory->ParentClass = CostClass ? CostClass : UGameplayEffect::StaticClass();
		CostBP = Cast<UBlueprint>(CreateAsset("GE_" + AbilityName + "_Cost", TargetPath, UBlueprint::StaticClass(), BPFactory));
	}
	
	UBlueprint* DamageBP = nullptr;
	if (bCreateDamage)
	{
		BPFactory->ParentClass = DamageClass ? DamageClass : UGameplayEffect::StaticClass();
		DamageBP = Cast<UBlueprint>(CreateAsset("GE_" + AbilityName + "_Damage", TargetPath, UBlueprint::StaticClass(), BPFactory));
	}

	// 2. Create Ability
	BPFactory->ParentClass = ParentClass;

	// 3. Link them & Apply Tags
	if (UBlueprint* AbilityBP = Cast<UBlueprint>(CreateAsset("GA_" + AbilityName, TargetPath, UBlueprint::StaticClass(), BPFactory)))
	{
		ConfigureAbility(AbilityBP, CostBP, CooldownBP, DamageBP);
	}

	return FReply::Handled();
}

UObject* SElementalAbilityWizard::CreateAsset(const FString& AssetName, const FString& FolderPath, UClass* Class, UFactory* Factory)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	return AssetToolsModule.Get().CreateAsset(AssetName, FolderPath, Class, Factory);
}

void SElementalAbilityWizard::ConfigureAbility(const UBlueprint* AbilityBlueprint, const UBlueprint* CostBP, const UBlueprint* CooldownBP, UBlueprint* DamageBP) const
{
	if (!AbilityBlueprint || !AbilityBlueprint->GeneratedClass)
	{
		return;
	}

	// We need to access the CDO to set properties
	if (UGameplayAbility* CDO = Cast<UGameplayAbility>(AbilityBlueprint->GeneratedClass->GetDefaultObject()))
	{
		bool bModified = false;

		// 1. Set Ability Tags (FStructProperty)
		if (AbilityTags.IsValid() && !AbilityTags->IsEmpty())
		{
			FStructProperty* TagsProp = FindFProperty<FStructProperty>(CDO->GetClass(), "AbilityTags");
			if (TagsProp && TagsProp->Struct == TBaseStructure<FGameplayTagContainer>::Get())
			{
				if (FGameplayTagContainer* TagContainer = TagsProp->ContainerPtrToValuePtr<FGameplayTagContainer>(CDO))
				{
					*TagContainer = *AbilityTags;
					bModified = true;
				}
			}
		}

		// 2. Link Cost GE (FClassProperty)
		if (CostBP && CostBP->GeneratedClass)
		{
			if (FClassProperty* CostProp = FindFProperty<FClassProperty>(CDO->GetClass(), "CostGameplayEffectClass"))
			{
				CostProp->SetObjectPropertyValue_InContainer(CDO, CostBP->GeneratedClass);
				bModified = true;
			}
		}

		// 3. Link Cooldown GE (FClassProperty)
		if (CooldownBP && CooldownBP->GeneratedClass)
		{
			if (FClassProperty* CooldownProp = FindFProperty<FClassProperty>(CDO->GetClass(), "CooldownGameplayEffectClass"))
			{
				CooldownProp->SetObjectPropertyValue_InContainer(CDO, CooldownBP->GeneratedClass);
				bModified = true;
			}
		}

		if (bModified)
		{
			// Mark the package as dirty so the user is prompted to save/compile
			if (UPackage* Package = AbilityBlueprint->GetOutermost())
			{
				Package->SetDirtyFlag(true);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
