// Copyright NeoNexus Studios. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class ELEMENTALARSENALEDITOR_API FElementalArsenalEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);

private:
	void RegisterMenus();
};
