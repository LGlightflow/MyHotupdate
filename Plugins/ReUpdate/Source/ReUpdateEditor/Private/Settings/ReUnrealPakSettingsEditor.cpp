#include "Settings/ReUnrealPakSettingsEditor.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/Paths.h"
#include "GenericPlatform/GenericPlatformFile.h"
#include "Object/ReUpdateObject.h"
#include "ReUpdateEditor.h"
#include "Settings/ObjectStorageServerSettingsEditor.h"
#include "Misc/FileHelper.h"
#include "Settings/ObjectStorageSettingsRuntime.h"

#define LOCTEXT_NAMESPACE "UReUnrealPakSettingsEditor"
#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif
UReUnrealPakSettingsEditor::UReUnrealPakSettingsEditor()
	:Super()
	,VersionControlObject(NULL)
{
	bInternalExecutionPak = false;
	VersionName = TEXT("1.0.0");
	VersionMaximumBase = 9;

	auto ResetPath = [](FString &InPath,const TCHAR *InNewPath)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		InPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / InNewPath);
		if (!PlatformFile.DirectoryExists(*InPath))
		{
			PlatformFile.CreateDirectory(*InPath);
		}
	};

	ResetPath(PakSavePaths.Path, TEXT("Pak"));
	ResetPath(PakLogPaths.Path, TEXT("PakLog"));
	PakPlatform = EHotUpdateSettingsPlatform::Windows;
	bCompress = true;
	bForceUTF8 = true;
	bAES = false;
	bOrder = false;
}

#if WITH_EDITOR
void UReUnrealPakSettingsEditor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("PakPlatform"))
	{
		FString PakPlatformString = GetPlatform();

		if (UReHTTPServerSettingsEditor* PakSettings = GetMutableDefault<UReHTTPServerSettingsEditor>())
		{
			PakSettings->VersionConfig.PakPlatform = PakPlatformString;
			PakSettings->SaveConfig();
		}

		if (UReOSSServerSettingsEditor* PakSettings = GetMutableDefault<UReOSSServerSettingsEditor>())
		{
			PakSettings->VersionConfig.PakPlatform = PakPlatformString;
			PakSettings->SaveConfig();
		}

		if (UReLocalSettingsEditor* PakSettings = GetMutableDefault<UReLocalSettingsEditor>())
		{
			PakSettings->VersionConfig.PakPlatform = PakPlatformString;
			PakSettings->SaveConfig();
		}

		if (UObjectStorageSettingsRuntime* PakSettings = GetMutableDefault<UObjectStorageSettingsRuntime>())
		{
			PakSettings->VersionConfig.PakPlatform = PakPlatformString;
			PakSettings->SaveConfig();
		}
	}
}
#endif

FString UReUnrealPakSettingsEditor::GetPlatform() const
{
	return UEnum::GetValueAsString(PakPlatform).RightChop(FString(TEXT("EHotUpdateSettingsPlatform::")).Len());
}

//从服务器或本地读取最新版本信息，生成一个丢弃列表
void UReUnrealPakSettingsEditor::BuildVersionToDiscard()
{
	if (!VersionControlObject)
	{
		VersionControlObject = UVersionControlObject::CreateObject(UVersionControlObject::StaticClass(),this);
	}

	auto InitVersionDiscardSettings = [&](const FVersion &InServerVersion)
	{	
		VersionDiscardSettings.Empty();
		for (auto& Tmp : InServerVersion.Content)
		{
			FString LinkName = FPaths::GetCleanFilename(Tmp.Link);
			VersionDiscardSettings.Add(LinkName, Tmp.bDiscard);
		}

		FMessageDialog::Open(EAppMsgType::YesNo,
			LOCTEXT("VersionDiscardSettings_GetCurrentVersion",
				"Get current server version success.Please check the versiondiscardsettings structure to decide which package to discard"));
	};

	if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
	{
		FString VersionPath = GetDefault<UReLocalSettingsEditor>()->GetVersionPath();
		if (VersionPath.IsEmpty())
		{
			return;
		}

		FString JsonVersion;
		FFileHelper::LoadFileToString(JsonVersion,*VersionPath);

		FVersion ServerVersion;
		ReVersionControl::Read(JsonVersion, ServerVersion);
		VersionControlObject->SetServerVersion(ServerVersion);

		InitVersionDiscardSettings(ServerVersion);
	}
	else
	{
		FVersion Version;
		if (VersionControlObject->GetCurrentVersion(Version))
		{
			VersionControlObject->SetServerVersion(Version);
			InitVersionDiscardSettings(Version);
		}		
	}
}

