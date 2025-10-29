#pragma once

#include "CoreMinimal.h"
#ifdef REUPDATE_OPEN_ENGINE_MODULE
#include "Engine/EngineTypes.h"
#endif
#include "ReUpdateType.generated.h"

#ifndef REUPDATE_OPEN_ENGINE_MODULE
struct FFilePath;
#endif


UENUM(BlueprintType)
enum class EObjectStorageServerType :uint8
{
	HTTP_SERVER,
	OSS_SERVER,
	LOCAL_POS,
};

UENUM(BlueprintType)
enum class EServerVersionResponseType :uint8
{
	VERSION_DIFF			UMETA(DisplayName = "Version Diff"),//更新补丁版本
	CRC_DIFF				UMETA(DisplayName = "Crc Diff"),//热更新
	EQUAL					UMETA(DisplayName = "Equal"),
	CONNECTION_ERROR		UMETA(DisplayName = "Connection Error"),
	MAX						UMETA(DisplayName = "Max"),
};

USTRUCT(BlueprintType)
struct REUPDATE_API FCustomVersionInstallation
{
	GENERATED_USTRUCT_BODY()

	FCustomVersionInstallation();
	// 主版本的pak
	UPROPERTY(EditAnywhere, Category = CustomInstallation)
	FFilePath FilePath;
	// 是否自动安装到未来打包好的项目下Content/Paks里面
	UPROPERTY(EditAnywhere, Category = CustomInstallation, meta = (ToolTip = "Specify the address to upload."))
	bool bInstallContentPak;
	//bInstallContentPak=false则安装到对应路径，否则还是安装到Content/Paks下面
	UPROPERTY(EditAnywhere, Category = CustomInstallation, meta = (ToolTip = "Specify the address to upload.", EditCondition = "!bInstallContentPak"))
	FString RelativeInstallationLocation;
};


USTRUCT(BlueprintType)
struct REUPDATE_API FObjectVersionConfig
{
	GENERATED_USTRUCT_BODY()

	FObjectVersionConfig();
	// 热更新服务器文件路径
	// 
	// 版本log的打印位置
	UPROPERTY(config, EditAnywhere, Category = GeneralSettings, meta = (ToolTip = "Specify the address to upload."))
	FName PatchVersionLogName;

	//服务器版本的位置
	UPROPERTY(config, EditAnywhere, Category = GeneralSettings, meta = (ToolTip = "Specify the address to upload."))
	FName ServerVersionName;

	//版本锁的位置
	UPROPERTY(config, EditAnywhere, Category = GeneralSettings, meta = (ToolTip = "Specify the address to upload."))
	FName VersionLock;

	UPROPERTY(config,VisibleDefaultsOnly, Category = GeneralSettings, meta = (ToolTip = "Specify the address to upload."))
	FString LinkURL;

	UPROPERTY(config, EditAnywhere, Category = GeneralSettings, meta = (ToolTip = "Hot update"))
	bool bHttps;
	//是否自动安装到未来打包好的项目下Content/Paks里面，
	// 如果为true，客户端下载后pak补丁将自动部署到Content/Paks。
	// true 的用处：随着引擎启动，自动加载pak，无需手动安装。
	UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "Specify the address to upload."))
	bool bInstallContentPak;

	//无缝热更新 适合全平台 如果bSeamlessHotUpdate=false 只适合windows
	UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "Seamless update, no need for third-party programs to pull up, internal operation, no perception of updates."))
	bool bSeamlessHotUpdate;

	//版本检测
	UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "Start version detection."))
	bool bStartVersionDetection;

	//How many seconds does version detection occur
	UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "How many seconds does version detection occur.", EditCondition = "bStartVersionDetection"))
	float PerSecondVersionDetection;

	UPROPERTY(config, VisibleDefaultsOnly, Category = PakSettings, meta = (ToolTip = "What is the chosen platform"))
	FString PakPlatform;

	/*项目路径 + 自定义的路径*/
	// 如果填/xx/xxx,客户端下载将会在自己的项目根目录自动生成一个 Project/xx/xxx文件路径，
	// 所有的补丁将会自动下载到里面，放在这里面的pak不会自动部署到游戏中，需要自行安装
	// 如果是空，会默认安装到Content/Paks里面。
	UPROPERTY(config, EditAnywhere, Category = PakSettings, meta = (ToolTip = "Specify the address to upload.", EditCondition = "!bInstallContentPak"))
	FString RelativeInstallationLocation;

	// 主版本信息
	UPROPERTY(config, EditAnywhere, Category = CustomPakSettings, meta = (ToolTip = "Specify the address to upload."))
	TArray<FCustomVersionInstallation> MainVersionInstallationInfo;

	UPROPERTY(config, EditAnywhere, Category = CustomPakSettings, meta = (ToolTip = "Specify the address to upload."))
	TArray<FCustomVersionInstallation> CustomExtraInstallationInfo;
};