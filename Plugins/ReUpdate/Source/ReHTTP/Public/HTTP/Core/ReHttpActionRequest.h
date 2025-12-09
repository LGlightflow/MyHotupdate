

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "HTTP/Core/ReHttpActionRequest.h"
#include "ReHTTPType.h"
#include "Request/RequestInterface.h"

/**
 * 
 */
class REHTTP_API FReHttpActionRequest :public TSharedFromThis<FReHttpActionRequest>
{
public:
	typedef FReHttpActionRequest Super;

	//BP
	FReHttpSingleRequestCompleteDelegate			ReHttpRequestCompleteDelegate;
	FReHttpSingleRequestProgressDelegate			ReHttpRequestProgressDelegate;
	FReHttpSingleRequestHeaderReceivedDelegate		ReHttpRequestHeaderReceivedDelegate;
	FAllRequestCompleteDelegate						AllRequestCompleteDelegate;

	//C++
	FReSingleCompleteDelegate						ReCompleteDelegate;
	FReSingleRequestProgressDelegate				ReSingleRequestProgressDelegate;
	FReSingleRequestHeaderReceivedDelegate			ReSingleRequestHeaderReceivedDelegate;
	FSimpleDelegate									AllTasksCompletedDelegate;

public:
	FReHttpActionRequest();
	virtual ~FReHttpActionRequest();

	virtual bool Suspend();
	virtual bool Cancel();

	virtual bool SendRequest(EReHTTPVerbType InType, const FString& URL, const TMap<FString, FString>& InMeta, const FString& InConentData);
	virtual bool SendRequest(EReHTTPVerbType InType, const FString& URL, const TMap<FString, FString>& InMeta, const TArray<uint8>& InData);

	//Compatibility blueprint
	virtual void GetObjects(const TArray<FString> &URL, const FString &SavePaths);
	virtual void GetObjects(const TArray<FString> &URL);
	virtual void DeleteObjects(const TArray<FString> &URL);

	virtual bool GetObject(const FString &URL);
	virtual bool GetObject(const FString &URL, const FString &SavePaths);
	virtual bool PutObject(const FString &URL, const TArray<uint8> &Data);
	virtual bool PutObject(const FString &URL, const FString &LocalPaths);
	virtual bool PutObjectByString(const FString& URL, const FString& InBuff);
	virtual bool PutObject(const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
	virtual bool DeleteObject(const FString &URL);
	virtual bool PostObject(const FString &URL);
	virtual bool PostObject(const FString& URL, const FString& LocalPaths);
	virtual bool PostObject(const FString& URL, const TArray<uint8>& Data);

	FORCEINLINE FString GetPaths() { return TmpSavePaths; }
	FORCEINLINE void SetPaths(const FString &NewPaths) { TmpSavePaths = NewPaths; }
	FORCEINLINE bool IsRequestComplete() { return bRequestComplete; }

	FORCEINLINE void SetAsynchronousState(bool bNewAsy) { bAsynchronous = bNewAsy; }

protected:
	virtual void HttpRequestComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
	virtual void HttpRequestProgress(FHttpRequestPtr Request, int32 BytesSent, int32 BytesReceived);
	virtual void HttpRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived);
	virtual void HttpRequestHeaderReceived(FHttpRequestPtr Request, const FString& HeaderName, const FString& NewHeaderValue);

	void Print(const FString &Msg, float Time = 10.f, FColor Color = FColor::Red);

protected:
	void RequestPtrToSimpleRequest(FHttpRequestPtr Request, FReHttpRequest& ReHttpRequest);
	void ResponsePtrToSimpleResponse(FHttpResponsePtr Response, FReHttpResponse& ReHttpResponse);

protected:
	virtual void ExecutionCompleteDelegate(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

protected:
	FString						TmpSavePaths;
	bool						bRequestComplete;
	bool						bSaveDisk;
	bool						bAsynchronous;
};
