// Copyright (C) RenZhai.2020.All Rights Reserved.
#include "OSSObjectStorageClientMethod.h"
#include "SimpleOSSManage.h"
//#include "Version/SimpleVersion.h"

namespace OSSObjectStorageClientMethod
{
	EOSSLockType IsLockOSSServer(const FString& InBucketName, const FString& InVersionLockPath, const FString& InPlatform)
	{
		FString LockBuffer = TEXT("False");
		if (SIMPLE_OSS.GetObjectToMemory(InBucketName, InPlatform / InVersionLockPath, LockBuffer))
		{
			return LockBuffer.ToBool() == true ? EOSSLockType::OSS_TRUE : EOSSLockType::OSS_FALSE;
		}
		else if (SIMPLE_OSS.PutObjectByMemory(InBucketName, InPlatform / InVersionLockPath, LockBuffer))
		{
			return EOSSLockType::OSS_FALSE;
		}
		else
		{
			FGraphEventRef EventRef = FFunctionGraphTask::CreateAndDispatchWhenReady([]()
			{
				FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("FSimpleVersionControlEditorModule", "Error_GetObjectToServer", "Unable to obtain lock, wrong OSS password or account, unable to access OSS server ."));
			},
			TStatId(),
			NULL,
			ENamedThreads::GameThread);
		}

		return EOSSLockType::OSS_CONNECTION_FAILED;
	}

	bool GetServerVersion(const FString& InBucketName, const FString& InServerVersionName, const FString& Platform, FVersion& InVersion)
	{
		FString JsonData;
		if (SIMPLE_OSS.GetObjectToMemory(InBucketName, Platform / InServerVersionName, JsonData))
		{
			return SimpleVersionControl::Read(JsonData, InVersion);
		}

		return false;
	}

	bool PutServerVersion(const FString& InBucketName, const FString& InServerVersionName, const FString& Platform, const FVersion& InVersion)
	{
		if (InVersion.Content.Num())
		{
			FString JsonString;
			SimpleVersionControl::Save(InVersion, JsonString);

			return PutServerVersion(InBucketName, InServerVersionName, Platform, JsonString);
		}
		
		return false;
	}

	bool PutServerVersion(const FString& InBucketName, const FString& InServerVersionName, const FString& Platform, const FString& InVersion)
	{
		return SIMPLE_OSS.PutObjectByMemory(InBucketName, Platform / InServerVersionName, InVersion);
	}

	FOSSLock::FOSSLock(const FString& InBucketName, const FString& InVersionLockPath, const FString& InPlatform)
	{
		BucketName = InBucketName;
		VersionLockPath = InVersionLockPath;
		Platform = InPlatform;

		EOSSLockType OSSType = IsLockOSSServer(InBucketName, InVersionLockPath, InPlatform);
		if (OSSType == EOSSLockType::OSS_FALSE)
		{
			FString LockBuffer = TEXT("True");
			if (SIMPLE_OSS.PutObjectByMemory(BucketName, Platform / VersionLockPath, LockBuffer))
			{

			}
		}
	}

	FOSSLock::~FOSSLock()
	{
		EOSSLockType OSSType = IsLockOSSServer(BucketName, VersionLockPath,Platform);
		if (OSSType == EOSSLockType::OSS_TRUE)
		{
			FString LockBuffer = TEXT("False");
			if (SIMPLE_OSS.PutObjectByMemory(BucketName, Platform / VersionLockPath, LockBuffer))
			{

			}
		}
	}
}