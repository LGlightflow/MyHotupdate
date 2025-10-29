#pragma once

#include "CoreMinimal.h"
#include "Core/ObjectStorageSettingsBase.h"
#include "ReUpdateType.h"
#include "ObjectStorageSettingsRuntime.generated.h"

UCLASS(config = ObjectStorageSettingsRuntime)
class REUPDATE_API UObjectStorageSettingsRuntime : public UObjectStorageSettingsBase
{
	GENERATED_BODY()

public:
	UObjectStorageSettingsRuntime();

	UPROPERTY(config, EditAnywhere, Category = GeneralSettings, meta = (ToolTip = "Version Config."))
	FObjectVersionConfig VersionConfig;
};