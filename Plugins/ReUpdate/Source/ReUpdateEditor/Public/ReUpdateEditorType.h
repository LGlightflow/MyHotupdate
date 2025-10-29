#pragma once

#include "CoreMinimal.h"
#include "Settings/ObjectStorageServerSettingsEditor.h"
#include "Settings/ReHTTPServerSettingsEditor.h"
#include "Settings/ReOSSServerSettingsEditor.h"
#include "Settings/ReLocalSettingsEditor.h"
#include "ReUpdateType.h"


enum class ESimpleSubMenuType :uint8
{
	HU_CONTENT,
	HU_ASSET
};

const UObjectStorageSettings* GetStorageSettings();