void UReUnrealPakSettingsEditor::UploadDiscardedVersion()
{
	if (VersionControlObject)
	{
		auto CheckUploadVersionInfoChange = [&](const FVersion& InVersion, FString &JsonString)->bool
		{
			bool bUpdate = false;
			if (InVersion.Content.Num() == VersionDiscardSettings.Num())
			{
				//检测版本pak是否一致
				for (auto& Tmp : InVersion.Content)
				{
					FString LinkName = FPaths::GetCleanFilename(Tmp.Link);
					if (!VersionDiscardSettings.Find(LinkName))
					{
						FMessageDialog::Open(EAppMsgType::YesNo,
							LOCTEXT("VersionDiscardSettings_VersionInconsistency",
								"Version inconsistency."));

						//版本不一致
						return false;
					}
				}

				//检测版本是否有更新需求
				for (auto& Tmp : InVersion.Content)
				{
					FString LinkName = FPaths::GetCleanFilename(Tmp.Link);
					if (bool* bValue = VersionDiscardSettings.Find(LinkName))
					{
						if (Tmp.bDiscard != *bValue)
						{
							bUpdate = true;
							break;
						}
					}
				}
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::YesNo,
					LOCTEXT("VersionDiscardSettings_NumberInconsistency",
						"The number of versions is inconsistent and needs to be updated again."));
				bUpdate = false;
			}

			if (bUpdate)
			{
				FVersion InNewVersion = InVersion;
				InNewVersion.Crc = FGuid::NewGuid().ToString();//标记更新新版本

				for (auto& Tmp : VersionDiscardSettings)
				{
					if (FRemoteDataDescribe* RemoteDataDescribe = InNewVersion.Content.FindByPredicate(
						[&](const FRemoteDataDescribe& InDataDescribe)->bool
						{
							return FPaths::GetCleanFilename(InDataDescribe.Link) == Tmp.Key;
						}))
					{
						RemoteDataDescribe->bDiscard = Tmp.Value;
					}
				}

				ReVersionControl::Save(InNewVersion, JsonString);
			}

			return bUpdate;
		};

		if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
		{
			FString VersionPath = GetDefault<UReLocalSettingsEditor>()->GetVersionPath();
			if (VersionPath.IsEmpty())
			{
				return;
			}

			//读取最新版本
			FString JsonVersion;
			FFileHelper::LoadFileToString(JsonVersion, *VersionPath);

			//获取服务器实体版本
			FVersion ServerVersion;
			ReVersionControl::Read(JsonVersion, ServerVersion);
			VersionControlObject->SetServerVersion(ServerVersion);

			//检测版本是否合法 准备更新丢弃版本
			FString JsonString;
			if (CheckUploadVersionInfoChange(ServerVersion, JsonString))
			{
				if (FFileHelper::SaveStringToFile(JsonString, *VersionPath))
				{
					FMessageDialog::Open(EAppMsgType::YesNo,
						LOCTEXT("VersionDiscardSettings_VersionPath",
							"Discard information Update succeeded."));
				}
				else
				{
					FMessageDialog::Open(EAppMsgType::YesNo,
						LOCTEXT("VersionDiscardSettings_Update",
							"Discard information update without local version detected."));
				}
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::YesNo,
					LOCTEXT("VersionDiscardSettings_Update",
						"Discard information update without local version detected."));
			}
		}
		else
		{
			FVersion InVersion;
			if (VersionControlObject->GetCurrentVersion(InVersion))
			{
				VersionControlObject->SetServerVersion(InVersion);
				if (InVersion.IsVaild())
				{
					FString JsonString;
					if (CheckUploadVersionInfoChange(InVersion, JsonString))
					{
						FReUpdateEditorModule& ReUpdateEditorModule = FModuleManager::Get().LoadModuleChecked<FReUpdateEditorModule>(TEXT("ReUpdateEditor"));

						//将版本推到服务器
						ReUpdateEditorModule.Push(
							GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType,
							JsonString);
					}
					else
					{
						FMessageDialog::Open(EAppMsgType::YesNo,
							LOCTEXT("VersionDiscardSettings_Update",
								"Discard information update without local version detected."));
					}
				}
			}
		}
	}
	else
	{
		FMessageDialog::Open(EAppMsgType::YesNo,
			LOCTEXT("VersionDiscardSettings_Object",
				"The hot update object was not created successfully."));
	}
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif

#undef LOCTEXT_NAMESPACE