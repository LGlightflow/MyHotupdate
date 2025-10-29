// Copyright (C) RenZhai.2021.All Rights Reserved.
#include "ObjectStorageServer/ObjectStorageServer.h"
#include "Interfaces/IPluginManager.h"
#include "Settings/ReHTTPServerSettingsEditor.h"
#include "Settings/ReOSSServerSettingsEditor.h"
#include "Settings/ReUnrealPakSettingsEditor.h"
#include "Settings/ReLocalSettingsEditor.h"
#include "ReHandleProgramReturnValue.h"
#include "Version/ReVersion.h"
#include "Misc/FileHelper.h"
#include "Protocol/ReUpdateProtocol.h"
#include "Async/Async.h"

#define LOCTEXT_NAMESPACE "ReUnrealPakEditor"

namespace ReUnrealPakEditor
{
	FString GetOSSExePath()
	{
		return FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin("ReUpdate")->GetBaseDir() / TEXT("Engine/Binaries/Win64/OSSObjectStorageClient.exe"));
	}

	FString GetHTTPExePath()
	{
		return FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin("ReUpdate")->GetBaseDir() / TEXT("Engine/Binaries/Win64/HotUpdateClient.exe"));
	}

	FString GetLoclExePath()
	{
		return FPaths::ConvertRelativePathToFull(IPluginManager::Get().FindPlugin("ReUpdate")->GetBaseDir() / TEXT("Engine/Binaries/Win64/LocalObjectStorageClient.exe"));
	}

	FString GetCheckValue(const FString &InString)
	{
		if (InString.IsEmpty())
		{
			return TEXT("NONE");
		}

		return InString;
	}

	void CheckLocalPutVersion(TFunction<void()> InFun)
	{
		if (GetDefault<UReLocalSettingsEditor>()->BucketName != NAME_None)
		{
			if (!GetDefault<UReLocalSettingsEditor>()->LocalVersionPath.Path.IsEmpty())
			{
				InFun();
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("LocalPut_LocalVersionPath",
					"You need to specify a location to store the version locally."));
			}
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("LocalPut_BucketName",
				"You need to specify a bucket name. The bucket name cannot be empty."));
		}
	}

	void CreateProc(const TCHAR* InEXEPath,const TCHAR* InParam)
	{
		void* ReadPipe = nullptr;
		void* WritePipe = nullptr;
		//FPlatformProcess::CreatePipe(ReadPipe, WritePipe);
		FProcHandle Handle = FPlatformProcess::CreateProc(
			InEXEPath,
			InParam,
			false, 
			false, 
			false, 
			nullptr, 
			0,
			NULL, 
			NULL, NULL/*WritePipe,
			ReadPipe*/);

		FPlatformProcess::WaitForProc(Handle);

		//AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
		//[]()
		//{

		//});
		//FString OutStd = FPlatformProcess::ReadPipe(ReadPipe);
		FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("ServerPut",
			"The upload has been completed. Please check the server."));

		//FMessageDialog::Open(EAppMsgType::YesNo, FText::Format(LOCTEXT("ServerPut",
		//	"The upload has been completed. Please check the server. {0}"),
		//	FText::FromString(OutStd)));

		int32 ReturnCode = (int32)EReUpdateProtocol::SP_OK_RESPONSE;
		FPlatformProcess::GetProcReturnCode(Handle, &ReturnCode);

		//捕获返回类型
		ReUnrealPakEditor::FObjectStorageClient::MsgPrint((EReUpdateProtocol)ReturnCode);
	}

	void FObjectStorageClient::MsgPrint(EReUpdateProtocol InProtocol)
	{
		switch (InProtocol)
		{
		case EReUpdateProtocol::SP_INVALID_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_INVALID_RESPONSE",
				"无效协议 有可能用户传递了 SP_NONE_REQUEST."));
			break;
		case EReUpdateProtocol::SP_OK_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_OK_RESPONSE",
				"执行成功."));
			break;
		case EReUpdateProtocol::SP_FAIL_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_FAIL_RESPONSE",
				"执行失败."));
			break;
		case EReUpdateProtocol::SP_SERVER_LOCKED_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_SERVER_LOCKED_RESPONSE",
				"服务器被锁住,可能是其管理员操作或者死锁."));
			break;
		case EReUpdateProtocol::SP_UPLOAD_VERSION_INFORMATION_FAIL_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_UPLOAD_VERSION_INFORMATION_FAIL_RESPONSE",
				"版本信息上传失败."));
			break;
		case EReUpdateProtocol::SP_CHECK_VERSION_PATH_IS_EMPTY_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_CHECK_VERSION_PATH_IS_EMPTY_RESPONSE",
				"检测到版本路径为空."));
			break;
		case EReUpdateProtocol::SP_CHECK_PAK_LOCAL_PATH_IS_EMPTY_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_CHECK_PAK_LOCAL_PATH_IS_EMPTY_RESPONSE",
				"检测PakLocalPaths路径为空."));
			break;
		case EReUpdateProtocol::SP_CLOSE_LOCK_FAIL_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_CLOSE_LOCK_FAIL_RESPONSE",
				"关闭锁失败."));
			break;
		case EReUpdateProtocol::SP_CHECK_VERSION_INFO_IS_EMPTY_RESPONSE:
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("MsgPrint_SP_CHECK_VERSION_INFO_IS_EMPTY_RESPONSE",
				"版本信息为空."));
			break;
		}
	}

	void FObjectStorageClient::HTTPPut(const FString& InPakInstallationPath, bool bMainVersion, const FString& InFilePath)
	{
		FString CreateParm = FString::Printf(
		TEXT("-log -Protocol=%i -VersionName=%s -Account=%s -Password=%s -Bucket=%s -PatchVersionLogName=%s -ServerVersionName=%s\
 -VersionLock=%s -platform=%s -PakLocalPaths=%s -Endpoint=%s -InstallationPath=%s -bMainVersion=%s -FilePath=%s -bHttps=%s"),
			(int32)EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST,//对应协议
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->VersionName.ToString()),
			TEXT(""),
			TEXT(""),
			*GetCheckValue(GetDefault<UReHTTPServerSettingsEditor>()->ObjectStorageName.ToString()),
			*GetCheckValue(GetDefault<UReHTTPServerSettingsEditor>()->VersionConfig.PatchVersionLogName.ToString()),
			*GetCheckValue(GetDefault<UReHTTPServerSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
			*GetCheckValue(GetDefault<UReHTTPServerSettingsEditor>()->VersionConfig.VersionLock.ToString()),
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
			*GetCheckValue(InFilePath.IsEmpty() ? GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path :TEXT("")),
			*GetCheckValue(GetDefault<UReHTTPServerSettingsEditor>()->LANIP),
			*GetCheckValue(InPakInstallationPath),
			*GetCheckValue(bMainVersion ? TEXT("true") : TEXT("false")),
			*GetCheckValue(InFilePath.IsEmpty() ? InFilePath : FPaths::ConvertRelativePathToFull(InFilePath)),
			GetDefault<UReHTTPServerSettingsEditor>()->VersionConfig.bHttps ? TEXT("true") : TEXT("false"));

		CreateProc(*GetHTTPExePath(),*CreateParm);
	}

	void FObjectStorageClient::OSSPut(const FString& InPakInstallationPath, bool bMainVersion,const FString &InFilePath)
	{
		FString CreateParm = FString::Printf(
			TEXT("-log -Protocol=%i -VersionName=%s -AccessKeyId=%s -AccessKeySecret=%s -Bucket=%s -PatchVersionLogName=%s -ServerVersionName=%s\
 -VersionLock=%s -platform=%s -PakLocalPaths=%s -Endpoint=%s -InstallationPath=%s -bMainVersion=%s -FilePath=%s"),
			(int32)EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST,//对应协议
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->VersionName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->AccessKeyId),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->AccessKeySecret),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->BucketName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.PatchVersionLogName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.VersionLock.ToString()),
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
			*GetCheckValue(InFilePath.IsEmpty() ? GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path :TEXT("")),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->Endpoint),
			*GetCheckValue(InPakInstallationPath),
			*GetCheckValue(bMainVersion ? TEXT("true"):TEXT("false")),
			*GetCheckValue(InFilePath.IsEmpty()? InFilePath :FPaths::ConvertRelativePathToFull(InFilePath)));

		CreateProc(*GetOSSExePath(), *CreateParm);
	}

	void FObjectStorageClient::HTTPPutVersion(const FString& InVersion)
	{
		if (InVersion.IsEmpty())
		{
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PutVersion_Version",
				"This interface cannot be called directly when the version information is empty."));

			return;
		}

		FString CreateParm = FString::Printf(
			TEXT("-Protocol=%i -Bucket=%s -ServerVersionName=%s -platform=%s -Endpoint=%s \
-VersionJson=%s"),
			(int32)EReUpdateProtocol::SP_UPLOAD_VERSION_INFORMATION_REQUEST,//对应协议
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->BucketName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->Endpoint),
			*InVersion);
		
		CreateProc(*GetHTTPExePath(), *CreateParm);
	}

	void FObjectStorageClient::OSSPutVersion(const FString& InVersion)
	{
		if (InVersion.IsEmpty())
		{
			FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PutVersion_Version",
				"This interface cannot be called directly when the version information is empty."));

			return;
		}
		FString CreateParm = FString::Printf(
			TEXT("-Protocol=%i -AccessKeyId=%s -AccessKeySecret=%s -Bucket=%s -ServerVersionName=%s -platform=%s -Endpoint=%s \
-VersionJson=%s"),
			(int32)EReUpdateProtocol::SP_UPLOAD_VERSION_INFORMATION_REQUEST,//对应协议
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->AccessKeyId),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->AccessKeySecret),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->BucketName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->Endpoint),
			*InVersion);

		CreateProc(*GetOSSExePath(), *CreateParm);
	}

	void FObjectStorageClient::LocalPutVersion(const FString& InVersion)
	{
		CheckLocalPutVersion(
		[&]()
		{
			FString CreateParm = FString::Printf(
				TEXT("-Protocol=%i -VersionName=%s -Bucket=%s -PatchVersionLogName=%s -ServerVersionName=%s\
-VersionLock=%s -platform=%s -BasedOnwhichVersionString=%s \
-LinkURL=%s -VersionJson=%s"),
				(int32)EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST,//对应协议
				*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->VersionName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->BucketName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.PatchVersionLogName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.VersionLock.ToString()),
				*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->BasedOnwhichVersion.FilePath),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.LinkURL),
				*InVersion);

			CreateProc(*GetLoclExePath(), *CreateParm);
		});
	}

	void FObjectStorageClient::LocalPut(const FString& InPakInstallationPath, bool bMainVersion, const FString& InFilePath)
	{
		CheckLocalPutVersion(
			[&]()
			{
				FString CreateParm = FString::Printf(
					TEXT("-Protocol=%i -VersionName=%s -Bucket=%s -PatchVersionLogName=%s -ServerVersionName=%s\
 -VersionLock=%s -platform=%s -PakLocalPaths=%s -InstallationPath=%s -bMainVersion=%s -FilePath=%s -BasedOnwhichVersionString=%s \
-LinkURL=%s -LocalVersionPath=%s -bHttps=%s"),
					(int32)EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST,//对应协议
					*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->VersionName.ToString()),
					*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->BucketName.ToString()),
					*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.PatchVersionLogName.ToString()),
					*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
					*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.VersionLock.ToString()),
					*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
					*GetCheckValue(FPaths::ConvertRelativePathToFull(InFilePath.IsEmpty() ? GetDefault<UReUnrealPakSettingsEditor>()->PakSavePaths.Path : TEXT(""))),
					*GetCheckValue(InPakInstallationPath),
					*GetCheckValue(bMainVersion ? TEXT("true") : TEXT("false")),
					*GetCheckValue(bMainVersion ? FPaths::ConvertRelativePathToFull(InFilePath): InFilePath),
					*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->BasedOnwhichVersion.FilePath.Contains(TEXT("../../../")) ? FPaths::ConvertRelativePathToFull(GetDefault<UReLocalSettingsEditor>()->BasedOnwhichVersion.FilePath) : GetDefault<UReLocalSettingsEditor>()->BasedOnwhichVersion.FilePath),
					*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.LinkURL),
					*GetCheckValue(FPaths::ConvertRelativePathToFull(GetDefault<UReLocalSettingsEditor>()->LocalVersionPath.Path)),
					GetDefault<UReLocalSettingsEditor>()->VersionConfig.bHttps ? TEXT("true") : TEXT("false"));

				CreateProc(*GetLoclExePath(), *CreateParm);

				//生成一个base
				if (UReLocalSettingsEditor *InLocalSettingsEditor = const_cast<UReLocalSettingsEditor*>(GetDefault<UReLocalSettingsEditor>()))
				{
					InLocalSettingsEditor->BasedOnwhichVersion.FilePath =
						GetDefault<UReLocalSettingsEditor>()->LocalVersionPath.Path /
						GetDefault<UReLocalSettingsEditor>()->BucketName.ToString()/
						GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform() /
						GetDefault<UReLocalSettingsEditor>()->VersionConfig.ServerVersionName.ToString();

					InLocalSettingsEditor->SaveObjectStorageConfig();
				}
			});
	}

	void FObjectStorageClient::HTTPUnlock()
	{
		FString CreateParm = FString::Printf(
			TEXT("-Protocol=%i -Bucket=%s -ServerVersionName=%s -platform=%s -Endpoint=%s"),
			(int32)EReUpdateProtocol::SP_CLOSE_LOCK_REQUEST,//对应协议
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->BucketName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->Endpoint));

		CreateProc(*GetHTTPExePath(), *CreateParm);
	}

	void FObjectStorageClient::OSSUnlock()
	{
		FString CreateParm = FString::Printf(
			TEXT("-Protocol=%i -AccessKeyId=%s -AccessKeySecret=%s -Bucket=%s -ServerVersionName=%s -platform=%s -Endpoint=%s"),
			(int32)EReUpdateProtocol::SP_CLOSE_LOCK_REQUEST,//对应协议
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->AccessKeyId),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->AccessKeySecret),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->BucketName.ToString()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
			*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
			*GetCheckValue(GetDefault<UReOSSServerSettingsEditor>()->Endpoint));

		CreateProc(*GetOSSExePath(), *CreateParm);
	}

	void FObjectStorageClient::LocalUnlock()
	{
		CheckLocalPutVersion(
			[&]()
			{
				FString CreateParm = FString::Printf(
					TEXT("-Protocol=%i -VersionName=%s -Bucket=%s -PatchVersionLogName=%s -ServerVersionName=%s \
-VersionLock=%s -platform=%s -LocalVersionPath=%s"),
				(int32)EReUpdateProtocol::SP_CLOSE_LOCK_REQUEST,//对应协议
				*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->VersionName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->BucketName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.PatchVersionLogName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.ServerVersionName.ToString()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->VersionConfig.VersionLock.ToString()),
				*GetCheckValue(GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform()),
				*GetCheckValue(GetDefault<UReLocalSettingsEditor>()->LocalVersionPath.Path));

				CreateProc(*GetLoclExePath(), *CreateParm);
			});
	}
}

#undef LOCTEXT_NAMESPACE