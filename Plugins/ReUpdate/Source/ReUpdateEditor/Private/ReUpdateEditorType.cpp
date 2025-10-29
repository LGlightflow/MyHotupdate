#include "ReUpdateEditorType.h"

const UObjectStorageSettings* GetStorageSettings()
{
	switch (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType)
	{
		case EObjectStorageServerType::HTTP_SERVER:
		{
			return GetDefault<UReHTTPServerSettingsEditor>();
		}
		case EObjectStorageServerType::OSS_SERVER:
		{
			return GetDefault<UReOSSServerSettingsEditor>();
		}
		case EObjectStorageServerType::LOCAL_POS:
		{
			return GetDefault<UReLocalSettingsEditor>();
		}
	}

	return NULL;
}
