// Copyright (C) RenZhai.2020.All Rights Reserved.
#include "HTTPObjectStorageClientMethod.h"
#include "HTTPObjectStorageClientLog.h"
#include "SimpleHttpManage.h"
#include "SimpleHTTPType.h"

namespace HTTPObjectStorageClientMethod
{
	FString GetParseValue(const FString& InKey)
	{
		FString Value;
		if (!FParse::Value(FCommandLine::Get(), *InKey, Value))
		{
			UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("%s was not found value"), *InKey);
		}

		return Value;
	}

	void CheckHTTPServerLock(TFunction<void(bool)> InFun)
	{
		ProtocolLock(InFun, EHTTPObjectStorageProtocol::HSP_CHECK_LOCK);
	}

	void HTTPServerLock(TFunction<void(bool)> InFun)
	{
		ProtocolLock(InFun, EHTTPObjectStorageProtocol::HSP_LOCK);
	}

	void HTTPServerUnLock(TFunction<void(bool)> InFun)
	{
		ProtocolLock(InFun, EHTTPObjectStorageProtocol::HSP_UNLOCK);
	}

	void GetHTTPServerVersion(TFunction<void(const FString&)> InFun)
	{
		const FString Bucket = GetParseValue(TEXT("-Bucket="));
		const FString VersionLock = GetParseValue(TEXT("-VersionLock="));
		const FString Platform = GetParseValue(TEXT("-Platform="));
		const FString Endpoint = GetParseValue(TEXT("-Endpoint="));
		FString ServerVersionName = GetParseValue(TEXT("-ServerVersionName="));

		FString URL = TEXT("http://") + Endpoint / Bucket / Platform / ServerVersionName;

		FSimpleHTTPResponseDelegate Delegate;
		Delegate.SimpleCompleteDelegate.BindLambda(
		[&,InFun](const FSimpleHttpRequest& InHttpRequest, const FSimpleHttpResponse& InHttpResponse, bool bLink)
		{
			if (bLink)
			{
				InFun(InHttpResponse.ResponseMessage);
			}
			else
			{
				UE_LOG(LogHTTPObjectStorageClient, Error, TEXT("Failed to get server version"));
				InFun(TEXT("Error"));
			}
		});

		if (!SIMPLE_HTTP.GetObjectToMemory(Delegate, URL,true))
		{
			UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("Get Server Version Fail!"));
		}
	}

	void ProtocolLock(TFunction<void(bool)> InFun, EHTTPObjectStorageProtocol InProtocol)
	{
		const FString Bucket = GetParseValue(TEXT("-Bucket="));
		const FString VersionLock = GetParseValue(TEXT("-VersionLock="));
		const FString Platform = GetParseValue(TEXT("-Platform="));
		const FString Endpoint = GetParseValue(TEXT("-Endpoint="));

		FString URL = TEXT("http://") + Endpoint / HTTPServerScriptPathName;
		FSimpleHTTPResponseDelegate ResponseDelegate;
		ResponseDelegate.SimpleCompleteDelegate.BindLambda(
		[&](const FSimpleHttpRequest& InHttpRequest, const FSimpleHttpResponse& InHttpResponse, bool bLink)
		{
			UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("%s"), *InHttpResponse.ResponseMessage);
			InFun(InHttpResponse.ResponseMessage.ToBool());
		});

		FString Prams = FString::Printf(
			TEXT("Protocol=%i&VersionLockPath=%s"),
			InProtocol,
			*(Bucket / Platform / VersionLock));

		SIMPLE_HTTP.PostRequest(*URL, *Prams, ResponseDelegate,true);
	}

	void PutServerVersion(const FString& ServerVersionJson, TFunction<void(bool)> InFun)
	{
		const FString Bucket = GetParseValue(TEXT("-Bucket="));
		const FString Platform = GetParseValue(TEXT("-Platform="));
		const FString Endpoint = GetParseValue(TEXT("-Endpoint="));
		const FString ServerVersionName = GetParseValue(TEXT("-ServerVersionName="));
		FString URL = TEXT("http://") + Endpoint / Bucket/ Platform / ServerVersionName;

		FSimpleHTTPResponseDelegate ResponseDelegate;
		ResponseDelegate.SimpleCompleteDelegate.BindLambda(
		[InFun](const FSimpleHttpRequest& InHttpRequest, const FSimpleHttpResponse& InHttpResponse, bool bLink)
		{
			UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("%s"), *InHttpResponse.ResponseMessage);
			InFun(bLink);
		});

		SIMPLE_HTTP.PutObjectFromString(ResponseDelegate, URL, ServerVersionJson,true);
	}

	void PutServerVersion(const FVersion& ServerVersion, TFunction<void(bool)> InFun)
	{
		FString JsonString;
		SimpleVersionControl::Save(ServerVersion, JsonString);

		PutServerVersion(JsonString, InFun);
	}
}

