#pragma once

#include "CoreMinimal.h"
#include "Settings/ObjectStorageSettings.h"
#include "ReHTTPServerSettingsEditor.generated.h"

//HTTP服务器设置
UCLASS(config = ReHTTPServerSettingsEditor)
class UReHTTPServerSettingsEditor : public UObjectStorageSettings
{
	GENERATED_BODY()

public:
	UReHTTPServerSettingsEditor();

	//TODO:Account Password
	UPROPERTY(config, EditAnywhere, Category = HTTPServerSettings, meta = (ToolTip = "Hot update"))
	FString PublicIP;

	UPROPERTY(config, EditAnywhere, Category = HTTPServerSettings, meta = (ToolTip = "Hot update"))
	FString LANIP;

	UPROPERTY(config, EditAnywhere, Category = HTTPServerSettings, meta = (ToolTip = "Specify the address to upload."))
	FName ObjectStorageName;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent);
#endif
	virtual void ResetLinkURL();
};