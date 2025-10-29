
#pragma once

#include "CoreMinimal.h"
#include "ReUpdateEditorType.h"
//#include "ReUpdateType.h"
#include "Settings/Core/ObjectStorageSettingsBase.h"
#include "ObjectStorageServerSettingsEditor.generated.h"

enum class EObjectStorageServerType :uint8;

UCLASS(config = ObjectStorageServerSettingsEditor)
class UObjectStorageServerSettingsEditor : public UObjectStorageSettingsBase
{
	GENERATED_BODY()

public:
	UObjectStorageServerSettingsEditor();

	UPROPERTY(config, EditAnywhere, Category = ObjectStorageServerSettings, meta = (ToolTip = "Hot update"))
	EObjectStorageServerType ObjectStorageServerType;
	
	UPROPERTY(config, EditAnywhere, Category = ObjectStorageServerSettings, meta = (ToolTip = "Hot update"))
	bool bClearPak;
};