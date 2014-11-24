#pragma once

#include "CityGenPch.h"

class CityGenModule : public IModuleInterface
{
public:
	CityGenModule();

	/** Start the plugin */
	virtual void StartupModule() override;
	/** Shut down the plugin */
	virtual void ShutdownModule() override;

private:
	/**
	* Builds a new menu entry for the tab with an action to open the plugin window
	* @param[inout] MenuBuilder	-	To create the menu entry
	*/
	void CreateWindowMenu(FMenuBuilder& MenuBuilder);

	/**
	* Opens the MultiEdit window and is a action of the menu entry created
	* in CreateWindowMenu
	*/
	void OpenPluginWindow();

	/**
	* Creates MultiEdit window tab in the window menu of the Unreal Editor
	* @param[in] Args	-	Arguments for the tab
	* @return Shared reference to the created docktab
	*/
	TSharedRef<SDockTab> CreateTab(const FSpawnTabArgs& Args);
};
