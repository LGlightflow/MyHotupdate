#include "ReInstallationManage.h"
#include "Version/ReVersion.h"

#define LOCTEXT_NAMESPACE "FReInstallationManage"

FReInstallationManage::FReInstallationManage()
{
}

FReInstallationManage::~FReInstallationManage()
{
}

void FReInstallationManage::CollectFiles(
	const FString& InResourcesToCopied,
	TArray<FString>& OutFoundFiles)
{
	//资源遍历
	UE_LOG(ReInstallationLog, Display, TEXT("Resource traversal."));

	if (!InResourcesToCopied.IsEmpty())
	{
		IFileManager::Get().FindFilesRecursive(OutFoundFiles, *InResourcesToCopied, TEXT("*"), true, false);
	}
}

void FReInstallationManage::CollectFilesCustom(
	const FString& InResourcesToCopiedCustom,
	const TArray<FString>& InRelativePatchs,
	TArray<FString>& OutCustomFoundFiles,
	TArray<FReInstallationCustomCopyPath>& OutCustomCopyPaths)
{
	UE_LOG(ReInstallationLog, Display, TEXT("Resource Custom traversal."));

	//收集自定义拷贝路径
	if (InRelativePatchs.Num() > 0)
	{
		IFileManager::Get().FindFilesRecursive(OutCustomFoundFiles, *InResourcesToCopiedCustom, TEXT("*"), true, false);

		for (auto& Tmp : InRelativePatchs)
		{
			FString RelativePatchKey = FPaths::GetCleanFilename(Tmp);
			for (auto& CustomTmp : OutCustomFoundFiles)
			{
				FString CustomFoundFilesKey = FPaths::GetCleanFilename(CustomTmp);
				if (RelativePatchKey == CustomFoundFilesKey)
				{
					FReInstallationCustomCopyPath& CopyPath = OutCustomCopyPaths.AddDefaulted_GetRef();
					CopyPath.From = CustomTmp;
					CopyPath.To = Tmp;
					break;
				}
			}
		}
	}
}

void FReInstallationManage::Wait(float& InProgressInstallationPercentage, int32 InPakNumber, int32 InCustomPakNumber)
{
	if (InPakNumber != 0 || InCustomPakNumber != 0)
	{
		//IV.检测百分比的变化
		UE_LOG(ReInstallationLog, Display, TEXT("4. Delete temporary package."));
		while (!FMath::IsNearlyEqual(InProgressInstallationPercentage, 1.f, 0.001f) && !IsEngineExitRequested())
		{
			UE_LOG(ReInstallationLog, Display, TEXT("Start waiting for program progress...[If the program is stuck here for a long time, it may be occupied]"));
			UE_LOG(ReInstallationLog, Display, TEXT("InProgressInstallationPercentage = %f"),InProgressInstallationPercentage);
		
			FPlatformProcess::Sleep(0.5f);
		}
	}
}

void FReInstallationManage::DeleteResourceFile(const TArray<FString>& InFoundFiles)
{
	for (auto& Tmp : InFoundFiles)
	{
		DeleteFile(*Tmp);
	}
}

void FReInstallationManage::HandleDescribe(
	const FString& InProjectToContentPaks,
	const FString& InProjectRootPath,
	const TArray<FRemoteDataDescribe>& InDiscardInfo)
{
	for (const auto& Tmp : InDiscardInfo)
	{
		if (!Tmp.Link.IsEmpty())
		{
			FString NewPath;
			if (Tmp.InstallationPath.IsEmpty())
			{
				NewPath = InProjectToContentPaks;
			}
			else
			{
				NewPath = InProjectRootPath + Tmp.InstallationPath;
			}

			FString Head = FPaths::GetCleanFilename(Tmp.Link);
			NewPath /= Head;

			DeleteFile(*NewPath);
		}
	}
}

void FReInstallationManage::DeleteFile(const TCHAR* NewFilename)
{
	if (IFileManager::Get().Delete(NewFilename))
	{
		UE_LOG(ReInstallationLog, Display, TEXT("Delete = [%s] Success."), NewFilename);
	}
	else
	{
		UE_LOG(ReInstallationLog, Error, TEXT("Delete = [%s] Fail."), NewFilename);
	}
}

#undef LOCTEXT_NAMESPACE