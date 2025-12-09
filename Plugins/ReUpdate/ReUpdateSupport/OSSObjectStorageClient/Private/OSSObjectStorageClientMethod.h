

#pragma once

#include "CoreMinimal.h"
#include "OSSObjectStorageClientType.h"
#include "Version/ReVersion.h"

namespace OSSObjectStorageClientMethod
{
	EOSSLockType IsLockOSSServer(const FString &InBucketName, const FString &InVersionLockPath,const FString &InPlatform);
	bool GetServerVersion(const FString &InBucketName, const FString& InServerVersionName,const FString &Platform, FVersion& InVersion);
	bool PutServerVersion(const FString& InBucketName, const FString& InServerVersionName, const FString& Platform, const FVersion& InVersion);
	bool PutServerVersion(const FString& InBucketName, const FString& InServerVersionName, const FString& Platform, const FString& InVersion);
	struct FOSSLock
	{
		FOSSLock(const FString& InBucketName, const FString& InVersionLockPath, const FString& InPlatform);
		~FOSSLock();

		FString BucketName;
		FString VersionLockPath;
		FString Platform;
	};
}