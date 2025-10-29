// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReUpdateEditor.h"
#include "ReUpdateEditorStyle.h"
#include "ReUpdateEditorCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"
#include "Pak/UnrealPakFile.h"
#include <ContentBrowserModule.h>
#include <ISettingsModule.h>
#include "Settings/ReUnrealPakSettingsEditor.h"
#include "Settings/ReHTTPServerSettingsEditor.h"
#include "Settings/ReOSSServerSettingsEditor.h"
#include "Settings/ReLocalSettingsEditor.h"
#include "Settings/ObjectStorageServerSettingsEditor.h"
#include "ObjectStorageServer/ObjectStorageServer.h"
#include "Widget/SReUpdateSlateMain.h"

static const FName ReUpdateEditorTabName("ReUpdateEditor");

#define LOCTEXT_NAMESPACE "FReUpdateEditorModule"

void FReUpdateEditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FReUpdateEditorStyle::Initialize();
	FReUpdateEditorStyle::ReloadTextures();

	FReUpdateEditorCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FReUpdateEditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FReUpdateEditorModule::PluginButtonClicked),
		FCanExecuteAction());

	UE_LOG(LogTemp, Warning, TEXT("RegisterMenus() called"));
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FReUpdateEditorModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ReUpdateEditorTabName, FOnSpawnTab::CreateRaw(this, &FReUpdateEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FReUpdateEditorTabTitle", "ReUpdateEditor"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);//Hidden


	/*************************
	//	Custom
	*************************/

	//文件夹
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserMenuExtender_SelectedPaths = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	ContentBrowserMenuExtender_SelectedPaths.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRaw(this, &FReUpdateEditorModule::OnExtendContentBrowser));// 扩展右键菜单

	//文件
	TArray<FContentBrowserMenuExtender_SelectedAssets>& ContentBrowserMenuExtender_SelectedAssets = ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	ContentBrowserMenuExtender_SelectedAssets.Add(FContentBrowserMenuExtender_SelectedAssets::CreateRaw(this, &FReUpdateEditorModule::OnExtendContentAssetBrowser));

	//注册设置
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings")))
	{
		//注册pak
		SettingsModule->RegisterSettings("Project", "ProjectPak", "PakSettings",
			LOCTEXT("SimpleUnrealPakSettings", "Pak Settings"),
			LOCTEXT("SimpleUnrealPakSettingsTip", "Package settings for the current platform you have set."),
			GetMutableDefault<UReUnrealPakSettingsEditor>());

		//对象存储总设置
		SettingsModule->RegisterSettings("Project", "ObjectStorage", "ObjectStorageTotalSettings",
			LOCTEXT("ObjectStorageSettings", "A Object Storage Total Settings"),
			LOCTEXT("ObjectStorageSettingsTip", "Total settings for the object storage server."),
			GetMutableDefault<UObjectStorageServerSettingsEditor>());

		//HTTP设置
		SettingsModule->RegisterSettings("Project", "ObjectStorage", "HTTPSettings",
			LOCTEXT("HTTPServerSettings", "HTTP Server Settings"),
			LOCTEXT("HTTPServerSettingsTip", "Support custom object storage server."),
			GetMutableDefault<UReHTTPServerSettingsEditor>());

		//OSS设置
		SettingsModule->RegisterSettings("Project", "ObjectStorage", "OSSSettings",
			LOCTEXT("OSSServerSettings", "OSS Server Settings"),
			LOCTEXT("OSSServerSettingsTip", "Support Alibaba cloud object storage server."),
			GetMutableDefault<UReOSSServerSettingsEditor>());

		//Local设置
		SettingsModule->RegisterSettings("Project", "ObjectStorage", "LocalVersionSettings",
			LOCTEXT("LocalVersionSettings", "Local Version Settings"),
			LOCTEXT("LocalVersionSettingsTip", "Support Alibaba Local object storage Version."),
			GetMutableDefault<UReLocalSettingsEditor>());

	}
}

void FReUpdateEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FReUpdateEditorStyle::Shutdown();

	FReUpdateEditorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ReUpdateEditorTabName);
}

TSharedRef<SDockTab> FReUpdateEditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FReUpdateEditorModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("ReUpdateEditor.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(SReUpdateSlateMain)
			]
		];
}

void FReUpdateEditorModule::PluginButtonClicked()
{
#if (ENGINE_MAJOR_VERSION == 5 || (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 2 && ENGINE_PATCH_VERSION >= 7))
	FGlobalTabmanager::Get()->TryInvokeTab(ReUpdateEditorTabName);
#else
	FGlobalTabmanager::Get()->InvokeTab(FTabId(ReUpdateEditorTabName));
#endif
}

