#pragma once

#include "CoreMinimal.h"
#include "Core/ObjectStorageSettingsBase.h"
#include "ReUpdateType.h"
#include "ObjectStorageSettings.generated.h"

UCLASS(config = ObjectStorageSettings)
class REUPDATE_API UObjectStorageSettings : public UObjectStorageSettingsBase
{
	GENERATED_BODY()

public:
	UObjectStorageSettings();

	UPROPERTY(config, EditAnywhere, Category = GeneralSettings, meta = (ToolTip = "Version Config."))
	FObjectVersionConfig VersionConfig;

protected:
	void UpdateStorageSettingsInstance();
};