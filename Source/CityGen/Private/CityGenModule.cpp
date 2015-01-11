#include "CityGenPch.h"
#include "CityGenModule.h"
#include "CityGenWindow.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"

DEFINE_LOG_CATEGORY(CityGen);

IMPLEMENT_MODULE(CityGenModule, CityGen);

#define LOCTEXT_NAMESPACE "CityGen"

const FName CityGenWindowTabName(TEXT("CityGenTabName"));

CityGenModule::CityGenModule()
{

}


void CityGenModule::StartupModule()
{
	//Starts the plugin
	UE_LOG(CityGen, Warning, TEXT("Starting CityGen Module"));

	//Register MultiEdit window to the Unreal Editor window menu
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(CityGenWindowTabName, FOnSpawnTab::CreateRaw(this, &CityGenModule::CreateTab))
		.SetDisplayName(FText::FromString(TEXT("CityGen Window")))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetLevelEditorCategory())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Log.TabIcon"));

}

void CityGenModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CityGenWindowTabName);
}

TSharedRef<SDockTab> CityGenModule::CreateTab(const FSpawnTabArgs& Args)
{
	return
		SNew(SDockTab)
		.Label(LOCTEXT("TabTitle", "CityGen Window"))
		.TabRole(ETabRole::MajorTab)
		.ContentPadding(5)
		[
			SNew(SBorder)
			.Padding(4)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SNew(CityGenWindow)
			]
		];
}