// 资产右键
TSharedRef<FExtender> FReUpdateEditorModule::OnExtendContentAssetBrowser(const TArray<FAssetData>& AssetData)
{
	TSharedRef<FExtender> Extender(new FExtender);
	TArray<FString> NewPaths;
	for (const auto& Tmp : AssetData)
	{
		NewPaths.Add(Tmp.PackageName.ToString());
	}
	Extender->AddMenuExtension(
		"AssetContextReferences",
		EExtensionHook::Before,
		nullptr,
		FMenuExtensionDelegate::CreateRaw(this,
			&FReUpdateEditorModule::CreateSuMenuForContentBrowser, //具体子菜单内容
			NewPaths, EReSubMenuType::HU_ASSET));

	return Extender;
}

// 文件夹右键
TSharedRef<FExtender> FReUpdateEditorModule::OnExtendContentBrowser(const TArray<FString>& NewPaths)
{
	TSharedRef<FExtender> Extender(new FExtender);

	Extender->AddMenuExtension(
		"FolderContext",
		EExtensionHook::Before, 
		nullptr, FMenuExtensionDelegate::CreateRaw(this,
		&FReUpdateEditorModule::CreateSuMenuForContentBrowser,
			NewPaths, EReSubMenuType::HU_CONTENT));

	return Extender;
}

////具体子菜单可选项
void FReUpdateEditorModule::CreateSuMenuForContentBrowser(FMenuBuilder& MunuBuilder, TArray<FString> NewPaths, EReSubMenuType InType)
{
	FText SectionName;
	FText SectionNameTip;
	switch (InType)
	{
	case EReSubMenuType::HU_CONTENT:
	{
		SectionName = LOCTEXT("HotUpdateContentEditor", "Package resources");
		SectionNameTip = LOCTEXT("HotUpdateContentEditorDescribe", "Upload data to the server .");

		break;
	}
	case EReSubMenuType::HU_ASSET:
	{
		SectionName = LOCTEXT("HotUpdateContentAssetEditor", "Package Asset to Server");
		SectionNameTip = LOCTEXT("HotUpdateContentEditorAssetDescribe", "Upload data to the server .");

		break;
	}
	}

	MunuBuilder.AddSubMenu(
		SectionName,
		SectionNameTip,
		FNewMenuDelegate::CreateRaw(this,
			&FReUpdateEditorModule::SubExtendContentBrowser,// 可选项具体内容
			NewPaths,
			InType));
}

//可选项具体内容
void FReUpdateEditorModule::SubExtendContentBrowser(FMenuBuilder& MunuBuilder, TArray<FString> NewPaths, EReSubMenuType InType)
{
	MunuBuilder.BeginSection("HotUpdate", LOCTEXT("HotUpdateMenu", "Upload server mode"));

	FText SectionName;
	FText SectionNameTip;
	switch (InType)
	{
		case EReSubMenuType::HU_CONTENT:
		{
			if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
			{
				SectionName = LOCTEXT("HotUpdate_PushLocal", "Push To Local");
				SectionNameTip = LOCTEXT("HotUpdate_PushTip", "Push resources to the local.");
			}
			else
			{
				SectionName = LOCTEXT("HotUpdate_PushServer", "Push To Sever");
				SectionNameTip = LOCTEXT("HotUpdate_PushTip", "Push resources to the Local.");
			}

			break;
		}
		case EReSubMenuType::HU_ASSET:
		{
			if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
			{
				SectionName = LOCTEXT("HotUpdateAsset_Push", "Push Asset to local");
				SectionNameTip = LOCTEXT("HotUpdateAsset_PushTip", "Push resources to the local.");
			}
			else
			{
				SectionName = LOCTEXT("HotUpdateAsset_Push", "Push Asset");
				SectionNameTip = LOCTEXT("HotUpdateAsset_PushTip", "Push resources to the server.");
			}

			break;
		}
	}

	MunuBuilder.AddMenuEntry(
		SectionName,
		SectionNameTip,
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FReUpdateEditorModule::Push, NewPaths, InType)));

	MunuBuilder.AddMenuEntry(
		LOCTEXT("HotUpdate_Pak", "Pak To Local"),
		SectionNameTip = LOCTEXT("HotUpdate_PakTip", "Pack resources as Pak to local."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FReUpdateEditorModule::Push, NewPaths)));

	MunuBuilder.AddMenuEntry(
		LOCTEXT("HotUpdate_UnitTesting", "Unit Testing"),
		SectionNameTip = LOCTEXT("HotUpdate_UnitTestingTip", "This function is only used for unit testing."),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FReUpdateEditorModule::UnitTesting, NewPaths)));


	MunuBuilder.EndSection();
}

