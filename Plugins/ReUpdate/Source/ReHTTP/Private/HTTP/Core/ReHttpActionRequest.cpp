// Copyright (C) RenZhai.2020.All Rights Reserved.

#include "HTTP/Core/ReHttpActionRequest.h"
#include "Client/HTTPClient.h"
#include "Core/ReHttpMacro.h"
#include "HAL/FileManager.h"
#include "ReHttpLog.h"
#include "Misc/Paths.h"
#include "Math/UnrealMathUtility.h"
#include "Misc/FileHelper.h"

//#include "GenericPlatform/GenericPlatformHttp.h"

FReHttpActionRequest::FReHttpActionRequest()
	:bRequestComplete(false)
	,bSaveDisk(true)
	,bAsynchronous(true)
{
}

FReHttpActionRequest::~FReHttpActionRequest()
{

}

bool FReHttpActionRequest::Suspend()
{
	checkf(0,TEXT("UE HTTP currently does not support single pause. However, we support the suspension of the entire HTTP download!"));
	
	return false;
}

bool FReHttpActionRequest::Cancel()
{
	return false;
}

bool FReHttpActionRequest::SendRequest(EReHTTPVerbType InType,const FString& URL, const TMap<FString, FString>& InMeta, const FString& InConentData)
{
	return false;
}

bool FReHttpActionRequest::SendRequest(EReHTTPVerbType InType, const FString& URL, const TMap<FString, FString>& InMeta, const TArray<uint8>& InData)
{
	return false;
}

void FReHttpActionRequest::GetObjects(const TArray<FString> &URL, const FString &SavePaths)
{

}

void FReHttpActionRequest::GetObjects(const TArray<FString> &URL)
{

}

void FReHttpActionRequest::DeleteObjects(const TArray<FString> &URL)
{

}

void FReHttpActionRequest::HttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FString DebugPram;
	Request->GetURLParameter(DebugPram);
	UE_LOG(LogReHTTP, Warning,
		TEXT("HttpRequestComplete URL=%s,Param=%s"),
		*Request->GetURL(),
		*DebugPram);

	//404 405 100 -199 200 -299
	if (!Request.IsValid())
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccessfully);

		UE_LOG(LogReHTTP, Warning, TEXT("Server request failed."));
	}
	else if(!Response.IsValid())
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccessfully);
		UE_LOG(LogReHTTP, Warning, TEXT(" Response failed."));
	}
	else if (!bConnectedSuccessfully)
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccessfully);
		UE_LOG(LogReHTTP, Warning, TEXT("HTTP connected failed. msg[%s]"),
			*Response->GetContentAsString());
	}
	else if (!EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		ExecutionCompleteDelegate(Request, Response, bConnectedSuccessfully);
		UE_LOG(LogReHTTP, Warning, TEXT("HTTP error code [%d] Msg[%s]."), 
			Response->GetResponseCode(),
			*Response->GetContentAsString());
	}
	else
	{
		if (Request->GetVerb() == "GET")
		{
			if (bSaveDisk)
			{
				FString Filename = FPaths::GetCleanFilename(Request->GetURL());
				FFileHelper::SaveArrayToFile(Response->GetContent(), *(GetPaths() / Filename));

				UE_LOG(LogReHTTP, Log, TEXT("Store the obtained http file locally."));
				UE_LOG(LogReHTTP, Log, TEXT("%s."), *Filename);
			}
			else
			{
				UE_LOG(LogReHTTP, Log, TEXT("This is a get request that is not stored locally."));
			}
		}

		ExecutionCompleteDelegate(Request, Response, bConnectedSuccessfully);
		UE_LOG(LogReHTTP, Log, TEXT("Request to complete execution of binding agent."));
	}
}

void FReHttpActionRequest::HttpRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived)
{
	FReHttpRequest ReHttpRequest;
	RequestPtrToSimpleRequest(Request, ReHttpRequest);

	ReHttpRequestProgressDelegate.ExecuteIfBound(ReHttpRequest, BytesSent, BytesReceived);
	ReSingleRequestProgressDelegate.ExecuteIfBound(ReHttpRequest, BytesSent, BytesReceived);
}

