// Copyright (C) RenZhai.2023.All Rights Reserved.

#include "HTTP/ReHTTPObject.h"
#include "HttpManager.h"
#include "Core/ReHTTPMethod.h"

namespace ReHTTPObject
{
	FHTTP::FHTTP(FHTTPDelegate InDelegate)
		:Delegate(InDelegate)
	{
		OutTime = 120.f;
	}

	TSharedPtr<FHTTP> FHTTP::CreateHTTPObject(FHTTPDelegate InDelegate)
	{
		return MakeShareable(new FHTTP(InDelegate));
	}

	bool FHTTP::IsValidRequest(const FGuid& InRequestID) const
	{
		return HttpRequests.Contains(InRequestID);
	}

	FGuid FHTTP::Request(
		const FString& InURL,
		const TArray<uint8>& InBytes,
		const TMap<FString, FString>& InCustomMetadataHeader,
		bool bSynchronous,
		EReHTTPVerbType VerbType)
	{
		return Request(InURL,FString(), InBytes, InCustomMetadataHeader, bSynchronous, VerbType);
	}

	void FHTTP::CancelRequest(const FGuid& InRequestID)
	{
		if (FReRequest* HttpRequest = HttpRequests.Find(InRequestID))
		{
			HttpRequest->Request->CancelRequest();
		}
	}

	FGuid FHTTP::Request(
		const FString& InURL,
		const FString& InContent,
		const TArray<uint8>& InByteData,
		const TMap<FString, FString>& InCustomMetadataHeader,
		bool bSynchronous,
		EReHTTPVerbType VerbType)
	{
		FReRequest ReRequest;

		FGuid RequestID = FGuid::NewGuid();

		ReRequest.Request = FHttpModule::Get().CreateRequest();
		ReRequest.Request->SetURL(InURL);

		ReRequest.Request->SetTimeout(OutTime);

		ReRequest.Request->SetHeader(TEXT("RequestID"), RequestID.ToString());

		for (auto& Tmp : InCustomMetadataHeader)
		{
			ReRequest.Request->SetHeader(Tmp.Key, Tmp.Value);
		}

		ReRequest.Request->SetVerb(ReHTTP::ToReHTTPVerbTypeString(VerbType));

		if (InByteData.Num() > 0)
		{
			ReRequest.Request->SetContent(InByteData);
		}
		else if (!InContent.IsEmpty())
		{
			ReRequest.Request->SetContentAsString(InContent);
		}

		ReRequest.Request->OnProcessRequestComplete().BindSP(this, &FHTTP::OnRequestComplete);

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
		ReRequest.Request->OnRequestProgress64().BindSP(this, &FHTTP::OnRequestProgress64);
#else
		ReRequest.Request->OnRequestProgress64().BindSP(this, &FHTTP::OnRequestProgress);
#endif
		ReRequest.Request->OnHeaderReceived().BindSP(this, &FHTTP::OnRequestHeaderReceived);

		bool bRequest = ReRequest.Request->ProcessRequest();
		if (bRequest)
		{
			HttpRequests.Add(RequestID, ReRequest);
			if (bSynchronous)
			{
				if (IsInGameThread())
				{
					FHttpModule::Get().GetHttpManager().Flush(EHttpFlushReason::Default);
				}
				else
				{
					//异步线程里面也可以同步
					while (HttpRequests.Contains(RequestID))
					{
						FPlatformProcess::Sleep(0.1f);
					}
				}
			}
		}
		else
		{
			RequestID = FGuid();
		}

		return RequestID;
	}

	FGuid FHTTP::Request(
		const FString& InURL,
		const FString& InContent,//xml json
		const TMap<FString, FString>& InCustomMetadataHeader,
		bool bSynchronous,
		EReHTTPVerbType VerbType)
	{
		return Request(InURL,InContent, TArray<uint8>(), InCustomMetadataHeader, bSynchronous, VerbType);
	}

	void FHTTP::OnRequestComplete(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
	{
		Delegate.ReCompleteDelegate.ExecuteIfBound(HttpRequest, HttpResponse, bSucceeded);

		FString RequestIDString = HttpRequest->GetHeader(TEXT("RequestID"));

		FGuid RequestID;
		FGuid::Parse(RequestIDString, RequestID);

		HttpRequests.Remove(RequestID);
	}

	void FHTTP::OnRequestProgress64(FHttpRequestPtr HttpRequest, uint64 SendBytes, uint64 BytesReceived)
	{
		Delegate.ReSingleRequestProgressDelegate.ExecuteIfBound(HttpRequest, SendBytes, BytesReceived);
	}

	void FHTTP::OnRequestProgress(FHttpRequestPtr HttpRequest, int32 SendBytes, int32 BytesReceived)
	{
		Delegate.ReSingleRequestProgressDelegate.ExecuteIfBound(HttpRequest, SendBytes, BytesReceived);
	}

	void FHTTP::OnRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue)
	{
		Delegate.ReSingleRequestHeaderReceivedDelegate.ExecuteIfBound(Request, HeaderName, NewHeaderValue);
	}
}