void FReUpdateEditorModule::UnitTesting(TArray<FString> NewPaths)
{
}

// 入口:打包资源到本地
void FReUpdateEditorModule::Push(TArray<FString> NewPaths)
{
	if (ReUnrealPakEditor::FPak().PakFile(NewPaths))
	{
		FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PakLocalSuccess",
			"The resource is packaged successfully and has been stored in the local."));

		FPlatformProcess::ExploreFolder(*GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path);

		//检查打包的pak项目
		for (auto& Tmp : NewPaths)
		{
			FString PakName = *GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path / FPaths::GetCleanFilename(Tmp) + TEXT(".pak");
			ReUnrealPakEditor::FPak().ListPak(PakName);

			//if (FPlatformFileOpenLog* PlatformFileOpenLog = (FPlatformFileOpenLog*)(FPlatformFileManager::Get().FindPlatformFile(FPlatformFileOpenLog::GetTypeName())))
			//{
			//	PlatformFileOpenLog->AddPackageToOpenLog(*PakName);
			//}
		}
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PakLocalFail",
			"Resource packaging failed. Check whether the resource has been cooked or whether the resource exists."));
	}
}

void FReUpdateEditorModule::Push(TArray<FString> NewPaths, EReSubMenuType InType)
{
	//先清除前任的pak痕迹
	ClearPak();

	//再继续pak资源
	bool bPak = false;
	switch (InType)
	{
	case EReSubMenuType::HU_CONTENT:
		bPak = ReUnrealPakEditor::FPak().PakFile(NewPaths);
		break;
	case EReSubMenuType::HU_ASSET:
		bPak = ReUnrealPakEditor::FPak().PakMapFile(NewPaths);
		break;
	}

	if (bPak)
	{
		if (const UObjectStorageSettings* InStorageSettings = GetStorageSettings())
		{
			FString RelativeInstallationLocation = InStorageSettings->VersionConfig.bInstallContentPak == false ?
				InStorageSettings->VersionConfig.RelativeInstallationLocation : TEXT("");

			//版本名字迭代
			VersionIteration();

			//推到对象存储服务器
			if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::HTTP_SERVER)
			{
				ReUnrealPakEditor::FObjectStorageClient::HTTPPut(RelativeInstallationLocation);
			}
			else if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::OSS_SERVER)
			{
				ReUnrealPakEditor::FObjectStorageClient::OSSPut(RelativeInstallationLocation);
			}
			else if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
			{
				ReUnrealPakEditor::FObjectStorageClient::LocalPut(RelativeInstallationLocation);
			}
		}

		if (GetDefault<UObjectStorageServerSettingsEditor>()->bClearPak)
		{
			//清除Pak
			ClearPak();
		}

		FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PutPakSuccess",
			"Resources uploaded successfully."));
	}
	else
	{
		check(0);
	}
}

void FReUpdateEditorModule::Push(EObjectStorageServerType ServerType, const UObjectStorageSettings* InStorageSettings)
{
	if (InStorageSettings)
	{
		//版本名字迭代
		VersionIteration();

		auto UpdateToServer = [&](const TArray<FCustomVersionInstallation>& InArray, bool bMainVersion = false)
			{
				switch (ServerType)
				{
				case EObjectStorageServerType::HTTP_SERVER:
				{
					for (auto& Tmp : InArray)
					{
						FString RelativeInstallationLocation = Tmp.bInstallContentPak == false ?
							Tmp.RelativeInstallationLocation : TEXT("");

						ReUnrealPakEditor::FObjectStorageClient::HTTPPut(
							RelativeInstallationLocation,
							bMainVersion,
							Tmp.FilePath.FilePath);
					}

					break;
				}
				case EObjectStorageServerType::OSS_SERVER:
				{
					for (auto& Tmp : InArray)
					{
						FString RelativeInstallationLocation = Tmp.bInstallContentPak == false ?
							Tmp.RelativeInstallationLocation : TEXT("");

						ReUnrealPakEditor::FObjectStorageClient::OSSPut(
							RelativeInstallationLocation,
							bMainVersion,
							Tmp.FilePath.FilePath);
					}

					break;
				}
				case EObjectStorageServerType::LOCAL_POS:
				{
					for (auto& Tmp : InArray)
					{
						FString RelativeInstallationLocation = Tmp.bInstallContentPak == false ?
							Tmp.RelativeInstallationLocation : TEXT("");

						ReUnrealPakEditor::FObjectStorageClient::LocalPut(
							RelativeInstallationLocation,
							bMainVersion,
							Tmp.FilePath.FilePath);
					}

					break;
				}
				}
			};

		UpdateToServer(InStorageSettings->VersionConfig.CustomExtraInstallationInfo);
		UpdateToServer(InStorageSettings->VersionConfig.MainVersionInstallationInfo, true);

		FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PutMainPakSuccess",
			"The main version or additional version was uploaded successfully."));
	}
}

