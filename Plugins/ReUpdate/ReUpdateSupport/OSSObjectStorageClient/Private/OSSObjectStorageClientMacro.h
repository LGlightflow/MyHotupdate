

#pragma once
#include "OSSObjectStorageClientMethod.h"

#define LOCK_OSS_SERVER(Parm1,Parm2,Parm3) OSSObjectStorageClientMethod::FOSSLock(Parm1,Parm2,Parm3);

#define CHECK_OSS_LOCK_STATUS(FunctionReturnValue,InBucketName,InVersionLockPath,InPlatform,Code) \
EOSSLockType OSSLockT = OSSObjectStorageClientMethod::IsLockOSSServer(InBucketName, InVersionLockPath, InPlatform); \
if (OSSLockT == EOSSLockType::OSS_TRUE || OSSLockT == EOSSLockType::OSS_CONNECTION_FAILED) \
{\
	Code; \
	UE_LOG(LogOSSObjectStorageClient, Warning, TEXT("The server is occupied by another administrator .")); \
	return FunctionReturnValue; \
}\
LOCK_OSS_SERVER(InBucketName,InVersionLockPath,InPlatform)