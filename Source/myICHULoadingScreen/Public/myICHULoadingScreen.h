//
// Created by estebagel on 11/28/18.
//


#pragma once

#include "ModuleInterface.h"
#include "Modules/ModuleManager.h"

/** Module interface for this game's loading screens */
class ImyICHULoadingScreenModule : public IModuleInterface
{
public:
	/** Loads the module so it can be turned on */
	static inline ImyICHULoadingScreenModule& Get()
	{
		return FModuleManager::LoadModuleChecked<ImyICHULoadingScreenModule>("myICHULoadingScreen");
	}

	/** Kicks off the loading screen for in game loading (not startup) */
	virtual void StartInGameLoadingScreen(bool bPlayUntilStopped, float PlayTime) = 0;

	/** Stops the loading screen */
	virtual void StopInGameLoadingScreen() = 0;
};


