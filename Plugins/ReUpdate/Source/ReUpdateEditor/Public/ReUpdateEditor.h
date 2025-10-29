// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "ReUpdateEditorType.h"

class FToolBarBuilder;
class FMenuBuilder;

enum class EReSubMenuType :uint8
{
	HU_CONTENT,
	HU_ASSET
};

class FReUpdateEditorModule : public IModuleInterface
{
	friend class SReUpdateSlateMain;
	friend class UReUnrealPakSettingsEditor;

public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	/** This function will be bound to Command (by default it will bring up plugin window) */
	void PluginButtonClicked();
	
private:
	//对资产鼠标右键进行激活
	TSharedRef<FExtender> OnExtendContentAssetBrowser(const TArray<FAssetData>& AssetData);
	TSharedRef<FExtender> OnExtendContentBrowser(const TArray<FString>& NewPaths);

	void CreateSuMenuForContentBrowser(FMenuBuilder& MunuBuilder, TArray<FString> NewPaths, EReSubMenuType InType);
	void SubExtendContentBrowser(FMenuBuilder& MunuBuilder, TArray<FString> NewPaths, EReSubMenuType InType);

	void UnitTesting(TArray<FString> NewPaths);

	void Push(TArray<FString> NewPaths);
	void Push(TArray<FString> NewPaths, EReSubMenuType InType);
	void Push(EObjectStorageServerType ServerType, const UObjectStorageSettings* InStorageSettings);
	void Push(EObjectStorageServerType ServerType, const FString& InNewVersion);//单独上传版本
	void Unlock(EObjectStorageServerType ServerTyp);

	void VersionIteration();
	void VersionNumberRecursion(TArray<int32>& VersionNumber, int32 DepthValue);



	void RegisterMenus();
	void ClearPak();


	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
};
