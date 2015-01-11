#pragma once

#include "CityGenPch.h"

class CityGenModule : public IModuleInterface
{
public:
	CityGenModule();
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedRef<SDockTab> CreateTab(const FSpawnTabArgs& Args);
};
