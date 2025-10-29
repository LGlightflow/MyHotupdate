#pragma once

#include "CoreMinimal.h"
#include "ObjectStorageSettingsBase.generated.h"

UCLASS(config = ObjectStorageSettingsBase)
class REUPDATE_API UObjectStorageSettingsBase : public UObject
{
	GENERATED_BODY()

public:
	UObjectStorageSettingsBase(){}

public:
	//保存配置文件
	virtual void SaveObjectStorageConfig();
	//加载配置文件
	virtual void LoadObjectStorageConfig();

	virtual void ResetLinkURL(){}
};