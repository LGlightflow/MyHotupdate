#include "Settings/ReHTTPServerSettingsEditor.h"
#include "Settings/ObjectStorageSettings.h"

UReHTTPServerSettingsEditor::UReHTTPServerSettingsEditor()
{
	
}

#if WITH_EDITOR
void UReHTTPServerSettingsEditor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("PublicIP") || 
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("LANIP") || 
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("VersionConfig"))
	{
		ResetLinkURL();
	}
}
#endif

void UReHTTPServerSettingsEditor::ResetLinkURL()
{
	VersionConfig.LinkURL = (PublicIP / ObjectStorageName.ToString());

	UpdateStorageSettingsInstance();
}