void FReHttpActionRequest::HttpRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived)
{
	FReHttpRequest ReHttpRequest;
	RequestPtrToSimpleRequest(Request, ReHttpRequest);

	ReHttpRequestProgressDelegate.ExecuteIfBound(ReHttpRequest, BytesSent, BytesReceived);
	ReSingleRequestProgressDelegate.ExecuteIfBound(ReHttpRequest, BytesSent, BytesReceived);

//	UE_LOG(LogReHTTP, Log, TEXT("Http request progress."));
}

void FReHttpActionRequest::HttpRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue)
{
	FReHttpRequest ReHttpRequest;
	RequestPtrToSimpleRequest(Request, ReHttpRequest);

	ReHttpRequestHeaderReceivedDelegate.ExecuteIfBound(ReHttpRequest, HeaderName, NewHeaderValue);
	ReSingleRequestHeaderReceivedDelegate.ExecuteIfBound(ReHttpRequest, HeaderName, NewHeaderValue);

//	UE_LOG(LogReHTTP, Log, TEXT("Http request header received."));
}

void FReHttpActionRequest::Print(const FString &Msg, float Time /*= 10.f*/, FColor Color /*= FColor::Red*/)
{
#ifdef PLATFORM_PROJECT
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, Time, Color, Msg);
	}
#endif
}

void FReHttpActionRequest::ExecutionCompleteDelegate(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	FReHttpRequest ReHttpRequest;
	FReHttpResponse ReHttpResponse;
	ResponsePtrToSimpleResponse(Response, ReHttpResponse);
	RequestPtrToSimpleRequest(Request, ReHttpRequest);

	ReHttpRequestCompleteDelegate.ExecuteIfBound(ReHttpRequest, ReHttpResponse, bConnectedSuccessfully);
	ReCompleteDelegate.ExecuteIfBound(ReHttpRequest, ReHttpResponse, bConnectedSuccessfully);
}

bool FReHttpActionRequest::GetObject(const FString &URL, const FString &SavePaths)
{
	return false;
}

bool FReHttpActionRequest::GetObject(const FString &URL)
{
	return false;
}

bool FReHttpActionRequest::PutObject(const FString &URL,const TArray<uint8> &Data)
{
	return false;
}

bool FReHttpActionRequest::PutObject(const FString &URL, const FString &LocalPaths)
{
	return false;
}

bool FReHttpActionRequest::PutObjectByString(const FString& URL, const FString& InBuff)
{
	return false;
}

bool FReHttpActionRequest::PutObject(const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream)
{
	return false;
}

bool FReHttpActionRequest::DeleteObject(const FString &URL)
{
	return false;
}

bool FReHttpActionRequest::PostObject(const FString &URL)
{
	return false;
}

bool FReHttpActionRequest::PostObject(const FString& URL, const FString& LocalPaths)
{
	return false;
}

bool FReHttpActionRequest::PostObject(const FString& URL, const TArray<uint8>& Data)
{
	return false;
}

void FReHttpActionRequest::RequestPtrToSimpleRequest(FHttpRequestPtr Request, FReHttpRequest& ReHttpRequest)
{
	if (Request.IsValid())
	{
		ReHttpRequest.Verb = Request->GetVerb();
		ReHttpRequest.URL = Request->GetURL();
		ReHttpRequest.Status = (FReHttpStarte)Request->GetStatus();
		ReHttpRequest.ElapsedTime = Request->GetElapsedTime();
		ReHttpRequest.ContentType = Request->GetContentType();
		ReHttpRequest.ContentLength = Request->GetContentLength();
		ReHttpRequest.AllHeaders = Request->GetAllHeaders();
		ReHttpRequest.RowPtr = &Request;
	}
}

void FReHttpActionRequest::ResponsePtrToSimpleResponse(FHttpResponsePtr Response, FReHttpResponse& ReHttpResponse)
{
	if (Response.IsValid())
	{
		ReHttpResponse.ResponseCode = Response->GetResponseCode();
		ReHttpResponse.URL = Response->GetURL();
		ReHttpResponse.ResponseMessage = Response->GetContentAsString();
		ReHttpResponse.ContentType = Response->GetContentType();
		ReHttpResponse.ContentLength = Response->GetContentLength();
		ReHttpResponse.AllHeaders = Response->GetAllHeaders();
		ReHttpResponse.Content->Content = const_cast<TArray<uint8>*>(&Response->GetContent());
		ReHttpResponse.RowPtr = &Response;
	}
}