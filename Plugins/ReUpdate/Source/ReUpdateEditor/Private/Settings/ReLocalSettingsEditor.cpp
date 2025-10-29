#include "Settings/ReLocalSettingsEditor.h"
#include "Settings/ReUnrealPakSettingsEditor.h"

#define LOCTEXT_NAMESPACE "UReLocalSettingsEditor"

UReLocalSettingsEditor::UReLocalSettingsEditor()
	//:bHttps(false)
{

}

#if WITH_EDITOR
void UReLocalSettingsEditor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("IPOrDomain") ||
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("BucketName") ||
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("LocalVersionPath") ||
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("ServerVersionName") ||
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("VersionConfig"))

	{
		BasedOnwhichVersion.FilePath = LocalVersionPath.Path / BucketName.ToString();

		ResetLinkURL();
	}
}

#endif
void UReLocalSettingsEditor::ResetLinkURL()
{
	IPOrDomain.RemoveFromEnd(TEXT("/"));
	IPOrDomain.RemoveFromEnd(TEXT("//"));
	IPOrDomain.RemoveFromEnd(TEXT("\\"));

	VersionConfig.LinkURL = (IPOrDomain / BucketName.ToString());

	UpdateStorageSettingsInstance();
}

FString UReLocalSettingsEditor::GetVersionPath() const
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	FString Platform = GetDefault<UReUnrealPakSettingsEditor>()->GetPlatform();

	FString RootPath = LocalVersionPath.Path / BucketName.ToString() / Platform;
	FString VersionPath = RootPath / VersionConfig.ServerVersionName.ToString();

	FString NewPath;
	if (BasedOnwhichVersion.FilePath.IsEmpty())
	{
		if (PlatformFile.FileExists(*VersionPath))
		{
			NewPath = VersionPath;
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::YesNo,
				LOCTEXT("VersionDiscardSettings_locationVersion",
					"There is no matching version in this location. Please regenerate a version."));
		}
	}
	else
	{
		if (BasedOnwhichVersion.FilePath != TEXT("NONE"))
		{
			//优先级最高
			NewPath = BasedOnwhichVersion.FilePath;
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::YesNo,
				LOCTEXT("VersionDiscardSettings_locationVersion2",
					"BasedOnwhichVersion != TEXT(\"NONE\"."));
		}
	}

	return NewPath;
}

#undef LOCTEXT_NAMESPACE