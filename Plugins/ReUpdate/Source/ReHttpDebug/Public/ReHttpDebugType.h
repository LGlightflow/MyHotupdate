#pragma once

#include "CoreMinimal.h"
#include "ReHttpDebugType.generated.h"

enum class EHTTPObjectStorageProtocol :uint8
{
	HSP_INIT,//初始化服务器的数据
	HSP_CHECK_LOCK,//检测锁
	HSP_LOCK,//上锁
	HSP_UNLOCK,//解锁
};

USTRUCT(BlueprintType)
struct ReHttpDEBUG_API FHotHttpDebugFileInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString FileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString VersionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString Bucket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString PatchVersionLogName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString ServerVersionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString VersionLock;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString Platform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString PakLocalPaths;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString FilePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString Endpoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HotHttpDebugFileInfo")
	FString InstallationPath;
};