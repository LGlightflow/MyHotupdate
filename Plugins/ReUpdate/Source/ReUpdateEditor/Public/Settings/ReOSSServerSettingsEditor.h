#pragma once

#include "CoreMinimal.h"
#include "Settings/ObjectStorageSettings.h"
#include "ReOSSServerSettingsEditor.generated.h"

// OSS服务器配置
UCLASS(config = ReOSSServerSettingsEditor)
class UReOSSServerSettingsEditor : public UObjectStorageSettings
{
	GENERATED_BODY()

public:
	UReOSSServerSettingsEditor();
	//OSS访问者的ID
	UPROPERTY(config, EditAnywhere, Category = OSSServerSettings, meta = (ToolTip = "Hot update"))
	FString AccessKeyId;

	// OSS访问者的密钥
	UPROPERTY(config, EditAnywhere, Category = OSSServerSettings, meta = (ToolTip = "Hot update"))
	FString AccessKeySecret;

	//当前OSS的Endpoint
	UPROPERTY(config, EditAnywhere, Category = OSSServerSettings, meta = (ToolTip = "Hot update"))
	FString Endpoint;

	//OSS存储桶名称，也是热更新位置
	UPROPERTY(config, EditAnywhere, Category = OSSServerSettings, meta = (ToolTip = "Specify the address to upload."))
	FName BucketName;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif

	virtual void ResetLinkURL();
};