// Copyright NeoNexus Studios. All Rights Reserved.

#include "ElementalArsenalEditorModule.h"
#include "Modules/ModuleManager.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "UI/SElementalAbilityWizard.h"

#define LOCTEXT_NAMESPACE "FElementalArsenalEditorModule"

static const FName ElementalArsenalTabName("ElementalArsenalAbilityWizard");

void FElementalArsenalEditorModule::StartupModule()
{
	// Register the tab spawner
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ElementalArsenalTabName, FOnSpawnTab::CreateRaw(this, &FElementalArsenalEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FElementalArsenalTabTitle", "Ability Wizard"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FElementalArsenalEditorModule::RegisterMenus));
}

void FElementalArsenalEditorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ElementalArsenalTabName);
}

TSharedRef<SDockTab> FElementalArsenalEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SElementalAbilityWizard)
		];
}

void FElementalArsenalEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("ElementalArsenal");
			Section.Label = LOCTEXT("ElementalArsenalLabel", "Elemental Arsenal");
			
			Section.AddMenuEntry(
				"ElementalAbilityWizard",
				LOCTEXT("ElementalAbilityWizardTabTitle", "Ability Wizard"),
				LOCTEXT("ElementalAbilityWizardTooltip", "Opens the Elemental Ability Wizard to generate GAS assets."),
				FSlateIcon(),
				FUIAction(
					FExecuteAction::CreateLambda([]() {
						FGlobalTabmanager::Get()->TryInvokeTab(ElementalArsenalTabName);
					})
				)
			);
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FElementalArsenalEditorModule, ElementalArsenalEditor)
