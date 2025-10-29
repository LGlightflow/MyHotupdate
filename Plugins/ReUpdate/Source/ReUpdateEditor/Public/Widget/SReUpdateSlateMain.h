#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "ReUpdateEditorType.h"

class SReUpdateSlateMain :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SReUpdateSlateMain)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	FText PutServetModeBuilderText() const;

	void HandlePutServetTypeByClicked(EObjectStorageServerType InOSType);

	void InitView();
	void InitConfig();
	FReply Unlock();
	FReply GetCurrentVersion();
	FReply PushVersion();
	FReply SaveAsDefault();
	FReply PutToServer();
	bool IsPutToServer()const;
	bool IsPushVersion()const;
	void SaveConfig();
private:
	TSharedPtr<class SBox> ObjectContent;
};