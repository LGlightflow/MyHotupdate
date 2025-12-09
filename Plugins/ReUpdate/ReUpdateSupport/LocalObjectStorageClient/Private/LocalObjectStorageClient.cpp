

#include "LocalObjectStorageClient.h"
#include "RequiredProgramMainCPPInclude.h"
#include "Protocol/ReUpdateProtocol.h"
#include "LocalObjectStorageClientLog.h"
#include "LocalObjectStorageClientMethod.h"

using namespace LocalObjectStorageClientMethod;

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

IMPLEMENT_APPLICATION(LocalObjectStorageClient, "LocalObjectStorageClient");

#define LOCTEXT_NAMESPACE "LocalObjectStorageClient"

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogLocalObjectStorageClient, Display, TEXT("Hello World"));
	
	//版本名称
	FString VersionName = GetParseValue(TEXT("-VersionName="));

	//获取协议号
	EReUpdateProtocol Protocol = (EReUpdateProtocol)FCString::Atoi(*GetParseValue(TEXT("-Protocol=")));
	
	FString BucketName = GetParseValue(TEXT("-Bucket="));
	FString PatchVersionLogName = GetParseValue(TEXT("-PatchVersionLogName="));
	FString ServerVersionName = GetParseValue(TEXT("-ServerVersionName="));
	FString VersionLock = GetParseValue(TEXT("-VersionLock="));
	FString Platform = GetParseValue(TEXT("-platform="));
	FString PakLocalPaths = GetParseValue(TEXT("-PakLocalPaths="));
	FString FilePath = GetParseValue(TEXT("-FilePath="));
	FString LinkURL = GetParseValue(TEXT("-LinkURL="));
	FString LocalVersionPath = GetParseValue(TEXT("-LocalVersionPath="));

	FString VersionJson = GetParseValue(TEXT("-VersionJson="));
	FString BasedOnwhichVersionString = GetParseValue(TEXT("-BasedOnwhichVersionString="));

	FString InstallationPath = GetParseValue(TEXT("-InstallationPath="));
	bool bMainVersion = GetParseValue(TEXT("-bMainVersion=")).ToBool();
	bool bHttps = GetParseValue(TEXT("-bHttps=")).ToBool();

	FString RootPath = LocalVersionPath / BucketName / Platform;

	FString VersionPath = RootPath / ServerVersionName;
	FString VersionLockPath = RootPath / VersionLock;

	switch (Protocol)
	{
		case EReUpdateProtocol::SP_NONE_REQUEST:
		{
			//直接返回
			FEngineLoop::AppExit();
			exit((int32)EReUpdateProtocol::SP_INVALID_RESPONSE);
			break;
		}
		case EReUpdateProtocol::SP_UPLOAD_VERSION_INFORMATION_REQUEST:
		{
			if (!FFileHelper::SaveStringToFile(VersionJson, *VersionPath))
			{
				FEngineLoop::AppExit();
				exit((int32)EReUpdateProtocol::SP_UPLOAD_VERSION_INFORMATION_FAIL_RESPONSE);
			}
			
			break;
		}
		case EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST:
		{
			IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

			FString BasedOnwhichVersion;
			if (BasedOnwhichVersionString.IsEmpty() || BasedOnwhichVersionString == TEXT("NONE"))
			{
				if (!PlatformFile.FileExists(*VersionPath))
				{
					if (EAppReturnType::No == FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("LocalPut_VersionPath_Check",
						"We didn't find the new version generated based on any version. If your version is the first version, click continue. If your version has a new version, this is only the next version. Click Cancel. The hot update will be considered as the first version.")))
					{
						FEngineLoop::AppExit();
						exit((int32)EReUpdateProtocol::SP_CHECK_VERSION_PATH_IS_EMPTY_RESPONSE);
					}
				}

				BasedOnwhichVersion = VersionPath;
			}
			else
			{
				if (BasedOnwhichVersionString != TEXT("NONE"))
				{
					//优先级最高
					BasedOnwhichVersion = BasedOnwhichVersionString;
				}
				else
				{
					FEngineLoop::AppExit();
					exit((int32)EReUpdateProtocol::SP_FAIL_RESPONSE);
				}
			}
			//I 生成基本文件
			auto CreateDirectory = [](const FString& InPath)
			{
				IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				if (!PlatformFile.DirectoryExists(*InPath))
				{
					PlatformFile.CreateDirectory(*InPath);
				}
			};

			CreateDirectory(RootPath);
			CreateDirectory(RootPath / TEXT("Patch"));
			FFileHelper::SaveStringToFile(TEXT("false"), *VersionLockPath);

			//II 拿到当前最新版本
			FVersion Version;
			FString VersionJsonString;
			FFileHelper::LoadFileToString(VersionJsonString, *BasedOnwhichVersion);
			if (!VersionJsonString.IsEmpty())
			{
				ReVersionControl::Read(VersionJsonString, Version);
			}

			//III 判断版本是否一样 如果一样代表热更新 不一样代表有新版本上传
			Version.Crc = FGuid::NewGuid().ToString();
			Version.Name = VersionName;

			FString URL = LinkURL;

			auto UpdateNewData = [&](const FString& NewPath)
			{
				if (!NewPath.IsEmpty())
				{
					bool bExists = false;

					FString PatchName = FPaths::GetCleanFilename(NewPath);

					FRemoteDataDescribe Describe;
					FRemoteDataDescribe* RemoteDataDescribe = &Describe;
					for (auto& TmpVersion : Version.Content)
					{
						FString LinkName = FPaths::GetCleanFilename(TmpVersion.Link);
						if (LinkName == PatchName)
						{
							RemoteDataDescribe = &TmpVersion;
							bExists = true;
							break;
						}
					}

					//安装和主版本
					{
						RemoteDataDescribe->InstallationPath = InstallationPath;
						RemoteDataDescribe->bMajorVersion = bMainVersion;
					}

					//大小
					{
						TArray<uint8> DataInstance;
						FFileHelper::LoadFileToArray(DataInstance, *NewPath);
						RemoteDataDescribe->Len = DataInstance.Num();
					}

					RemoteDataDescribe->Crc = FGuid::NewGuid().ToString();

					FString ObjectName = TEXT("Patch") / PatchName;

					FString Target = RootPath / ObjectName;
					if (LocalObjectStorageClientMethod::LocalPut(NewPath, Target))
					{
						FString DomainName = URL / Platform / ObjectName;
						auto InitHTTP=[&]()
						{
							for (auto &Tmp : Version.Content)
							{
								if (Tmp.Link.Contains(DomainName))
								{
									Tmp.Link = RemoteDataDescribe->Link;
								}
							}
						};

						RemoteDataDescribe->Link = bHttps ? TEXT("https://"): TEXT("http://") + DomainName;
						if (!bExists)
						{
							Version.Content.Add(*RemoteDataDescribe);
						}
						else //只更新 http或者是 https
						{
							if (bHttps)
							{
								if (!RemoteDataDescribe->Link.Contains(TEXT("https://")))
								{
									InitHTTP();
								}
							}
							else
							{
								if (!RemoteDataDescribe->Link.Contains(TEXT("http://")))
								{
									InitHTTP();
								}
							}
						}

						UE_LOG(LogLocalObjectStorageClient, Display, TEXT("Upload %s succeeded."), *FPaths::ConvertRelativePathToFull(NewPath));
					}
					else
					{
						UE_LOG(LogLocalObjectStorageClient, Error, TEXT("Upload %s fail."), *FPaths::ConvertRelativePathToFull(NewPath));
					}
				}
			};

			TArray<FString> PakPaths;
			if (FilePath != TEXT("NONE"))
			{
				PakPaths.Add(FilePath);
			}
			else if (PakLocalPaths != TEXT("NONE"))
			{
				IFileManager::Get().FindFilesRecursive(PakPaths, *PakLocalPaths, TEXT("*"), true, false);
			}
			else
			{
				FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("LocalPut_FilePak_Check",
					"Both local path and file path are empty. At least one must be set."));
				
				FEngineLoop::AppExit();
				exit((int32)EReUpdateProtocol::SP_CHECK_PAK_LOCAL_PATH_IS_EMPTY_RESPONSE);
			}

			//V上传本地版本
			for (const auto& Tmp : PakPaths)
			{
				UpdateNewData(Tmp);
			}

			//VI上传服务器版本
			VersionJsonString.Empty();
			ReVersionControl::Save(Version, VersionJsonString);

			FFileHelper::SaveStringToFile(VersionJsonString, *VersionPath);

			break;
		}
		case EReUpdateProtocol::SP_CLOSE_LOCK_REQUEST:
		{
			if (!FFileHelper::SaveStringToFile(TEXT("false"), *VersionLockPath))
			{
				FEngineLoop::AppExit();
				exit((int32)EReUpdateProtocol::SP_CLOSE_LOCK_FAIL_RESPONSE);
			}

			break;
		}
	}

	FEngineLoop::AppExit();
	exit((int32)EReUpdateProtocol::SP_OK_RESPONSE);
}

#undef LOCTEXT_NAMESPACE

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif