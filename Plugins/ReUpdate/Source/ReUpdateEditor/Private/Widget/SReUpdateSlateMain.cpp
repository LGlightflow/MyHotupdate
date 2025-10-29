#include "Widget/SReUpdateSlateMain.h"
#include "Settings/ObjectStorageServerSettingsEditor.h"
#include "Settings/ReHTTPServerSettingsEditor.h"
#include "Settings/ReOSSServerSettingsEditor.h"
#include "Settings/ReUnrealPakSettingsEditor.h"
#include "Settings/ReLocalSettingsEditor.h"
#include "Settings/ObjectStorageSettings.h"
#include "Widgets/Layout/SBox.h"
#include "PropertyEditorModule.h"
#include "Widgets/Layout/SScrollBox.h"
#include "ReUpdateEditor.h"
#include "Settings/ObjectStorageSettingsRuntime.h"

#define LOCTEXT_NAMESPACE "SReUpdateSlateMain"

void SReUpdateSlateMain::Construct(const FArguments& InArgs)
{
	FMenuBuilder PutServetModeBuilder(true, NULL);
	FUIAction HTTPServer(FExecuteAction::CreateSP(this,&SReUpdateSlateMain::HandlePutServetTypeByClicked, EObjectStorageServerType::HTTP_SERVER));
	PutServetModeBuilder.AddMenuEntry(LOCTEXT("HTTP_Server", "HTTPServer"), LOCTEXT("HTTP_ServerTip", "You can set HTTP server related content."), FSlateIcon(),HTTPServer);
	
	FUIAction OSSServer(FExecuteAction::CreateSP(this, &SReUpdateSlateMain::HandlePutServetTypeByClicked, EObjectStorageServerType::OSS_SERVER));
	PutServetModeBuilder.AddMenuEntry(LOCTEXT("OSS_SERVER", "OSSServer"), LOCTEXT("OSSServerTip", "Alibaba cloud based object storage server."), FSlateIcon(), OSSServer);
	
	FUIAction LocalVerison(FExecuteAction::CreateSP(this, &SReUpdateSlateMain::HandlePutServetTypeByClicked, EObjectStorageServerType::LOCAL_POS));
	PutServetModeBuilder.AddMenuEntry(LOCTEXT("LOCAL_VERISON", "Local"), LOCTEXT("LocalVerisonTip", "Store verison to the specified local location."), FSlateIcon(), LocalVerison);
	
	ChildSlot
	[
		SNew(SScrollBox)
		.Orientation(Orient_Vertical)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.Padding(4.f, 2.f, 4.f, 2.f)
				[
					SNew(SComboButton)
					.ButtonContent()
					[
						SNew(STextBlock)
						.Text(this, &SReUpdateSlateMain::PutServetModeBuilderText)
					]
					.MenuContent()
					[
						PutServetModeBuilder.MakeWidget()
					]
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.Padding(4.f, 2.f, 4.f, 2.f)
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("Get_Current_Version", "Current Version"))
					.HAlign(HAlign_Center)
					.OnClicked(this, &SReUpdateSlateMain::GetCurrentVersion)
					.ToolTipText(LOCTEXT("Get_Current_VersionTip", "Get current version."))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.Padding(4.f, 2.f, 4.f, 2.f)
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("Get_Unlock", "Unlock"))
					.HAlign(HAlign_Center)
					.OnClicked(this, &SReUpdateSlateMain::Unlock)
					.ToolTipText(LOCTEXT("UnlockTip", "Unlock server deadlock."))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.Padding(4.f, 2.f, 4.f, 2.f)
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("Get_Push_Version", "Push Version Info"))
					.HAlign(HAlign_Center)
					.OnClicked(this, &SReUpdateSlateMain::PushVersion)
					.IsEnabled(this, &SReUpdateSlateMain::IsPushVersion)
					.ToolTipText(LOCTEXT("Get_Push_VersionTip", "Only upload version information, excluding resources. It is generally set for discarding resources."))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.Padding(4.f, 2.f, 4.f, 2.f)
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("Save_as_Default", "Save as Default"))
					.HAlign(HAlign_Center)
					.OnClicked(this, &SReUpdateSlateMain::SaveAsDefault)
					.ToolTipText(LOCTEXT("Save_as_DefaultTip", "Store configuration locally."))
				]

				+ SHorizontalBox::Slot()
				.HAlign(EHorizontalAlignment::HAlign_Right)
				.Padding(4.f, 2.f, 4.f, 2.f)
				.AutoWidth()
				[
					SNew(SButton)
					.Text(LOCTEXT("Upload_Custom_File", "Push To Server"))
					.HAlign(HAlign_Center)
					.IsEnabled(this, &SReUpdateSlateMain::IsPutToServer)
					.OnClicked(this, &SReUpdateSlateMain::PutToServer)
					.ToolTipText(LOCTEXT("Upload_Custom_File_DefaultTip", "As a hot update upload button, it can be displayed correctly as long as the configuration is correct."))
				]
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				SAssignNew(ObjectContent, SBox)
			]
		]
	];

	InitView();
	InitConfig();

	SaveConfig();
}

template<class T>
void SaveObjectConfig()
{
	if (T* InSettingsEditor = const_cast<T*>(GetDefault<T>()))
	{
		InSettingsEditor->SaveObjectStorageConfig();
	}
}

template<class T>
void LoadObjectConfig()
{
	if (T* InSettingsEditor = const_cast<T*>(GetDefault<T>()))
	{
		InSettingsEditor->LoadObjectStorageConfig();
	}
}

bool SReUpdateSlateMain::IsPutToServer()const
{
	if (const UObjectStorageSettings *InObjectStorageSettings = GetStorageSettings())
	{
		if (InObjectStorageSettings->VersionConfig.CustomExtraInstallationInfo.Num() > 0)
		{
			return !InObjectStorageSettings->VersionConfig.CustomExtraInstallationInfo[0].FilePath.FilePath.IsEmpty();
		}
		else if(InObjectStorageSettings->VersionConfig.MainVersionInstallationInfo.Num() > 0)
		{
			return !InObjectStorageSettings->VersionConfig.MainVersionInstallationInfo[0].FilePath.FilePath.IsEmpty();
		}
	}

	return false;
}

bool SReUpdateSlateMain::IsPushVersion() const
{
	return GetDefault<UReUnrealPakSettingsEditor>()->VersionControlObject != NULL && 
		GetDefault<UReUnrealPakSettingsEditor>()->VersionDiscardSettings.Num() > 0;
}

FReply SReUpdateSlateMain::PutToServer()
{
	if (IsPutToServer())
	{
		if (FReUpdateEditorModule* InEditorModule = &FModuleManager::LoadModuleChecked<FReUpdateEditorModule>(TEXT("ReUpdateEditor")))
		{
			InEditorModule->Push(
				GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType,
				GetStorageSettings());
		}
	}

	return FReply::Handled();
}

FReply SReUpdateSlateMain::PushVersion()
{
	if (UReUnrealPakSettingsEditor* InUnrealPakSettingsEditor = const_cast<UReUnrealPakSettingsEditor*>(GetDefault<UReUnrealPakSettingsEditor>()))
	{
		InUnrealPakSettingsEditor->UploadDiscardedVersion();
	}

	return FReply::Handled();
}

FReply SReUpdateSlateMain::GetCurrentVersion()
{
	if (UReUnrealPakSettingsEditor* InUnrealPakSettingsEditor = const_cast<UReUnrealPakSettingsEditor*>(GetDefault<UReUnrealPakSettingsEditor>()))
	{
		InUnrealPakSettingsEditor->BuildVersionToDiscard();
	}

	return FReply::Handled();
}

FReply SReUpdateSlateMain::SaveAsDefault()
{
	if (FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PreSaveAsDefaultUserData", 
		"Are you sure you want to store the above data in the local configuration?")) != EAppReturnType::Yes)
	{
		return FReply::Unhandled();
	}

	SaveConfig();

	FMessageDialog::Open(EAppMsgType::YesNo, LOCTEXT("PostSaveAsDefaultUserData",
		"The data of the object has been stored."));

	return FReply::Handled();
}

void SReUpdateSlateMain::InitConfig()
{
	LoadObjectConfig<UReUnrealPakSettingsEditor>();

	LoadObjectConfig<UReHTTPServerSettingsEditor>();
	LoadObjectConfig<UReOSSServerSettingsEditor>();
	LoadObjectConfig<UReLocalSettingsEditor>();
	LoadObjectConfig<UObjectStorageServerSettingsEditor>();
	LoadObjectConfig<UObjectStorageSettings>();
	LoadObjectConfig<UObjectStorageSettingsRuntime>();
}

FReply SReUpdateSlateMain::Unlock()
{
	if (FReUpdateEditorModule* InEditorModule = &FModuleManager::LoadModuleChecked<FReUpdateEditorModule>(TEXT("ReUpdateEditor")))
	{
		InEditorModule->Unlock(GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType);
	}

	return FReply::Handled();
}

void SReUpdateSlateMain::SaveConfig()
{
	switch (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType)
	{
	case EObjectStorageServerType::HTTP_SERVER:
	{
		SaveObjectConfig<UReHTTPServerSettingsEditor>();
		if (UReHTTPServerSettingsEditor* InSettingsEditor = const_cast<UReHTTPServerSettingsEditor*>(GetDefault<UReHTTPServerSettingsEditor>()))
		{
			InSettingsEditor->ResetLinkURL();
		}
		break;
	}
	case EObjectStorageServerType::OSS_SERVER:
	{
		SaveObjectConfig<UReOSSServerSettingsEditor>();
		if (UReOSSServerSettingsEditor* InSettingsEditor = const_cast<UReOSSServerSettingsEditor*>(GetDefault<UReOSSServerSettingsEditor>()))
		{
			InSettingsEditor->ResetLinkURL();
		}
		break;
	}case EObjectStorageServerType::LOCAL_POS:
	{
		SaveObjectConfig<UReLocalSettingsEditor>();
		if (UReLocalSettingsEditor* InSettingsEditor = const_cast<UReLocalSettingsEditor*>(GetDefault<UReLocalSettingsEditor>()))
		{
			InSettingsEditor->ResetLinkURL();
		}
		break;
	}
	}

	SaveObjectConfig<UReUnrealPakSettingsEditor>();
	SaveObjectConfig<UObjectStorageServerSettingsEditor>();
}

FText SReUpdateSlateMain::PutServetModeBuilderText() const
{
	if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::HTTP_SERVER)
	{
		return LOCTEXT("HTTP_Server", "HTTPServer");
	}
	else if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::OSS_SERVER)
	{
		return LOCTEXT("OSS_SERVER", "OSSServer");
	}
	else if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
	{
		return LOCTEXT("LOCAL_POS", "Local");
	}

	return LOCTEXT("Server_None", "Server_None");
}

void SReUpdateSlateMain::InitView()
{
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked< FPropertyEditorModule >("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	{
		DetailsViewArgs.bAllowMultipleTopLevelObjects = true;
		DetailsViewArgs.bShowPropertyMatrixButton = false;
	}
	TSharedPtr<class IDetailsView> ConfigPanel = PropertyModule.CreateDetailView(DetailsViewArgs);

	TArray<UObject*> SourceObjects;
	SourceObjects.Reserve(2);
	if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::HTTP_SERVER)
	{
		SourceObjects.Add(const_cast<UReHTTPServerSettingsEditor*>(GetDefault<UReHTTPServerSettingsEditor>()));
	}
	else if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::OSS_SERVER)
	{
		SourceObjects.Add(const_cast<UReOSSServerSettingsEditor*>(GetDefault<UReOSSServerSettingsEditor>()));
	}
	else if (GetDefault<UObjectStorageServerSettingsEditor>()->ObjectStorageServerType == EObjectStorageServerType::LOCAL_POS)
	{
		SourceObjects.Add(const_cast<UReLocalSettingsEditor*>(GetDefault<UReLocalSettingsEditor>()));
	}

	SourceObjects.Add(const_cast<UReUnrealPakSettingsEditor*>(GetDefault<UReUnrealPakSettingsEditor>()));

	ConfigPanel->SetObjects(SourceObjects);
	ObjectContent->SetContent(ConfigPanel.ToSharedRef());
}

void SReUpdateSlateMain::HandlePutServetTypeByClicked(EObjectStorageServerType InOSType)
{
	if (const UObjectStorageServerSettingsEditor* ServerSettingsEditorConst = GetDefault<UObjectStorageServerSettingsEditor>())
	{
		UObjectStorageServerSettingsEditor* ServerSettingsEditor = const_cast<UObjectStorageServerSettingsEditor*>(ServerSettingsEditorConst);
		ServerSettingsEditor->ObjectStorageServerType = InOSType;

		InitView();

		SaveConfig();
	}
}

#undef LOCTEXT_NAMESPACE