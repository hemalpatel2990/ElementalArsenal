// Copyright NeoNexus Studios. All Rights Reserved.

#include "UI/SElementalAbilityWizard.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorStyleSet.h"
#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Factories/BlueprintFactory.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "Kismet2/KismetEditorUtilities.h"

#define LOCTEXT_NAMESPACE "SElementalAbilityWizard"

void SElementalAbilityWizard::Construct(const FArguments& InArgs)
{
	ParentClass = UGameplayAbility::StaticClass();
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
			.Padding(0, 0, 0, 10)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("WizardTitle", "Elemental Ability Wizard"))
				.Font(FCoreStyle::GetDefaultFontStyle("Bold", 20))
			]

			// Name Input
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 5)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.VAlign(VAlign_Center)
				.Padding(0, 0, 10, 0)
				[
					SNew(STextBlock).Text(LOCTEXT("NameLabel", "Ability Name:"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				[
					SNew(SEditableTextBox)
					.HintText(LOCTEXT("NameHint", "e.g., FireBall"))
					.OnTextChanged(this, &SElementalAbilityWizard::OnNameChanged)
				]
			]

			// Path Selection
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 10)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(STextBlock).Text(LOCTEXT("PathLabel", "Destination Folder:"))
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(0, 5)
				[
					ContentBrowserModule.Get().CreatePathPicker(PathPickerConfig)
				]
			]

			// Options
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 10)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) { bCreateCooldown = (NewState == ECheckBoxState::Checked); })
					[
						SNew(STextBlock).Text(LOCTEXT("CooldownOption", "Create Cooldown GE"))
					]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) { bCreateCost = (NewState == ECheckBoxState::Checked); })
					[
						SNew(STextBlock).Text(LOCTEXT("CostOption", "Create Cost GE"))
					]
				]
				+ SVerticalBox::Slot().AutoHeight().Padding(0, 2)
				[
					SNew(SCheckBox)
					.IsChecked(ECheckBoxState::Checked)
					.OnCheckStateChanged_Lambda([this](ECheckBoxState NewState) { bCreateDamage = (NewState == ECheckBoxState::Checked); })
					[
						SNew(STextBlock).Text(LOCTEXT("DamageOption", "Create Damage GE"))
					]
				]
			]

			// Create Button
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(0, 20)
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
	return !AbilityName.IsEmpty() && !TargetPath.IsEmpty();
}

FReply SElementalAbilityWizard::OnCreateClicked()
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	
	// Factories
	UBlueprintFactory* BPFactory = NewObject<UBlueprintFactory>();
	BPFactory->BlueprintType = BPTYPE_Normal;

	// 1. Create Gameplay Effects First (so we can link them)
	UBlueprint* CooldownBP = nullptr;
	if (bCreateCooldown)
	{
		BPFactory->ParentClass = UGameplayEffect::StaticClass();
		CooldownBP = Cast<UBlueprint>(CreateAsset("GE_" + AbilityName + "_Cooldown", TargetPath, UBlueprint::StaticClass(), BPFactory));
	}

	UBlueprint* CostBP = nullptr;
	if (bCreateCost)
	{
		BPFactory->ParentClass = UGameplayEffect::StaticClass();
		CostBP = Cast<UBlueprint>(CreateAsset("GE_" + AbilityName + "_Cost", TargetPath, UBlueprint::StaticClass(), BPFactory));
	}
	
	if (bCreateDamage)
	{
		BPFactory->ParentClass = UGameplayEffect::StaticClass();
		CreateAsset("GE_" + AbilityName + "_Damage", TargetPath, UBlueprint::StaticClass(), BPFactory);
	}

	// 2. Create Ability
	BPFactory->ParentClass = ParentClass;
	UBlueprint* AbilityBP = Cast<UBlueprint>(CreateAsset("GA_" + AbilityName, TargetPath, UBlueprint::StaticClass(), BPFactory));

	// 3. Link them
	if (AbilityBP)
	{
		ConfigureAbility(AbilityBP, CostBP, CooldownBP);
	}

	return FReply::Handled();
}

UObject* SElementalAbilityWizard::CreateAsset(const FString& AssetName, const FString& FolderPath, UClass* Class, UFactory* Factory)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	return AssetToolsModule.Get().CreateAsset(AssetName, FolderPath, Class, Factory);
}

void SElementalAbilityWizard::ConfigureAbility(UBlueprint* AbilityBlueprint, UBlueprint* CostBP, UBlueprint* CooldownBP)
{
	// Note: In a real implementation, we would modify the CDO of the generated blueprint here.
	// This often requires compiling the blueprint first or accessing the GeneratedClass.
	
	if (AbilityBlueprint->GeneratedClass)
	{
		UGameplayAbility* CDO = Cast<UGameplayAbility>(AbilityBlueprint->GeneratedClass->GetDefaultObject());
		if (CDO)
		{
			// Setting these properties usually requires the blueprint to be compiled at least once or dirtying the asset.
			// For this MVP, we will just log that we would do it.
			// To actually do it, we'd need to modify the property on the Blueprint's formatted class or compile it.
			
			// FKismetEditorUtilities::CompileBlueprint(AbilityBlueprint);
		}
	}
}

#undef LOCTEXT_NAMESPACE
