// Copyright NeoNexus Studios. All Rights Reserved.

#include "ElementalArsenal.h"
#include "GameplayTagsManager.h"

DEFINE_LOG_CATEGORY(LogElementalArsenal);

#define LOCTEXT_NAMESPACE "FElementalArsenalModule"

void FElementalArsenalModule::StartupModule()
{
	// Register the Native Gameplay Tag for Recursion Guarding
	UGameplayTagsManager::Get().AddNativeGameplayTag(
		FName("Effect.Reaction"), 
		FString("Tag added to reaction effects to prevent infinite recursion loops.")
	);
}

void FElementalArsenalModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FElementalArsenalModule, ElementalArsenal)