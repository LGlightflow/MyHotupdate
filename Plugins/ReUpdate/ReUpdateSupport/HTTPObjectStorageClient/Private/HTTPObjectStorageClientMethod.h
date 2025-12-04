// Copyright (C) RenZhai.2020.All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTTPObjectStorageProtocol.h"
#include "Version/SimpleVersion.h"
#include "HTTPObjectStorageClientType.h"

namespace HTTPObjectStorageClientMethod
{
	FString GetParseValue(const FString& InKey);

	void CheckHTTPServerLock(TFunction<void(bool)> InFun);

	void HTTPServerLock(TFunction<void(bool)> InFun);
	void HTTPServerUnLock(TFunction<void(bool)> InFun);

	void GetHTTPServerVersion(TFunction<void(const FString &)> InFun);

	void ProtocolLock(TFunction<void(bool)> InFun, EHTTPObjectStorageProtocol InProtocol);
	
	void PutServerVersion(const FString& ServerVersionJson, TFunction<void(bool)> InFun);
	void PutServerVersion(const FVersion &ServerVersion ,TFunction<void(bool)> InFun);
}