#include "ReUpdateType.h"

FCustomVersionInstallation::FCustomVersionInstallation()
{
	bInstallContentPak = false;
}

FObjectVersionConfig::FObjectVersionConfig()
	:PatchVersionLogName("Version/VersionLog.con")
	, ServerVersionName("Version/ServerVersion.con")
	, VersionLock("Version/MutexVersion.con")
{
	bHttps = true;
	bSeamlessHotUpdate = true;
	bStartVersionDetection = true;
	PerSecondVersionDetection = 1.f;

	bInstallContentPak = false;

	PakPlatform = TEXT("Windows");
}