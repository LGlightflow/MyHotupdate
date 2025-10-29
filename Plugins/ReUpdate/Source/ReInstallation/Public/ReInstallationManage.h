#pragma once

#include "CoreMinimal.h"
#include "ReInstallationLog.h"
#include "ReInstallationType.h"
#include "HAL/FileManager.h"

struct FRemoteDataDescribe;

class REINSTALLATION_API FReInstallationManage
{
public:
	FReInstallationManage();
	virtual ~FReInstallationManage();

public:
	template<class T>
	bool HandleHotInstallation(
		const FString& InResourcesToCopied,
		const FString& InResourcesToCopiedCustom,
		const FString& InProjectToContentPaks,
		const FString& InProjectRootPath,
		const TArray<FString>& InRelativePatchs,
		const TArray<FRemoteDataDescribe>& InDiscardInfo,
		float& OutPakNumber,
		float& OutCustomPakNumber,
		float& OutProgressInstallationPercentage);

public:
	//收集资源
	void CollectFiles(
		const FString& InResourcesToCopied,
		TArray<FString>& OutFoundFiles);

	void CollectFilesCustom(
		const FString& InResourcesToCopiedCustom,
		const TArray<FString>& InRelativePatchs,
		TArray<FString>& OutCustomFoundFiles,
		TArray<FReInstallationCustomCopyPath>& OutCustomCopyPaths);

	template<class T>
	void ResourceCopy(const TArray<FString>& InFoundFiles,
		const FString& InProjectToContentPaks,
		TArray<T*>& OutInstallationProgressArray);

	template<class T>
	void ResourceCopyCustom(
		const TArray<FReInstallationCustomCopyPath>& InCustomCopyPaths,
		TArray<T*>& OutCustomCopyPaths);

	void Wait(float& InProgressInstallationPercentage, int32 InPakNumber, int32 InCustomPakNumber);

	void DeleteResourceFile(const TArray<FString>& InFoundFiles);

	void HandleDescribe(
		const FString& InProjectToContentPaks,
		const FString& InProjectRootPath,
		const TArray<FRemoteDataDescribe>& InDiscardInfo);

	template<class T>
	void DeleteProgress(const TArray<T*>& InProgress);

protected:
	void DeleteFile(const TCHAR* NewFilename);
};

template<class T>
inline void FReInstallationManage::ResourceCopy(const TArray<FString>& InFoundFiles, const FString& InProjectToContentPaks, TArray<T*>& OutInstallationProgressArray)
{
	//资源拷贝
	UE_LOG(ReInstallationLog, Display, TEXT("Resource copy."));

	for (auto& Tmp : InFoundFiles)
	{
		UE_LOG(ReInstallationLog, Display, TEXT("Resource = [%s]"), *Tmp);

		FString TargetPath = InProjectToContentPaks / FPaths::GetCleanFilename(Tmp);
		T* InstallationProgress = new T();
		IFileManager::Get().Copy(*TargetPath, *Tmp, true, false, false, InstallationProgress);

		//作为后面回收
		OutInstallationProgressArray.Add(InstallationProgress);
	}
}

template<class T>
inline void FReInstallationManage::ResourceCopyCustom(const TArray<FReInstallationCustomCopyPath>& InCustomCopyPaths, TArray<T*>& OutCustomCopyPaths)
{
	UE_LOG(ReInstallationLog, Display, TEXT("Custom Resource copy."));
	for (auto& Tmp : InCustomCopyPaths)
	{
		UE_LOG(ReInstallationLog, Display, TEXT("From [%s] => [%s]"), *Tmp.From, *Tmp.To);

		T* InstallationProgress = new T();
		IFileManager::Get().Copy(*Tmp.To, *Tmp.From, true, false, false, InstallationProgress);

		OutCustomCopyPaths.Add(InstallationProgress);
	}
}

template<class T>
inline void FReInstallationManage::DeleteProgress(const TArray<T*>& InProgress)
{
	for (const auto& Tmp : InProgress)
	{
		delete Tmp;
	}
}

template<class T>
inline bool FReInstallationManage::HandleHotInstallation(
	const FString& InResourcesToCopied,
	const FString& InResourcesToCopiedCustom,
	const FString& InProjectToContentPaks,
	const FString& InProjectRootPath,
	const TArray<FString>& InRelativePatchs,
	const TArray<FRemoteDataDescribe>& InDiscardInfo,
	float& OutPakNumber,
	float& OutCustomPakNumber,
	float& OutProgressInstallationPercentage)
{
	//II.资源遍历
	UE_LOG(ReInstallationLog, Display, TEXT("2. Resource traversal."));

	TArray<FString> FoundFiles;
	CollectFiles(InResourcesToCopied, FoundFiles);

	//收集自定义拷贝路径
	TArray<FReInstallationCustomCopyPath> CustomCopyPaths;
	TArray<FString> CustomFoundFiles;
	CollectFilesCustom(
		InResourcesToCopiedCustom,
		InRelativePatchs,
		CustomFoundFiles,
		CustomCopyPaths);

	OutPakNumber = FoundFiles.Num();
	OutCustomPakNumber = CustomCopyPaths.Num();

	TArray<T*> InstallationProgressArray;

	ResourceCopy(FoundFiles, InProjectToContentPaks, InstallationProgressArray);

	UE_LOG(ReInstallationLog, Display, TEXT("3.Custom Resource copy."));
	ResourceCopyCustom(CustomCopyPaths, InstallationProgressArray);

	//等待处理 等待异步处理完成
	Wait(OutProgressInstallationPercentage, OutPakNumber, OutCustomPakNumber);

	if (IsEngineExitRequested())
	{
		UE_LOG(ReInstallationLog, Display, TEXT("5. Program forced exit."));
		return false;
	}

	//V.删除临时包
	UE_LOG(ReInstallationLog, Display, TEXT("5. Delete temporary package."));

	DeleteResourceFile(FoundFiles);
	DeleteResourceFile(CustomFoundFiles);

	//V.删除丢弃的包
	HandleDescribe(InProjectToContentPaks, InProjectRootPath, InDiscardInfo);

	//删除进度
	DeleteProgress(InstallationProgressArray);

	return true;
}
