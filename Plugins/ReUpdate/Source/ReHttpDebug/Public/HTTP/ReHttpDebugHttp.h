#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "Runtime/Launch/Resources/Version.h"

namespace REHTTPDEBUGHTTP
{
	enum class EHTTPVerbType :uint8
	{
		HOTUPDATE_POST,
		HOTUPDATE_PUT,
		HOTUPDATE_GET,
		HOTUPDATE_DELETE,
	};

	struct REHTTPDEBUG_API FHTTPDelegate
	{
		FHttpRequestCompleteDelegate						SimpleCompleteDelegate;
#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
		FHttpRequestProgressDelegate64						SimpleSingleRequestProgressDelegate;
#else
		FHttpRequestProgressDelegate						SimpleSingleRequestProgressDelegate;
#endif
		FHttpRequestHeaderReceivedDelegate					SimpleSingleRequestHeaderReceivedDelegate;
	};

	struct REHTTPDEBUG_API FHTTP :public TSharedFromThis<FHTTP>
	{
		static TSharedPtr<FHTTP> CreateHTTPObject(FHTTPDelegate InDelegate);

	public:
		FHTTP(FHTTPDelegate InDelegate);

		//请求String
		FGuid Request(
			const FString& InURL,
			const FString& InContent,
			const TMap<FString, FString>& InCustomMetadataHeader,
			bool bSynchronous = false,
			REHTTPDEBUGHTTP::EHTTPVerbType VerbType = REHTTPDEBUGHTTP::EHTTPVerbType::HOTUPDATE_POST);

		//二进制数据
		FGuid Request(
			const FString& InURL,
			const TArray<uint8>& InByteData,
			const TMap<FString, FString>& InCustomMetadataHeader,
			bool bSynchronous = false,
			REHTTPDEBUGHTTP::EHTTPVerbType VerbType = REHTTPDEBUGHTTP::EHTTPVerbType::HOTUPDATE_POST);

		void CancelRequest(const FGuid& InRequestID);
		bool IsValidRequest(const FGuid& InRequestID) const;

	private:
		FGuid Request(
			const FString& InURL,
			const FString& InContent1,
			const TArray<uint8>& InContent2,
			const TMap<FString, FString>& InCustomMetadataHeader,
			bool bSynchronous = false,
			REHTTPDEBUGHTTP::EHTTPVerbType VerbType = REHTTPDEBUGHTTP::EHTTPVerbType::HOTUPDATE_POST);

	private:
		void OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);
		void OnRequestProgress64(FHttpRequestPtr HttpRequest, uint64 SendBytes, uint64 BytesReceived);
		void OnRequestProgress(FHttpRequestPtr HttpRequest, int32 SendBytes, int32 BytesReceived);
		void OnRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue);

	private:
		FString HTTPVerbToString(REHTTPDEBUGHTTP::EHTTPVerbType InVerbType);

	private:
		TMap<FGuid, TSharedPtr<IHttpRequest, ESPMode::ThreadSafe>> HttpRequests;
		REHTTPDEBUGHTTP::FHTTPDelegate Delegate;
		bool bRequestComplete;
	};
}