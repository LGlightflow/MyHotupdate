

#include "HTTPObjectStorageClient.h"
#include "ReHttpManage.h"
#include "RequiredProgramMainCPPInclude.h"
#include "ReHTTPType.h"
#include "HTTPObjectStorageClientLog.h"
#include "HTTPObjectStorageClientMethod.h"
#include "Protocol/ReUpdateProtocol.h"
#include "Async/Async.h"

using namespace HTTPObjectStorageClientMethod;

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

IMPLEMENT_APPLICATION(HTTPObjectStorageClient, "HTTPObjectStorageClient");

INT32_MAIN_INT32_ARGC_TCHAR_ARGV()
{
	GEngineLoop.PreInit(ArgC, ArgV);
	UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Hello World"));
	
	//账户
	FString VersionName = GetParseValue(TEXT("-VersionName="));

	//获取协议号
	EReUpdateProtocol Protocol = (EReUpdateProtocol)FCString::Atoi(*GetParseValue(TEXT("-Protocol=")));
	
	FString Bucket = GetParseValue(TEXT("-Bucket="));
	FString PatchVersionLogName = GetParseValue(TEXT("-PatchVersionLogName="));
	FString ServerVersionName = GetParseValue(TEXT("-ServerVersionName="));
	FString VersionLock = GetParseValue(TEXT("-VersionLock="));
	FString Platform = GetParseValue(TEXT("-platform="));
	FString PakLocalPaths = GetParseValue(TEXT("-PakLocalPaths="));
	FString FilePath = GetParseValue(TEXT("-FilePath="));
	FString Endpoint = GetParseValue(TEXT("-Endpoint="));

	FString VersionJson = GetParseValue(TEXT("-VersionJson="));

	FString InstallationPath = GetParseValue(TEXT("-InstallationPath="));
	bool bMainVersion = GetParseValue(TEXT("-bMainVersion=")).ToBool();

	bool bHttps = GetParseValue(TEXT("-bHttps=")).ToBool();

	/*
	-AccessKeyId=LTAI4GKLH9HPGM1rbcL3JcY6 
	-AccessKeySecret=xxx 
	-Endpoint=oss-cn-shanghai.aliyuncs.com 
	-Bucket=osss-erver-test 
	-PatchVersionLogName=Version/VersionLog.con 
	-ServerVersionName=Version/ServerVersion.con 
	-VersionLock=Version/MutexVersion.con 
	-platform=windows 
	-PakLocalPaths=P:/ReUpdate/Save/Pak/
	*/

	//测试
	//FReHTTPResponseDelegate Delegate;
	//FString URL = TEXT("http://192.168.31.137");
	//FString Save = FPaths::ProjectDir() / TEXT("wordpress-5.7.2.zip");
	//RE_HTTP.GetObjectToLocal(Delegate, URL/TEXT("wordpress-5.7.2.zip"), Save);
	//FString LocalAsset = FPaths::ConvertRelativePathToFull( FPaths::ProjectDir() / TEXT("xxx1.zip"));

	//RE_HTTP.PutObjectFromLocal(Delegate, URL / TEXT("xxx1.zip"), LocalAsset);

	int32 ReturnValue = 0;
	int32 UpdateFileNumber = 0;

	//服务器版本
	FVersion Version;

	FReHTTPResponseDelegate Delegate;	
	switch (Protocol)
	{
		case EReUpdateProtocol::SP_NONE_REQUEST:
		{
			//直接返回
			exit((int32)EReUpdateProtocol::SP_INVALID_RESPONSE);
			break;
		}
		case EReUpdateProtocol::SP_PUT_TO_SERVER_REQUEST:
		{
			if (ReturnValue==0)
			{
				auto RemovePathHead = [](const FString& InPath)->FString
				{
					if (InPath.Contains("."))
					{
						return FPaths::GetPath(InPath);
					}

					return TEXT("");
				};

				FString URL = bHttps ? TEXT("https://") : TEXT("http://") + Endpoint / HTTPServerScriptPathName;
				FString Prams = FString::Printf(
					TEXT("Protocol=%i&Bucket=%s&PatchVersionLogName=%s&ServerVersionName=%s&VersionLock=%s&Platform=%s"),
					EHTTPObjectStorageProtocol::HSP_INIT,
					*Bucket,
					*RemovePathHead(PatchVersionLogName),
					*RemovePathHead(ServerVersionName),
					*RemovePathHead(VersionLock),
					*Platform);

				FReHTTPResponseDelegate ResponseDelegate;
				ResponseDelegate.ReCompleteDelegate.BindLambda(
				[&](
					const FReHttpRequest &InHttpRequest,
					const FReHttpResponse &InHttpResponse, 
					bool bLink)
				{
					UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("bLink = %s"), bLink ? TEXT("True" : TEXT("False")));
				});

				RE_HTTP.PostRequest(*URL, *Prams, ResponseDelegate, true);

				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Hello World!!!!!!!"));

				bool bLock = false;
				CheckHTTPServerLock([&](bool bNewLock)
				{
					bLock = bNewLock;
				});

				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("bLock = %i"), bLock);
				if (bLock)
				{
					UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("The http server is currently used by other operators."));
					exit((int32)EReUpdateProtocol::SP_SERVER_LOCKED_RESPONSE);
				}
				else
				{
					HTTPServerLock([&](bool) {});

					UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("-------------Server lock-----------"));
					GetHTTPServerVersion([&](const FString& Msg)
					{
						if (Msg != TEXT("Error"))
						{
							ReVersionControl::Read(Msg, Version);
						}
					});

					//创建新版本
					Version.Crc = FGuid::NewGuid().ToString();
					Version.Name = VersionName;

					URL = bHttps ? TEXT("https://") : TEXT("http://") + Endpoint / Bucket / Platform /TEXT("Version/Patch");
					UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Get URL = %s"), *URL);

					//单个对象上传成功
					Delegate.ReCompleteDelegate.BindLambda(
					[&](const FReHttpRequest& InHttpRequest, const FReHttpResponse& InHttpResponse, bool bLink)
					{
						if (bLink)
						{
							if (InHttpResponse.ResponseCode == 200)
							{
								bool bExists = false;
								FString PatchName = FPaths::GetCleanFilename(InHttpRequest.URL);

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

								RemoteDataDescribe->Len = InHttpRequest.ContentLength;
								RemoteDataDescribe->Crc = FGuid::NewGuid().ToString();
								RemoteDataDescribe->Link = InHttpRequest.URL;

								//主版本和安装路径
								{
									RemoteDataDescribe->InstallationPath = InstallationPath;
									RemoteDataDescribe->bMajorVersion = bMainVersion;
								}

								if (!bExists)
								{
									Version.Content.Add(*RemoteDataDescribe);
								}

								UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Update %s is OK."), *PatchName);
							}
							else if (InHttpResponse.ResponseCode == 500)
							{
								UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("Code = 500 ->%s"), *InHttpResponse.ResponseMessage);
								
								FString Error;
								FPlatformProcess::LaunchURL(
									TEXT("https://blog.csdn.net/qq_23369807/article/details/120058597"),NULL,&Error);

								UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("Error=%s"), *Error);
							}
							else if (InHttpResponse.ResponseCode == 403)
							{
								UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("Code = 403->%s"), *InHttpResponse.ResponseMessage);
								
							}
						}
						else
						{
							UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Update %s is not OK."), *URL);
						}

						UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("URL = %s"),*URL);
							
						UpdateFileNumber--;
					});

					Delegate.ReSingleRequestProgressDelegate.BindLambda(
					[](const FReHttpRequest& InRequest, int64 BytesSent, int64 BytesReceived)
					{
						if (InRequest.ContentLength)
						{
							float ProgressValue =(float)BytesSent / (float)InRequest.ContentLength;
							UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Send HTTPServer Progress = %f %%"),ProgressValue * 100.f);
						}					
					});
							
					if (FilePath != TEXT("NONE"))
					{
						URL /= FPaths::GetCleanFilename(FilePath);

						RE_HTTP.PutObjectFromLocal(Delegate, URL, FilePath);

						UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Update To HTTP Server FilePath = %s.URL=%s"),*FilePath,*URL);
					}
					else if (PakLocalPaths != TEXT("NONE"))
					{
						//这种上传方式似乎有些问题 问题先放放，以后再解决
						//RE_HTTP.PutObjectsFromLocal(Delegate, URL, PakLocalPaths);

						TArray<FString> FindPaths;
						IFileManager::Get().FindFilesRecursive(FindPaths, *PakLocalPaths, TEXT("*"), true, false);
								
						UpdateFileNumber = FindPaths.Num();

						for (auto &Tmp : FindPaths)
						{
							URL /= FPaths::GetCleanFilename(Tmp);

							RE_HTTP.PutObjectFromLocal(Delegate, URL, Tmp);
						}						

						UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Update To HTTP Server PakLocalPaths = %s,URL=%s"), *PakLocalPaths, *URL);
								
						//FPlatformProcess::Sleep(1);
					} 
					else
					{
						ReturnValue = 2;

						UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Error Return 2."));
					}
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

			bool bLock = false;
			CheckHTTPServerLock([&](bool bMewLock)
			{
				bLock = bMewLock;
				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("bLock = %i"), bLock);
			});

			if (bLock)
			{
				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("The http server is currently used by other operators."));
				exit((int32)EReUpdateProtocol::SP_SERVER_LOCKED_RESPONSE);
			}
			else
			{
				HTTPObjectStorageClientMethod::PutServerVersion(VersionJson,[&](bool bLink)
				{
					UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("PutServerVersion is OK."));
					if (bLink)
					{

					}					
				});

				//解锁
				HTTPObjectStorageClientMethod::HTTPServerUnLock([&](bool)
				{
					UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("-------------Server unlock-----------"));
					exit((int32)EReUpdateProtocol::SP_OK_RESPONSE);
				});
			}

			break;
		}
		case EReUpdateProtocol::SP_CLOSE_LOCK_REQUEST:
		{
			//解锁
			HTTPObjectStorageClientMethod::HTTPServerUnLock([](bool)
			{
				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("-------------Server unlock-----------"));
				exit((int32)EReUpdateProtocol::SP_OK_RESPONSE);
			});
			break;
		}
	}

	auto EndFunc = [&]()
	{
		HTTPObjectStorageClientMethod::PutServerVersion(Version, [](bool bLink)
		{	
			if (bLink)
			{
				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("PutServerVersion is OK."));
			}
			else
			{
				UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("Failed to upload server version."));
			}
		});

		//解锁
		HTTPObjectStorageClientMethod::HTTPServerUnLock([](bool bLink)
		{
			UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("-------------Server unlock-----------"));
			if (bLink)
			{
				UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("ServerUnLock is OK."));
			}
			else
			{
				UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("Failed to unlock server."));
			}
			//exit((int32)EReUpdateProtocol::SP_OK_RESPONSE);
		});
	};

	double LastTime = FPlatformTime::Seconds();
	while (UpdateFileNumber != INDEX_NONE)
	{
		FPlatformProcess::Sleep(0.01f);

		double Now = FPlatformTime::Seconds();
		float DeltaSenconds = Now - LastTime;
		FReHttpManage::Get()->Tick(DeltaSenconds);

		if (UpdateFileNumber <= 0)
		{
			EndFunc();
			UpdateFileNumber = INDEX_NONE;
		}

		LastTime = Now;
	} 

	FEngineLoop::AppExit();
	return ReturnValue;
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif