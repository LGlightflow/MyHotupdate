#include "Object/ReUpdateObject.h"
#include "Log/ReUpdateLog.h"
#include "Misc/FileHelper.h"
#include "HttpManager.h"
#include "Settings/ObjectStorageSettingsRuntime.h"
#include "RePakBPLibrary.h"
#include "GameMapsSettings.h"
#include "Kismet/GameplayStatics.h"
#include "ReInstallationManage.h"

#if PLATFORM_WINDOWS
#pragma optimize("",off) 
#endif

float PakNumber = 0.f;
float CustomPakNumber = 0.f;
float LastFraction = 0.f;
float PercentageInterval = 0.f;
float ProgressInstallationPercentage = 0.f;

struct FPakInstallationProgress :public FCopyProgress
{
	FPakInstallationProgress(){}
	virtual ~FPakInstallationProgress() {}

	virtual bool Poll(float Fraction)
	{
		PercentageInterval = Fraction - LastFraction;

		ProgressInstallationPercentage += (PercentageInterval / (PakNumber + CustomPakNumber));

		if (Fraction == 1.f)
		{
			LastFraction = 0.f;
		}
		else
		{
			LastFraction = Fraction;
		}

		return true;
	}
};

UVersionControlObject::UVersionControlObject()
{
	bInitialization = false;
	bStartVersionDetection = false;
	VersionDetectionTime = 0.f;
	bMajorVersion = false;
}

UVersionControlObject* UVersionControlObject::CreateObject(UClass* InClass, UObject* InParent)
{
	if (InParent)
	{
		if (UVersionControlObject* Obj = NewObject<UVersionControlObject>(InParent, InClass))
		{
			UE_LOG(LogReVersionControl, Log, TEXT("Create an object with a parent."));
			Obj->InitVersionControlObject();
			return Obj;
		}
	}
	else
	{
		if (UVersionControlObject* Obj = NewObject<UVersionControlObject>(NULL, InClass))
		{
			Obj->AddToRoot();
			Obj->InitVersionControlObject();

			UE_LOG(LogReVersionControl, Log, TEXT("Create an object with a parent."));
			return Obj;
		}
	}

	UE_LOG(LogReVersionControl, Error, TEXT("Creation failed for unknown reason."));

	return nullptr;
}

bool UVersionControlObject::InitVersion()
{
	if (!bInitialization)
	{
		//充值程序进度
		ResetProgramProgress();

		if (GetCurrentVersion(ServerVersion))
		{
			IterativeUpdating();
		}

		bInitialization = true;
		return true;
	}

	return false;
}

bool UVersionControlObject::GetCurrentVersion(FVersionInfor &OutVersion, bool bSynchronous)
{
	bool bConConnectNet = false;

	UE_LOG(LogReVersionControl, Display, TEXT("LinkURL=%s"),*GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.LinkURL);
	UE_LOG(LogReVersionControl, Display, TEXT("Platform=%s"), UTF8_TO_TCHAR(FPlatformProperties::IniPlatformName()));
	UE_LOG(LogReVersionControl, Display, TEXT("ServerVersionName=%s"), *GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.ServerVersionName.ToString());

	FString ServerVersionURL = FString(GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.bHttps ? TEXT("https:/") : TEXT("http:/"))/
		GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.LinkURL /
		GetMutableDefault<UObjectStorageSettingsRuntime>()->VersionConfig.PakPlatform /
		GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.ServerVersionName.ToString();

	UE_LOG(LogReVersionControl, Display, TEXT("ServerVersionURL=%s"),*ServerVersionURL);

	ReUpdateHTTP::FHTTPDelegate Delegate;
	Delegate.ReCompleteDelegate.BindLambda([&, bSynchronous](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConConnect)
	{
		if (bConConnect)
		{
			/*if (bSynchronous)
			{
				OutVersion.Empty();
				if (!ReVersionControl::Read(Response->GetContentAsString(), OutVersion))
				{
					UE_LOG(LogReVersionControl, Error, TEXT("The request for object server version information failed. Please check whether the permissions of the following OSS servers are public read."));
				}

				bConConnectNet = true;
			}
			else*/
			{
				FVersionInfor NewVersion;
				if (!ReVersionControl::Read(Response->GetContentAsString(), NewVersion))
				{
					UE_LOG(LogReVersionControl, Error, TEXT("The request for object server version information failed. Please check whether the permissions of the following OSS servers are public read."));
				}

				//比对服务器版本
				OnCompareServerVersions(NewVersion);	
			}
		}

		bConConnectNet = true;
	});

	bool bCreateHTTPObject = ReUpdateHTTP::FHTTP::CreateHTTPObject(Delegate)->GetObjectToMemory(ServerVersionURL);

	if (bSynchronous)
	{
		FHttpModule::Get().GetHttpManager().Flush(EHttpFlushReason::Default);
	}
	else
	{
		bConConnectNet = bCreateHTTPObject;
	}

	return bConConnectNet;
}

void UVersionControlObject::Tick(float DeltaTime)
{
	if (bStartVersionDetection && bInitialization)
	{
		VersionDetectionTime += DeltaTime;
		if (VersionDetectionTime >= GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.PerSecondVersionDetection)
		{
			VersionDetectionTime = 0.f;

			//开启异步
			FVersionInfor OutVersion;
			GetCurrentVersion(OutVersion,false);
		}
	}
}

TStatId UVersionControlObject::GetStatId() const
{
	return TStatId();
}

void UVersionControlObject::SetServerVersion(const FVersionInfor& InServerVersion)
{
	ServerVersion = InServerVersion;
}

void UVersionControlObject::IterativeUpdating()
{
	EServerVersionResponseType InResponseType = GetValidationVersion();
	switch (InResponseType)
	{
	case EServerVersionResponseType::VERSION_DIFF:
	case EServerVersionResponseType::CRC_DIFF:
	{
		UpdateVersion();
		break;
	}
	case EServerVersionResponseType::EQUAL:
	{
		bStartVersionDetection = GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.bStartVersionDetection;
		break;
	}
	case EServerVersionResponseType::CONNECTION_ERROR:
		break;
	case EServerVersionResponseType::MAX:
		break;
	}

	OnVersionDelegate.Broadcast(InResponseType);
}

EServerVersionResponseType UVersionControlObject::GetValidationVersion()
{
	if (ClientVersion.Name != ServerVersion.Name)
	{
		return EServerVersionResponseType::VERSION_DIFF;
	}
	else if (ClientVersion.Crc != ServerVersion.Crc)
	{
		return EServerVersionResponseType::CRC_DIFF;
	}

	return EServerVersionResponseType::EQUAL;
}

void UVersionControlObject::UpdateVersion()
{
	//I 检测要更新的pak
	TArray<FRemoteDataDescribe> LinkArray;
	for (auto& ServerTmp : ServerVersion.Content)
	{
		if (!ServerTmp.bDiscard)
		{
			for (auto& ClientTmp : ClientVersion.Content)
			{
				if (ServerTmp.Link == ClientTmp.Link)
				{
					if (ServerTmp.Crc != ClientTmp.Crc || 
						ClientTmp.bDiscard)//上一个版本可能丢弃过，这个版本加回来了
					{
						LinkArray.AddUnique(ServerTmp);
					}

					break;
				}
			}
		}
	}

	//II 检测要添加的pak
	for (auto& ServerTmp : ServerVersion.Content)
	{
		if (ServerTmp.bDiscard)
		{
			continue;
		}

		bool bExitPak = false;
		for (auto& ClientTmp : ClientVersion.Content)
		{
			if (ServerTmp.Link == ClientTmp.Link)
			{
				bExitPak = true;
				break;
			}
		}

		if (!bExitPak)
		{
			LinkArray.AddUnique(ServerTmp);
		}
	}

	//检查主版本
	for (auto &Tmp : LinkArray)
	{
		if (!Tmp.bDiscard)
		{
			if (Tmp.bMajorVersion)
			{
				bMajorVersion = true;
				break;
			}
		}
	}

	//IV 下载
	if (LinkArray.Num() > 0)
	{
		//组装Link
		TArray<FString> Links;
		for (auto &Tmp : LinkArray)
		{
			Links.Add(Tmp.Link);
		}

		if (Links.Num() > 0)
		{
			ReUpdateHTTP::FHTTPDelegate HTTPDelegate;
			HTTPDelegate.ReCompleteDelegate.BindLambda([&](FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
			{
				OnRequestComplete(HttpRequest, HttpResponse, bSucceeded);
			});
			HTTPDelegate.ReSingleRequestHeaderReceivedDelegate.BindLambda([&](FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue)
			{
				OnRequestHeaderReceived(Request, HeaderName, NewHeaderValue);
			});
			HTTPDelegate.ReSingleRequestProgressDelegate.BindLambda([&](FHttpRequestPtr HttpRequest, int32 TotalBytes, int32 BytesReceived)
			{
				OnRequestProgress(HttpRequest, TotalBytes, BytesReceived);
			});
			HTTPDelegate.AllTasksCompletedDelegate.BindLambda([&]()
			{
				OnRequestsComplete();
			});

			ReUpdateHTTP::FHTTP* InHttp = ReUpdateHTTP::FHTTP::CreateHTTPObject(HTTPDelegate);

			InHttp->GetObjectsToMemory(Links);
		}
	}
	else //可能用户只更新了版本，比如丢弃了某个包
	{
		UE_LOG(LogReVersionControl, Display, TEXT("Version changes may result in discarding."));

		OnRequestsComplete();
	}
}

void UVersionControlObject::ResetProgramProgress()
{
	PakNumber = 0.f;
	CustomPakNumber = 0.f;
	LastFraction = 0.f;
	PercentageInterval = 0.f;
	ProgressInstallationPercentage = 0.f;
}

void ExistsAndCreate(const FString &InNewPath)
{
	if (!IFileManager::Get().DirectoryExists(*InNewPath))
	{
		IFileManager::Get().MakeDirectory(*InNewPath, true);
	}
}

FString UVersionControlObject::GetDownLoadPersistencePath()
{
	FString DownloadPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Patch"));
	ExistsAndCreate(DownloadPath);

	return DownloadPath;
}

FString UVersionControlObject::GetDownLoadPersistenceTmpPath()
{
	FString DownloadPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("RelativePatch"));
	ExistsAndCreate(DownloadPath);
	return DownloadPath;
}

void UVersionControlObject::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	FString HeadName = FPaths::GetCleanFilename(HttpRequest->GetURL());

	//保存本地
	if (bSucceeded)
	{
		if (FRemoteDataDescribe* InDescribe = ServerVersion.Content.FindByPredicate([&](const FRemoteDataDescribe& InDescr)
			{
				return InDescr.Link == HttpRequest->GetURL();
			}))
		{
			FString DataFilename;
			if (InDescribe->InstallationPath == TEXT("NONE") || InDescribe->bMajorVersion)
			{
				DataFilename = GetDownLoadPersistencePath() / HeadName;
			}
			else
			{
				DataFilename = GetDownLoadPersistenceTmpPath() / HeadName;

				//为独立程序搜集相对非Content的路径
				{
					FString InstallationPath = InDescribe->InstallationPath;
					FPaths::NormalizeFilename(InstallationPath);

					InstallationPath.RemoveFromEnd(TEXT("/"));
					InstallationPath.RemoveFromStart(TEXT("/"));

					FString NewPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / InstallationPath);

					//检查路径是否存在
					ExistsAndCreate(NewPath);

					NewPath /= HeadName;
					RelativePatchs.Add(NewPath);
				}
			}

			FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *DataFilename);
			UE_LOG(LogReVersionControl, Log, TEXT("The [%s] was stored successfully.[%s]"), *HeadName,*DataFilename);
		}
		else
		{
			UE_LOG(LogReVersionControl, Error, TEXT("The [%s] No corresponding data found."), *HeadName);
		}
	}
	else
	{
		UE_LOG(LogReVersionControl, Error, TEXT("Failed to store the [%s] =>[%s] "),*HeadName,*HttpResponse->GetContentAsString());
	}

	OnObjectCompleteBPDelegate.Broadcast(HeadName);
}

void UVersionControlObject::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 TotalBytes, int32 BytesReceived)
{
	FString Head = FPaths::GetCleanFilename(HttpRequest->GetURL());

	float InValue = ((float)BytesReceived / (float)TotalBytes);
	UE_LOG(LogReVersionControl, Log, TEXT("[%s] : %.2lf %%"),*Head, InValue * 100.f);

	OnProgressDelegate.Broadcast(InValue, Head, TotalBytes, BytesReceived);
}

void UVersionControlObject::OnRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue)
{

}

void UVersionControlObject::OnRequestsComplete()
{
	if (GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.bSeamlessHotUpdate)
	{
		if (ExecuteInstallationProgressByUninstallAll())
		{
			UE_LOG(LogReVersionControl, Log, TEXT("Start up project success!"));

			ClientVersion = ServerVersion;
			SerializeClientVersion();

			FString StartupMapGameDefaultMap = GetDefault<UGameMapsSettings>()->GetGameDefaultMap();
			if (StartupMapGameDefaultMap.IsEmpty())
			{
				UE_LOG(LogReVersionControl, Error, TEXT("The startup map name cannot be empty."));
				ensure(0);

				FPlatformMisc::RequestExit(true);
			}

			UE_LOG(LogReVersionControl, Display, TEXT("StartupMapGameDefaultMap = %s"), *StartupMapGameDefaultMap);

			UGameplayStatics::OpenLevel(GetWorld(), *StartupMapGameDefaultMap);
		}
		else
		{
			UE_LOG(LogReVersionControl, Error, TEXT("major version.There was a problem performing internal installation!"));

			ensure(0);
		}	
	}
	else
	{
		if (CallInstallationProgress())
		{
			UE_LOG(LogReVersionControl, Log, TEXT("Start up project success!"));

			ClientVersion = ServerVersion;
			SerializeClientVersion();

			//GIsRequestingExit = true;
			//GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
		}
		else
		{
			UE_LOG(LogReVersionControl, Error, TEXT("Start up project failure!"));
			UE_LOG(LogReVersionControl, Error, TEXT("No pull program found"));

			ensure(0);
		}

		//强制直接退出
		FPlatformMisc::RequestExit(true);
	}
}

void UVersionControlObject::OnCompareServerVersions(const FVersionInfor& InNewVersion)
{
	ServerVersion = InNewVersion;

	if (bStartVersionDetection)
	{
		EServerVersionResponseType VersionResponseType = GetValidationVersion();
		switch (VersionResponseType)
		{
		case EServerVersionResponseType::VERSION_DIFF:
		case EServerVersionResponseType::CRC_DIFF:
		{
			bStartVersionDetection = false;

			//重置进度表
			ResetProgramProgress();
			break;
		}
		case EServerVersionResponseType::EQUAL:
			bStartVersionDetection = true;
			break;
		case EServerVersionResponseType::CONNECTION_ERROR:
			ensure(0);//不应该错误 提醒程序员出现bug
			break;
		}

		//通知更上层用户 是否更新本次包
		OnRealTimeVersionDetectionDelegate.Broadcast(VersionResponseType);
	}
}

bool UVersionControlObject::ExecuteInstallationProgressByUninstallAll()
{
	//卸载所有包
	if (UnLoadAllPak())
	{
		FString ResourcesToCopiedCustom = GetDownLoadPersistenceTmpPath();
		FString ResourcesToCopied = GetDownLoadPersistencePath();
		FString ProjectToContentPaks = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Paks"));
		FString ProjectRootPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());

		TArray<FRemoteDataDescribe> DiscardInfo;
		for (auto& ServerTmp : ServerVersion.Content)
		{
			if (ServerTmp.bDiscard)
			{
				DiscardInfo.Add(ServerTmp);
			}
		}

		if (FReInstallationManage().HandleHotInstallation<FPakInstallationProgress>(
			ResourcesToCopied,
			ResourcesToCopiedCustom,
			ProjectToContentPaks,
			ProjectRootPath,
			RelativePatchs,
			DiscardInfo,
			PakNumber,
			CustomPakNumber,
			ProgressInstallationPercentage))
		{
			return LoadAllPakCache();
		}
	}

	return false;
}

bool UVersionControlObject::UnLoadAllPak()
{
	//清除所有Actor
	//for (int32 LevelIndex = 0; LevelIndex < GetWorld()->GetNumLevels(); LevelIndex++)
	//{
	//	ULevel* Level = GetWorld()->GetLevel(LevelIndex);
	//	
	//	for (auto &Tmp : Level->Actors)
	//	{
	//		UE_LOG(LogReVersionControl,Display, TEXT("Remove Actor .%s"), *Tmp->GetName());

	//		Tmp->Destroy();
	//	}
	//}

	//卸载所有包
	PakFileCache.Empty();
	URePakBPLibrary::GetMountedPakFilenames(PakFileCache);

	for (auto &Tmp : PakFileCache)
	{
		if (!URePakBPLibrary::UnmountPak(Tmp))
		{
			UE_LOG(LogReVersionControl,Error, TEXT("Failed to uninstall package .%s"),* Tmp);

			return false;
		}
		else
		{
			UE_LOG(LogReVersionControl, Log, TEXT("Successfully uninstalled package. %s"),*Tmp);
		}
	}

	return true;
}

bool UVersionControlObject::LoadAllPakCache()
{
	if (PakFileCache.Num() > 0)
	{
		for (auto &Tmp : PakFileCache)
		{
			if (!URePakBPLibrary::MountPak(Tmp,0,TEXT("")))
			{
				UE_LOG(LogReVersionControl, Error, TEXT("Installation failed It may have been uninstalled.%s"), *Tmp);
			}
			else
			{
				UE_LOG(LogReVersionControl, Log, TEXT("Successfully installed package. %s"), *Tmp);
			}
		}
	}

	return true;
}

bool UVersionControlObject::CallInstallationProgress()
{
	auto GetPlatformName = []()->FString
	{
		FString PlatformName = FPlatformProperties::PlatformName();
		if (PlatformName.Contains(TEXT("Windows")))
		{
			return TEXT("Win64");
		}

		return PlatformName;
	};

	//寻找被丢弃的pak
	FString DiscardInfo;
	for (auto& ServerTmp : ServerVersion.Content)
	{
		if (ServerTmp.bDiscard)
		{
			FString Json;
			ReVersionControl::Save(ServerTmp, Json);

			if (!Json.IsEmpty())
			{
				DiscardInfo += (Json + TEXT("|"));
			}		
		}
	}
	DiscardInfo.RemoveFromEnd(TEXT("|"));

	//检测
	if (DiscardInfo.IsEmpty())
	{
		DiscardInfo = TEXT("NONE");
	}

	UE_LOG(LogReVersionControl, Log, TEXT("Download task completed."));

	FString ExePath = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() / TEXT("Binaries") / GetPlatformName() / TEXT("ReInstallationProgress-Win64-DebugGame.exe"));

	FString Param;

	//-ResourcesToCopiedCustom=
	Param += TEXT("-ResourcesToCopiedCustom=") + GetDownLoadPersistenceTmpPath();
	//-ResourcesToCopied=
	Param += TEXT(" -ResourcesToCopied=") + GetDownLoadPersistencePath();
	//-ProjectToContentPaks=
	Param += TEXT(" -ProjectToContentPaks=") + FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() / TEXT("Paks"));
	//-ProjectStartupPath=
	Param += TEXT(" -ProjectStartupPath=") + FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Binaries") / GetPlatformName() / FApp::GetProjectName() + TEXT(".exe"));
	//-ProjectProcessID=
	Param += TEXT(" -ProjectProcessID=") + FString::FromInt(FPlatformProcess::GetCurrentProcessId());
	//-Discards=
	Param += TEXT(" -Discards=") + DiscardInfo;
	//-ProjectRootPath=
	Param += TEXT(" -ProjectRootPath=") + FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
	//-LinkURL=
	Param += TEXT(" -LinkURL=") + GetDefault<UObjectStorageSettingsRuntime>()->VersionConfig.LinkURL;

	if (RelativePatchs.Num())
	{
		FString RelativePatch;
		for (auto &Tmp : RelativePatchs)
		{
			RelativePatch += (FPaths::ConvertRelativePathToFull(Tmp) + TEXT("+"));
		}
		RelativePatch.RemoveFromEnd(TEXT("+"));

		//-RelativePatchs=
		Param += (TEXT(" -RelativePatchs=") + RelativePatch);
	}
	
	FProcHandle Handle = FPlatformProcess::CreateProc(*ExePath, *Param, false, false, false, nullptr, 0, nullptr, nullptr);

	UE_LOG(LogReVersionControl, Log, TEXT("[ExePath] : %s"), *ExePath);
	UE_LOG(LogReVersionControl, Log, TEXT("[Param] : %s"), *Param);

	return Handle.IsValid();
}

void UVersionControlObject::InitVersionControlObject()
{
	FString ClientVersionsPaths = FPaths::ConvertRelativePathToFull(GetClientVersionPosition());
	if (!IFileManager::Get().FileExists(*ClientVersionsPaths))
	{
		SerializeClientVersion();
	}
	else
	{
		DeserializationClientVersion();
	}
}

void UVersionControlObject::SerializeClientVersion()
{
	FString CVJosn;
	ReVersionControl::Save(ClientVersion,CVJosn);
	FFileHelper::SaveStringToFile(CVJosn ,*GetClientVersionPosition());
}

void UVersionControlObject::DeserializationClientVersion()
{
	FString CVJosn;
	FString ClientVersionPath = FPaths::ConvertRelativePathToFull(GetClientVersionPosition());

	FFileHelper::LoadFileToString(CVJosn,*ClientVersionPath);
	if (!CVJosn.IsEmpty())
	{
		ReVersionControl::Read(CVJosn,ClientVersion);
	}
}

FString UVersionControlObject::GetClientVersionPosition()
{
	return FPaths::ProjectSavedDir() / TEXT(".Re/ClientVersion.con");;
}

FString UVersionControlObject::GetClientVersionName()
{
	return ClientVersion.Name;
}

FString UVersionControlObject::GetServerVersionName()
{
	return ServerVersion.Name;
}

float UVersionControlObject::GetProgressInstallationPercentage()
{
	return ProgressInstallationPercentage;
}

#if PLATFORM_WINDOWS
#pragma optimize("",on) 
#endif