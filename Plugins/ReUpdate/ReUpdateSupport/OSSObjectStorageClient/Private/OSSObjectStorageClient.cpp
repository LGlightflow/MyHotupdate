// Copyright (C) RenZhai.2020.All Rights Reserved.

#include "OSSObjectStorageClient.h"

#include "RequiredProgramMainCPPInclude.h"
#include "ReOSSManage.h"
#include "OSSObjectStorageClientMacro.h"
#include "Protocol/ReUpdateProtocol.h"

DEFINE_LOG_CATEGORY_STATIC(LogOSSObjectStorageClient, Log, All);

IMPLEMENT_APPLICATION(OSSObjectStorageClient, "OSSObjectStorageClient");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogOSSObjectStorageClient, Display, TEXT("Hello World"));

	auto GetParseValue = [](const FString &InKey)->FString
	{
		FString Value;
		if (!FParse::Value(FCommandLine::Get(),*InKey, Value))
		{
			UE_LOG(LogOSSObjectStorageClient, Error, TEXT("%s was not found value"),*InKey);
		}

		return Value;
	};

	//获取协议号
	EReUpdateProtocol Protocol = (EReUpdateProtocol)FCString::Atoi(*GetParseValue(TEXT("-Protocol=")));

	//账户
	FString AccessKeyId = GetParseValue(TEXT("-AccessKeyId="));
	FString AccessKeySecret = GetParseValue(TEXT("-AccessKeySecret="));
	FString Endpoint = GetParseValue(TEXT("-Endpoint="));

	FString Bucket = GetParseValue(TEXT("-Bucket="));
	FString PatchVersionLogName = GetParseValue(TEXT("-PatchVersionLogName="));
	FString ServerVersionName = GetParseValue(TEXT("-ServerVersionName="));
	FString VersionLock = GetParseValue(TEXT("-VersionLock="));
	FString Platform = GetParseValue(TEXT("-platform="));
	FString PakLocalPaths = GetParseValue(TEXT("-PakLocalPaths="));
	FString FilePath = GetParseValue(TEXT("-FilePath="));
	FString VersionName = GetParseValue(TEXT("-VersionName="));

	FString VersionJson = GetParseValue(TEXT("-VersionJson="));

	FString InstallationPath = GetParseValue(TEXT("-InstallationPath="));
	bool bMainVersion = GetParseValue(TEXT("-bMainVersion=")).ToBool();

	//账户初始化
	RE_OSS.InitAccounts(AccessKeyId, AccessKeySecret, Endpoint);

	if (RE_OSS.DoesBucketExist(Bucket))
	{
		switch (Protocol)
		{
			case EReUpdateProtocol::SP_NONE_REQUEST:
			{
				exit((int32)EReUpdateProtocol::SP_INVALID_RESPONSE);
				break;
			}
			case EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST:
			{
				CHECK_OSS_LOCK_STATUS(
					(int32)EReUpdateProtocol::SP_SERVER_LOCKED_RESPONSE,
					Bucket, VersionLock, Platform, );

				if (Bucket != TEXT("NONE"))
				{
					FVersion Version;
					OSSObjectStorageClientMethod::GetServerVersion(Bucket, ServerVersionName, Platform, Version);

					//判断版本是否一样 如果一样代表热更新 不一样代表有新版本上传
					Version.Crc = FGuid::NewGuid().ToString();
					Version.Name = VersionName;

					UE_LOG(LogOSSObjectStorageClient, Display, TEXT("new version.crc=%s"), *Version.Crc);
					UE_LOG(LogOSSObjectStorageClient, Display, TEXT("new version.Name=%s"), *Version.Name);
					
					FString URL = TEXT("https://") + Bucket + TEXT(".") + RE_OSS.GetEndpoint();
					
					UE_LOG(LogOSSObjectStorageClient, Display, TEXT("URL=%s"), *URL);
					
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

							FString ObjectName = Platform / TEXT("Patch") / PatchName;
							FString FullPath = FPaths::ConvertRelativePathToFull(NewPath);

							UE_LOG(LogOSSObjectStorageClient, Display, TEXT("ObjectName=%s"), *ObjectName);
							UE_LOG(LogOSSObjectStorageClient, Display, TEXT("FullPath=%s"), *FullPath);
							UE_LOG(LogOSSObjectStorageClient, Display, TEXT("Bucket=%s"), *Bucket);

							UE_LOG(LogOSSObjectStorageClient, Display, TEXT("Starting upload, please wait..."));

							if (RE_OSS.PutObject(Bucket, FullPath, ObjectName))
							{
								UE_LOG(LogOSSObjectStorageClient, Display, TEXT("PutObject %s succeeded"), *Bucket);
								
								RemoteDataDescribe->Link = URL / ObjectName;
								if (!bExists)
								{
									UE_LOG(LogOSSObjectStorageClient, Display, TEXT("RemoteDataDescribe->Link"), *RemoteDataDescribe->Link);
									
									Version.Content.Add(*RemoteDataDescribe);
								}

								UE_LOG(LogOSSObjectStorageClient, Display, TEXT("Upload %s succeeded."), *FPaths::ConvertRelativePathToFull(NewPath));
							}
							else
							{
								UE_LOG(LogOSSObjectStorageClient, Error, TEXT("Upload %s fail."), *FPaths::ConvertRelativePathToFull(NewPath));
								
								exit((int32)EReUpdateProtocol::SP_FAIL_RESPONSE);
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
						return -2;
					}

					for (const auto& Tmp : PakPaths)
					{
						UpdateNewData(Tmp);
					}

					if (OSSObjectStorageClientMethod::PutServerVersion(Bucket, ServerVersionName, Platform, Version))
					{
						UE_LOG(LogOSSObjectStorageClient, Display, TEXT("Upload %s succeeded."), *Bucket);
					}
					else
					{
						UE_LOG(LogOSSObjectStorageClient, Error, TEXT("Upload %s fail."), *Bucket);
					}
				}

				break;
			}
			case EReUpdateProtocol::SP_UPLOAD_VERSION_INFORMATION_REQUEST:
			{
				if (VersionJson.IsEmpty())
				{
					exit((int32)EReUpdateProtocol::SP_CHECK_VERSION_INFO_IS_EMPTY_RESPONSE);
				}

				if (OSSObjectStorageClientMethod::PutServerVersion(Bucket, ServerVersionName, Platform, VersionJson))
				{
					UE_LOG(LogOSSObjectStorageClient, Display, TEXT("Upload %s succeeded."), *Bucket);
				}
				else
				{
					UE_LOG(LogOSSObjectStorageClient, Error, TEXT("Upload %s fail."), *Bucket);
				}

				break;
			}
			case EReUpdateProtocol::SP_CLOSE_LOCK_REQUEST:
			{
				if (!RE_OSS.PutObjectByMemory(Bucket, Platform / VersionLock, TEXT("false")))
				{
					exit((int32)EReUpdateProtocol::SP_CLOSE_LOCK_FAIL_RESPONSE);
				}

				break;
			}
		}
	}
	else
	{
		UE_LOG(LogOSSObjectStorageClient, Error, TEXT("%s Bucket does not exist."), *Bucket);
	}

	FEngineLoop::AppExit();

	exit((int32)EReUpdateProtocol::SP_OK_RESPONSE);
}
