#include "HTTP/ReHttpDebugHttp.h"
#include "HttpManager.h"

namespace ReHttpDebugHTTP
{
	void FHTTP::CancelRequest(const FGuid& InRequestID)
	{
		if (TSharedPtr<IHttpRequest, ESPMode::ThreadSafe>* HttpRequest = HttpRequests.Find(InRequestID))
		{
			(*HttpRequest)->CancelRequest();
		}
	}

	bool FHTTP::IsValidRequest(const FGuid& InRequestID) const
	{
		return HttpRequests.Contains(InRequestID);
	}

	FHTTP::FHTTP(FHTTPDelegate InDelegate)
		:Delegate(InDelegate)
		, bRequestComplete(false)
	{

	}

	TSharedPtr<FHTTP> FHTTP::CreateHTTPObject(FHTTPDelegate InDelegate)
	{
		return MakeShareable(new ReHttpDebugHTTP::FHTTP(InDelegate));
	}

	FGuid FHTTP::Request(
		const FString& InURL,
		const FString& InContent,//xml json
		const TMap<FString, FString>& InCustomMetadataHeader,
		bool bSynchronous,
		ReHttpDebugHTTP::EHTTPVerbType VerbType)
	{
		return Request(InURL, InContent, TArray<uint8>(), InCustomMetadataHeader, bSynchronous, VerbType);
	}

	FGuid FHTTP::Request(
		const FString& InURL,
		const TArray<uint8>& InBytes,
		const TMap<FString, FString>& InCustomMetadataHeader,
		bool bSynchronous,
		ReHttpDebugHTTP::EHTTPVerbType VerbType)
	{
		return Request(InURL, TEXT(""), InBytes, InCustomMetadataHeader, bSynchronous, VerbType);
	}

	FGuid FHTTP::Request(
		const FString& InURL,
		const FString& InContent,
		const TArray<uint8>& InBytes,
		const TMap<FString, FString>& InCustomMetadataHeader,
		bool bSynchronous,
		ReHttpDebugHTTP::EHTTPVerbType VerbType)
	{
		FGuid RequestID = FGuid::NewGuid();

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(InURL);

		Request->SetTimeout(1100.f);

		Request->SetHeader(TEXT("RequestID"), RequestID.ToString());

		for (auto& Tmp : InCustomMetadataHeader)
		{
			Request->SetHeader(Tmp.Key, Tmp.Value);
		}

		Request->SetVerb(ReHttpDebugHTTP::FHTTP::HTTPVerbToString(VerbType));

		if (InBytes.Num() > 0)
		{
			Request->SetContent(InBytes);
		}
		else if (!InContent.IsEmpty())
		{
			Request->SetContentAsString(InContent);
		}

		Request->OnProcessRequestComplete().BindSP(this, &FHTTP::OnRequestComplete);

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
		Request->OnRequestProgress64().BindSP(this, &FHTTP::OnRequestProgress64);
#else
		Request->OnRequestProgress().BindSP(this, &FHTTP::OnRequestProgress);
#endif
		Request->OnHeaderReceived().BindSP(this, &FHTTP::OnRequestHeaderReceived);

		bool bRequest = Request->ProcessRequest();
		if (bRequest)
		{
			HttpRequests.Add(RequestID, Request);
			if (bSynchronous)
			{
				if (IsInGameThread())
				{
					FHttpModule::Get().GetHttpManager().Flush(EHttpFlushReason::Default);
				}
				else
				{
					//异步线程里面也可以同步
					while (!bRequestComplete)
					{
						FPlatformProcess::Sleep(0.1f);
					}

					bRequestComplete = false;
				}
			}
		}
		else
		{
			RequestID = FGuid();
		}

		return RequestID;
	}

	FString FHTTP::HTTPVerbToString(ReHttpDebugHTTP::EHTTPVerbType InVerbType)
	{
		switch (InVerbType)
		{
		case ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_POST:
			return TEXT("POST");
		case ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_PUT:
			return TEXT("PUT");
		case ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_GET:
			return TEXT("GET");
		case ReHttpDebugHTTP::EHTTPVerbType::HOTUPDATE_DELETE:
			return TEXT("DELETE");
		}

		return TEXT("POST");
	}

	void FHTTP::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		FString RequestIDString = HttpRequest->GetHeader(TEXT("RequestID"));

		FGuid RequestID;
		FGuid::Parse(RequestIDString, RequestID);

		HttpRequests.Remove(RequestID);

		Delegate.SimpleCompleteDelegate.ExecuteIfBound(HttpRequest, HttpResponse, bSucceeded);

		bRequestComplete = true;
	}

	void FHTTP::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 SendBytes, int32 BytesReceived)
	{
		Delegate.SimpleSingleRequestProgressDelegate.ExecuteIfBound(HttpRequest, SendBytes, BytesReceived);
	}

	void FHTTP::OnRequestProgress64(FHttpRequestPtr HttpRequest, uint64 SendBytes, uint64 BytesReceived)
	{
		Delegate.SimpleSingleRequestProgressDelegate.ExecuteIfBound(HttpRequest, SendBytes, BytesReceived);
	}

	void FHTTP::OnRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue)
	{
		Delegate.SimpleSingleRequestHeaderReceivedDelegate.ExecuteIfBound(Request, HeaderName, NewHeaderValue);
	}
}