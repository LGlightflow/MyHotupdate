#include "Settings/ReOSSServerSettingsEditor.h"

UReOSSServerSettingsEditor::UReOSSServerSettingsEditor()
{

}
#if WITH_EDITOR
void UReOSSServerSettingsEditor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName().ToString() == TEXT("Endpoint") || 
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("BucketName") || 
		PropertyChangedEvent.GetPropertyName().ToString() == TEXT("VersionConfig"))
	{
		ResetLinkURL();
	}
}
#endif

void UReOSSServerSettingsEditor::ResetLinkURL()
{
	VersionConfig.LinkURL = (BucketName.ToString() + TEXT(".") + Endpoint);
	UpdateStorageSettingsInstance();
}