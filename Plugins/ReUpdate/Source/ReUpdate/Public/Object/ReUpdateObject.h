#pragma once

#include "CoreMinimal.h"
#include "Version/ReVersion.h"
#include "HTTP/ReUpdateHTTP.h"
#include "ReUpdateType.h"
#include "Tickable.h"
#include "ReUpdateObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FProgressBPDelegate, float ,InPercentage, const FString& ,InContent, int32 ,TotalBytes, int32 ,BytesReceived);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FVersionBPDelegate, EServerVersionResponseType,InPercentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectCompleteBPDelegate, const FString &,InName);

UCLASS(BlueprintType, Blueprintable)
class REUPDATE_API UVersionControlObject :public UObject, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//包的下载进度
	UPROPERTY(BlueprintAssignable)
	FProgressBPDelegate OnProgressDelegate;

	//所有包下载完毕后执行本次代理
	UPROPERTY(BlueprintAssignable)
	FVersionBPDelegate OnVersionDelegate;

	//每间隔线上服务器版本验证
	UPROPERTY(BlueprintAssignable)
	FVersionBPDelegate OnRealTimeVersionDetectionDelegate;

	//单个对象完成
	UPROPERTY(BlueprintAssignable)
	FObjectCompleteBPDelegate OnObjectCompleteBPDelegate;

public:
	UVersionControlObject();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "CreateVersionObject", Keywords = "CVO"), Category = "VersionControl")
	static UVersionControlObject* CreateObject(UClass* InClass = NULL, UObject* InParent = NULL);

	UFUNCTION(BlueprintCallable, meta = (Keywords = "IV"), Category = "Version")
	virtual bool InitVersion();
	
	UFUNCTION(BlueprintCallable, meta = (Keywords = "SSV"), Category = "Version")
	void SetServerVersion(const FVersionInfor& InServerVersion);

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GCV"), Category = "Version")
	bool GetCurrentVersion(FVersionInfor& OutVersion,bool bSynchronous = true);
	
	UFUNCTION(BlueprintCallable, meta = (Keywords = "UV"), Category = "Version")
	void UpdateVersion();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "RPP"), Category = "Version")
	void ResetProgramProgress();

public:
	virtual void Tick(float DeltaTime);
	virtual TStatId GetStatId() const;

protected:
	UFUNCTION(BlueprintCallable, meta = (Keywords = "IU"), Category = "Version")
	void IterativeUpdating();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GVV"), Category = "Version")
	EServerVersionResponseType GetValidationVersion();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GDPP"), Category = "Version")
	FString GetDownLoadPersistencePath();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GDPT"), Category = "Version")
	FString GetDownLoadPersistenceTmpPath();

protected:
	void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
	void OnRequestProgress(FHttpRequestPtr HttpRequest, int32 TotalBytes, int32 BytesReceived);
	void OnRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue);
	void OnRequestsComplete();

	void OnCompareServerVersions(const FVersionInfor &InNewVersion);

protected:
	UFUNCTION(BlueprintCallable, meta = (Keywords = "CIP"), Category = "Version")
	bool CallInstallationProgress();

	//执行全卸载安装
	UFUNCTION(BlueprintCallable, meta = (Keywords = "CIP"), Category = "Version")
	bool ExecuteInstallationProgressByUninstallAll();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "ULP"), Category = "Version")
	bool UnLoadAllPak();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "LAP"), Category = "Version")
	bool LoadAllPakCache();

public:
	UFUNCTION(BlueprintCallable, meta = (Keywords = "IVCO"), Category = "Version")
	void InitVersionControlObject();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "SCV"), Category = "Version")
	void SerializeClientVersion();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "DCV"), Category = "Version")
	void DeserializationClientVersion();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GVP"), Category = "Version")
	FString GetClientVersionPosition();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GCV"), Category = "Version")
	FString GetClientVersionName();

	UFUNCTION(BlueprintCallable, meta = (Keywords = "GCV"), Category = "Version")
	FString GetServerVersionName();

	UFUNCTION(BlueprintPure, Category = "Version")
	float GetProgressInstallationPercentage();

protected:
	FVersion ClientVersion;
	FVersion ServerVersion;

	//初始化
	bool bInitialization;

	//开启版本检测
	bool bStartVersionDetection;

	float VersionDetectionTime;

	bool bMajorVersion;

protected:
	TArray<FString> RelativePatchs;
	TArray<FString> PakFileCache;
};