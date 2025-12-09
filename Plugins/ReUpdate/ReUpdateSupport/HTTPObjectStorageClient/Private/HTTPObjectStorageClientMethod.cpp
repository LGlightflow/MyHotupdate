
#include "HTTPObjectStorageClientMethod.h"
#include "HTTPObjectStorageClientLog.h"
#include "ReHttpManage.h"
#include "ReHTTPType.h"

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

		FReHTTPResponseDelegate Delegate;
		Delegate.ReCompleteDelegate.BindLambda(
		[&,InFun](const FReHttpRequest& InHttpRequest, const FReHttpResponse& InHttpResponse, bool bLink)
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

		if (!RE_HTTP.GetObjectToMemory(Delegate, URL,true))
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
		FReHTTPResponseDelegate ResponseDelegate;
		ResponseDelegate.ReCompleteDelegate.BindLambda(
		[&](const FReHttpRequest& InHttpRequest, const FReHttpResponse& InHttpResponse, bool bLink)
		{
			UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("%s"), *InHttpResponse.ResponseMessage);
			InFun(InHttpResponse.ResponseMessage.ToBool());
		});

		FString Prams = FString::Printf(
			TEXT("Protocol=%i&VersionLockPath=%s"),
			InProtocol,
			*(Bucket / Platform / VersionLock));

		RE_HTTP.PostRequest(*URL, *Prams, ResponseDelegate,true);
	}

	void PutServerVersion(const FString& ServerVersionJson, TFunction<void(bool)> InFun)
	{
		const FString Bucket = GetParseValue(TEXT("-Bucket="));
		const FString Platform = GetParseValue(TEXT("-Platform="));
		const FString Endpoint = GetParseValue(TEXT("-Endpoint="));
		const FString ServerVersionName = GetParseValue(TEXT("-ServerVersionName="));
		FString URL = TEXT("http://") + Endpoint / Bucket/ Platform / ServerVersionName;

		FReHTTPResponseDelegate ResponseDelegate;
		ResponseDelegate.ReCompleteDelegate.BindLambda(
		[InFun](const FReHttpRequest& InHttpRequest, const FReHttpResponse& InHttpResponse, bool bLink)
		{
			UE_LOG(LogHTTPObjectStorageClient, Display, TEXT("%s"), *InHttpResponse.ResponseMessage);
			InFun(bLink);
		});

		RE_HTTP.PutObjectFromString(ResponseDelegate, URL, ServerVersionJson,true);
	}

	void PutServerVersion(const FVersion& ServerVersion, TFunction<void(bool)> InFun)
	{
		FString JsonString;
		ReVersionControl::Save(ServerVersion, JsonString);

		PutServerVersion(JsonString, InFun);
	}
}

