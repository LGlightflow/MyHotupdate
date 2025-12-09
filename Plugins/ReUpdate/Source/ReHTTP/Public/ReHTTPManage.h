
#pragma once

#include "CoreMinimal.h"
#include "HTTP/Core/ReHTTPHandle.h"
#include "ReHTTPType.h"
#ifdef PLATFORM_PROJECT
#include "Tickable.h"
#endif

class FReHttpActionRequest;
const FName NONE_NAME = TEXT("NONE");
/*
 * A Re set of HTTP interface functions can quickly perform HTTP code operations. 
 * Only one interface is needed to interact with our HTTP server. Currently, 
 * HTTP supports downloading, uploading, deleting and other operations. 
 * See our API for details
*/
class REHTTP_API FReHttpManage 
#ifdef PLATFORM_PROJECT
	:public FTickableGameObject
#endif
{
	/**
	 * Used to determine whether the object should be ticked in the editor.  Defaults to false since
	 * that is the previous behavior.
	 *
	 * @return	true if this tickable object can be ticked in the editor
	 */
	virtual bool IsTickableInEditor() const;

	/** return the stat id to use for this tickable **/
	virtual TStatId GetStatId() const;

	/** Get HTTP function collection  **/
	struct REHTTP_API FHTTP
	{
		/*We want to have direct access to the commands in http .*/
		friend class FReHttpManage;

		FHTTP();

		 /**
		  * Pause all downloading content.
		*/
		void Suspend();

		/**
		 * awaken all downloading Pause content.
		*/
		void Awaken();

		/**
		 * Specify the content to pause the download.
		 */
		bool Suspend(const FReHTTPHandle& Handle);
		
		/**
		 * Cancel all downloads.
		 */
		bool Cancel();
	
		/**
		 * 	Cancel the specified Download.
		*/
		bool Cancel(const FReHTTPHandle& Handle);

		/*Gets the handle of the last execution request*/
		FReHTTPHandle GetHandleByLastExecutionRequest();

		bool RequestByConentString(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const FString& InContent, const FReHTTPBPResponseDelegate& BPResponseDelegate);
		bool RequestByConentByte(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes, const FReHTTPBPResponseDelegate& BPResponseDelegate);
		
		bool RequestByConentString(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const FString& InContent, const FReHTTPResponseDelegate& InResponseDelegate);
		bool RequestByConentByte(EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes, const FReHTTPResponseDelegate& InResponseDelegate);
		
		/**
		 * Submit form to server.
		 *
		 * @param InURL						Address to visit.
		 * @param InParam					Parameters passed.
		 * @param BPResponseDelegate		Proxy for site return.
		 */
		bool PostRequest(const TCHAR *InURL, const TCHAR *InParam, const FReHTTPBPResponseDelegate &BPResponseDelegate, bool bSynchronouse=false);

		/**
		 * The data can be downloaded to local memory via the HTTP serverll .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @Return						Returns true if the request succeeds 
		 */
		bool GetObjectToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, bool bSynchronous=false);
		
		/**
		 * The data can be downloaded to local memory via the HTTP serverll.
		 * Can download multiple at one time .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					Need domain name .
		 */
		void GetObjectsToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, bool bSynchronous = false);

		/**
		 * Download individual data locally.
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param SavePaths				Path to local storage .
		 * @Return						Returns true if the request succeeds 
		 */
		bool GetObjectToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &SavePaths);
		
		/**
		 * Download multiple data to local .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					Need domain name .
		 * @param SavePaths				Path to local storage .
		 */
		void GetObjectsToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, const FString &SavePaths);

		/**
		 * Upload single file from disk to server .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param LocalPaths			Specify the Path where you want to upload the file.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse=false);
		
		/**
		 * Upload duo files from disk to server  .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param LocalPaths			Specify the Path where you want to upload the file.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectsFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse = false);

		/**
		 * Can upload byte data .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param Buffer				Byte code data.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromBuffer(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const TArray<uint8> &Buffer);
		
		/**
		 * Can upload string data .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param Buffer				string code data.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromString(const FReHTTPBPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& InBuffer, bool bSynchronouse = false);

		/**
		 * Stream data upload supported by UE4 .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param Stream				UE4 storage structure .
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromStream(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);

		/**
		 * Remove a single object from the server .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @Return						Returns true if the request succeeds
		 */
		bool DeleteObject(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL);
		
		/**
		 * Multiple URLs need to be specified to remove multiple objects from the server .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					Need domain name .
		 */
		void DeleteObjects(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL);
		
		//////////////////////////////////////////////////////////////////////////

		/**
		 * Submit form to server.
		 *
		 * @param InURL						Address to visit.
		 * @param InParam					Parameters passed.
		 * @param BPResponseDelegate		Proxy for site return.
		 */
		bool PostRequest(const TCHAR *InURL, const TCHAR *InParam, const FReHTTPResponseDelegate &BPResponseDelegate, bool bSynchronouse = false);

		/**
		 * The data can be downloaded to local memory via the HTTP serverll .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @Return						Returns true if the request succeeds
		 */
		bool GetObjectToMemory(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, bool bSynchronous=false);

		/**
		 * The data can be downloaded to local memory via the HTTP serverll.
		 * Can download multiple at one time .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					Need domain name .
		 */
		void GetObjectsToMemory(const FReHTTPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, bool bSynchronous = false);

		/**
		 * Download individual data locally.
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @param SavePaths				Path to local storage .
		 * @Return						Returns true if the request succeeds
		 */
		bool GetObjectToLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &SavePaths);

		/**
		 * Download multiple data to local .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					Need domain name .
		 * @param SavePaths				Path to local storage .
		 */
		void GetObjectsToLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, const FString &SavePaths);

		/**
		 * Upload single file from disk to server .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @param LocalPaths			Specify the Path where you want to upload the file.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse = false);
		bool PostObjectFromLocal(const FReHTTPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& LocalPaths, bool bSynchronouse = false);
		
		/**
		 * Upload duo files from disk to server  .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @param LocalPaths			Specify the Path where you want to upload the file.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectsFromLocal(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths, bool bSynchronouse = false);
		bool PostObjectsFromLocal(const FReHTTPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& LocalPaths, bool bSynchronouse = false);

		/**
		 * Can upload byte data .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @param Buffer				Byte code data.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromBuffer(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, const TArray<uint8> &Buffer);
		
		/**
		 * Can upload string data .
		 *
		 * @param BPResponseDelegate	Proxy set relative to the blueprint.
		 * @param URL					domain name .
		 * @param Buffer				string code data.
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromString(const FReHTTPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& InBuffer, bool bSynchronouse=false);

		/**
		 * Stream data upload supported by UE4 .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @param Stream				UE4 storage structure .
		 * @Return						Returns true if the request succeeds
		 */
		bool PutObjectFromStream(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);

		/**
		 * Remove a single object from the server .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					domain name .
		 * @Return						Returns true if the request succeeds
		 */
		bool DeleteObject(const FReHTTPResponseDelegate &BPResponseDelegate, const FString &URL);

		/**
		 * Multiple URLs need to be specified to remove multiple objects from the server .
		 *
		 * @param BPResponseDelegate	C + + based proxy interface .
		 * @param URL					Need domain name .
		 */
		void DeleteObjects(const FReHTTPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL);

	private:
		bool RequestByConentString(FReHTTPHandle Handle, EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const FString& InContent);
		bool RequestByConentByte(FReHTTPHandle Handle, EReHTTPVerbType InType, const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes);

		/**
		 * Register our agent BP for internal use .
		 *
		 * @param return	Use handle find PRequest.
		 */
		FReHTTPHandle RegisteredHttpRequest(EHTTPRequestType RequestType = EHTTPRequestType::SINGLE,
			FReHttpSingleRequestCompleteDelegate ReHttpRequestCompleteDelegate = FReHttpSingleRequestCompleteDelegate(),
			FReHttpSingleRequestProgressDelegate	ReHttpRequestProgressDelegate = FReHttpSingleRequestProgressDelegate(),
			FReHttpSingleRequestHeaderReceivedDelegate ReHttpRequestHeaderReceivedDelegate = FReHttpSingleRequestHeaderReceivedDelegate(), 
			FAllRequestCompleteDelegate AllRequestCompleteDelegate = FAllRequestCompleteDelegate());

		/**
		 * Register our agent BP for internal use .
		 *
		 * @param return	Use handle find PRequest.
		 */
		FReHTTPHandle RegisteredHttpRequest(EHTTPRequestType RequestType = EHTTPRequestType::SINGLE,
			FReSingleCompleteDelegate ReHttpRequestCompleteDelegate = nullptr,
			FReSingleRequestProgressDelegate	ReHttpRequestProgressDelegate = nullptr,
			FReSingleRequestHeaderReceivedDelegate ReHttpRequestHeaderReceivedDelegate = nullptr,
			FSimpleDelegate AllRequestCompleteDelegate = nullptr);

		/** 
		 * Refer to the previous API for internal use details only 
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool GetObjectToMemory(const FReHTTPHandle &Handle,const FString &URL, bool bSynchronous = false);
		
		/** 
		 * Refer to the previous API for internal use details only 
		 *
		 * @param Handle	Easy to find requests .
		 */
		void GetObjectsToMemory(const FReHTTPHandle &Handle, const TArray<FString> &URL, bool bSynchronous = false);
	
		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool GetObjectToLocal(const FReHTTPHandle &Handle, const FString &URL, const FString &SavePaths);

		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		void GetObjectsToLocal(const FReHTTPHandle &Handle, const TArray<FString> &URL, const FString &SavePaths);
	
		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool PutObjectFromLocal(const FReHTTPHandle &Handle, const FString &URL, const FString &LocalPaths, bool bSynchronouse = false);
		bool PostObjectFromLocal(const FReHTTPHandle& Handle, const FString& URL, const FString& LocalPaths, bool bSynchronouse = false);

		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool PutObjectsFromLocal(const FReHTTPHandle &Handle, const FString &URL, const FString &LocalPaths, bool bSynchronouse=false);
		
		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool PutObjectFromBuffer(const FReHTTPHandle &Handle, const FString &URL, const TArray<uint8> &Buffer);
		
		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool PutObjectFromString(const FReHTTPHandle& Handle, const FString& URL, const FString& Buffer, bool bSynchronouse = false);

		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool PutObjectFromStream(const FReHTTPHandle &Handle, const FString &URL, TSharedRef<FArchive, ESPMode::ThreadSafe> Stream);
	
		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		bool DeleteObject(const FReHTTPHandle &Handle, const FString &URL);
		
		/**
		 * Refer to the previous API for internal use details only
		 *
		 * @param Handle	Easy to find requests .
		 */
		void DeleteObjects(const FReHTTPHandle &Handle, const TArray<FString> &URL);

		/**
		 * Submit form to server.
		 *
		 * @param Handle					It can be used to retrieve the corresponding request.
		 * @param InURL						Address to visit.
		 */
		bool PostRequest(const FReHTTPHandle &Handle, const FString &URL, bool bSynchronouse = false);

	private:
		/*You can find the corresponding request according to the handle  */
		TWeakPtr<FReHttpActionRequest> Find(const FReHTTPHandle &Handle);

		/*HTTP Map*/
		TMap<FReHTTPHandle, TSharedPtr<FReHttpActionRequest>> HTTPMap;

		/*Pause all download tasks*/
		/*UE HTTP currently does not support single pause. However, we support the suspension of the entire HTTP download!*/
		bool bPause;

		/*The handle points to the request just pointed to*/
		FName TemporaryStorageHandle;
	};

public:
	virtual ~FReHttpManage(){}

	/**
	 * GC
	 * Pure virtual that must be overloaded by the inheriting class. It will
	 * be called from within LevelTick.cpp after ticking all actors or from
	 * the rendering thread (depending on bIsRenderingThreadObject)
	 *
	 * @param DeltaTime	Game time passed since the last call.
	 */
	virtual void Tick(float DeltaTime);

	static FReHttpManage *Get();
	static void Destroy();

	/** Get HTTP function collection  **/
	FORCEINLINE FHTTP &GetHTTP() { return HTTP; }
private:

	static FReHttpManage *Instance;
	FHTTP HTTP;
	FCriticalSection Mutex;
};

#define RE_HTTP FReHttpManage::Get()->GetHTTP()
