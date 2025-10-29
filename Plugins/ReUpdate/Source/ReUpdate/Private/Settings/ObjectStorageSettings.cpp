#include "Settings/ObjectStorageSettings.h"
#include "Settings/ObjectStorageSettingsRuntime.h"

UObjectStorageSettings::UObjectStorageSettings()
{
	
}

void UObjectStorageSettings::UpdateStorageSettingsInstance()
{
	if (UObjectStorageSettingsRuntime *InSettingsRuntime = GetMutableDefault<UObjectStorageSettingsRuntime>())
	{
		InSettingsRuntime->VersionConfig = VersionConfig;
		InSettingsRuntime->SaveObjectStorageConfig();
	}

	SaveObjectStorageConfig();
}