void FReUpdateEditorModule::Push(EObjectStorageServerType ServerType, const FString& InNewVersion)
{
	switch (ServerType)
	{
	case EObjectStorageServerType::HTTP_SERVER:
	{
		ReUnrealPakEditor::FObjectStorageClient::HTTPPutVersion(InNewVersion);

		break;
	}
	case EObjectStorageServerType::OSS_SERVER:
	{
		ReUnrealPakEditor::FObjectStorageClient::OSSPutVersion(InNewVersion);

		break;
	}
	case EObjectStorageServerType::LOCAL_POS:
	{
		ReUnrealPakEditor::FObjectStorageClient::LocalPutVersion(InNewVersion);

		break;
	}
	}
}

void FReUpdateEditorModule::Unlock(EObjectStorageServerType ServerTyp)
{
	switch (ServerTyp)
	{
	case EObjectStorageServerType::HTTP_SERVER:
		ReUnrealPakEditor::FObjectStorageClient::HTTPUnlock();
		break;
	case EObjectStorageServerType::OSS_SERVER:
		ReUnrealPakEditor::FObjectStorageClient::OSSUnlock();
		break;
	case EObjectStorageServerType::LOCAL_POS:
		ReUnrealPakEditor::FObjectStorageClient::LocalUnlock();
		break;
	}
}


void FReUpdateEditorModule::VersionIteration()
{
	if (UReUnrealPakSettingsEditor* InSettingsEditor = const_cast<UReUnrealPakSettingsEditor*>(GetDefault<UReUnrealPakSettingsEditor>()))
	{
		FString VersionString = InSettingsEditor->VersionName.ToString();

		//I 将版本拆分
		TArray<FString> VersionArray;
		VersionString.ParseIntoArray(VersionArray, TEXT("."));

		//II 将拆分版本转为int32
		//xx.xx.xx
		TArray<int32> VersionArrayNumber;
		for (auto& Tmp : VersionArray)
		{
			VersionArrayNumber.Add(FCString::Atoi(*Tmp));
		}

		//III 版本的迭代运算
		VersionNumberRecursion(VersionArrayNumber, 1);

		//V 版本合成
		FString NewVersion;
		for (auto& Tmp : VersionArrayNumber)
		{
			NewVersion += TEXT(".") + FString::FromInt(Tmp);
		}
		NewVersion.RemoveFromStart(".");

		//VI 版本复制
		InSettingsEditor->VersionName = *NewVersion;

		//保存
		InSettingsEditor->SaveObjectStorageConfig();
	}
}


void FReUpdateEditorModule::VersionNumberRecursion(TArray<int32>& VersionNumber, int32 DepthValue)
{
	int32 Number = VersionNumber.Num();
	if (Number >= DepthValue)
	{
		int32& LastNumber = VersionNumber[Number - DepthValue];
		if (LastNumber >= GetDefault<UReUnrealPakSettingsEditor>()->VersionMaximumBase)
		{
			VersionNumberRecursion(VersionNumber, ++DepthValue);

			//置为零
			LastNumber = 0;
		}
		else
		{
			LastNumber++;
		}
	}
}

void FReUpdateEditorModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FReUpdateEditorCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FReUpdateEditorCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

void FReUpdateEditorModule::ClearPak()
{
	const FString PakPath = GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path;

	TArray<FString> PakLocalPaths;
	IFileManager::Get().FindFilesRecursive(PakLocalPaths, *PakPath, TEXT("*"), true, false);
	for (auto& Tmp : PakLocalPaths)
	{
		if (IFileManager::Get().Delete(*Tmp))
		{

		}
	}
}







#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FReUpdateEditorModule, ReUpdateEditor)

