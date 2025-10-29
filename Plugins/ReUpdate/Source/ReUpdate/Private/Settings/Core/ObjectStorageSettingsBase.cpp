#include "Settings/Core/ObjectStorageSettingsBase.h"
#include "Log/ReUpdateLog.h"

void UObjectStorageSettingsBase::SaveObjectStorageConfig()
{
	FString SettingsEditorConfigPath = FPaths::ProjectConfigDir() / TEXT("Default") + GetClass()->ClassConfigName.ToString() + TEXT(".ini");
	
	SaveConfig(CPF_Config, *SettingsEditorConfigPath);

	UE_LOG(LogReVersionControl, Display, TEXT("SettingsEditorConfigPath=%s"), *SettingsEditorConfigPath);
}

void UObjectStorageSettingsBase::LoadObjectStorageConfig()
{
	FString SettingsEditorConfigPath = FPaths::ProjectConfigDir() / TEXT("Default") + GetClass()->ClassConfigName.ToString() + TEXT(".ini");
	
	LoadConfig(GetClass(), *SettingsEditorConfigPath);

	UE_LOG(LogReVersionControl, Display, TEXT("SettingsEditorConfigPath=%s"), *SettingsEditorConfigPath);
}
