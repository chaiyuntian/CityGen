#include "CityGenPch.h"
#include "CityGenModule.h"
#include "CityGenWindow.h"

DEFINE_LOG_CATEGORY(CityGen);

IMPLEMENT_MODULE(CityGenModule, CityGen);

#define LOCTEXT_NAMESPACE "CityGen"

const FName CityGenWindowTabName(TEXT("MultiEditWindowTab"));

CityGenModule::CityGenModule()
{

}


void CityGenModule::StartupModule()
{
	//Starts the plugin
	UE_LOG(CityGen, Warning, TEXT("Starting CityGen Module"));

	TSharedPtr<FExtender> extender = MakeShareable(new FExtender);
	extender->AddMenuExtension(
		"WindowLocalTabSpawners",
		EExtensionHook::After,
		NULL,
		FMenuExtensionDelegate::CreateRaw(this, &CityGenModule::CreateWindowMenu));

	FLevelEditorModule& levelEditor = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	levelEditor.GetMenuExtensibilityManager()->AddExtender(extender);

	//Register MultiEdit window to the Unreal Editor window menu
	FGlobalTabmanager::Get()->RegisterTabSpawner(
		CityGenWindowTabName,
		FOnSpawnTab::CreateRaw(this, &CityGenModule::CreateTab))
		.SetDisplayName(FText::FromString(TEXT("CityGen Window")));

}

void CityGenModule::ShutdownModule()
{

}

void CityGenModule::CreateWindowMenu(FMenuBuilder& MenuBuilder)
{
	//Creates the window when pressed
	MenuBuilder.AddMenuEntry(
		LOCTEXT("OpenWindow", "CityGen Window"),
		LOCTEXT("OpenWindowToolTip", "Opens the CityGen Plugin window"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &CityGenModule::OpenPluginWindow)));
}

void CityGenModule::OpenPluginWindow()
{
	UE_LOG(CityGen, Warning, TEXT("Creating CityGen Window"));
	FGlobalTabmanager::Get()->InvokeTab(CityGenWindowTabName);
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