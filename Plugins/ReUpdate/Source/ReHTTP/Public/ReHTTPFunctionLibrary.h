

#pragma once

#include "CoreMinimal.h"
#include "ReHTTPType.h"
#include "ReHTTPFunctionLibrary.generated.h"

/**
 * A Re set of HTTP interface functions can quickly perform HTTP code operations.
 * Only one interface is needed to interact with our HTTP server. Currently,
 * HTTP supports downloading, uploading, deleting and other operations.
 * See our API for details
*/
UCLASS(meta = (BlueprintThreadSafe, ScriptName = "HTTPLibrary"))
class REHTTP_API UReHTTPFunctionLibrary : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Pause all downloading content.
	*/
	UFUNCTION(BlueprintCallable, Category = "ReHTTP")
	static void Pause();

	/**
	 * awaken all downloading Pause content.
	*/
	UFUNCTION(BlueprintCallable, Category = "ReHTTP")
	static void Awaken();

	/**
	* Cancel all downloads.
	*/
	UFUNCTION(BlueprintCallable, Category = "ReHTTP")
	static bool Cancel();

	/**
	 * 	Cancel the specified Download.
	*/
	UFUNCTION(BlueprintCallable, Category = "ReHTTP")
	static bool CancelByHandle(const FName& Handle);
	
	/**
	 * Gets the handle of the last execution request
	*/
	UFUNCTION(BlueprintPure, Category = "ReHTTP")
	static FName GetHandleByLastExecutionRequest();

	/**
	 * Submit form to server.
	 *
	 * @param InURL						Address to visit.
	 * @param InParam					Parameters passed.
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool PostRequest(const FString &InURL, const FString &InParam, const FReHTTPBPResponseDelegate &BPResponseDelegate);

	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool RequestByConentString(EReHTTPVerbType InType, const FString& InURL,const TMap<FString,FString> &InHeadMeta,const FString &InContent,const FReHTTPBPResponseDelegate& BPResponseDelegate);
	
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool RequestByConentByte(EReHTTPVerbType InType,const FString& InURL, const TMap<FString, FString>& InHeadMeta, const TArray<uint8>& InBytes, const FReHTTPBPResponseDelegate& BPResponseDelegate);

	/**
	 *If platform is not turned PLATFORM_PROJECT macro, there is no need to manually put it in the tick of the project
	 * @See ReHTTP.Build.cs
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static void Tick(float DeltaTime);

	/**
	* The data can be downloaded to local memory via the HTTP serverll .
	*
	* @param BPResponseDelegate	Proxy set relative to the blueprint.
	* @param URL					domain name .
	* @Return						Returns true if the request succeeds
	*/
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool GetObjectToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL);

	/**
	 * Download individual data locally.
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					domain name .
	 * @param SavePaths				Path to local storage .
	 * @Return						Returns true if the request succeeds
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool GetObjectToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &SavePaths);

	/**
	 * Upload single file from disk to server .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					domain name .
	 * @param LocalPaths			Specify the Path where you want to upload the file.
	 * @Return						Returns true if the request succeeds
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool PutObjectFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths);
	
	/**
	 * Can upload byte data .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					domain name .
	 * @param Buffer				Byte code data.
	 * @Return						Returns true if the request succeeds
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool PutObjectFromBuffer(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const TArray<uint8> &Buffer);

	/**
	 * Can upload string data .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					domain name .
	 * @param Buffer				string data.
	 * @Return						Returns true if the request succeeds
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool PutObjectFromString(const FReHTTPBPResponseDelegate& BPResponseDelegate, const FString& URL, const FString& InBuffer);

	/**
	 * Remove a single object from the server .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					domain name .
	 * @Return						Returns true if the request succeeds
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|SingleAction")
	static bool DeleteObject(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL);

	/**
	 * Upload duo files from disk to server  .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					domain name .
	 * @param LocalPaths			Specify the Path where you want to upload the file.
	 * @Return						Returns true if the request succeeds
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|MultpleAction")
	static bool PutObjectsFromLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const FString &URL, const FString &LocalPaths);
	
	/**
	 * Download multiple data to local .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					Need domain name .
	 * @param SavePaths				Path to local storage .
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|MultpleAction")
	static void GetObjectsToLocal(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL, const FString &SavePaths);
	
	/**
	 * The data can be downloaded to local memory via the HTTP serverll.
	 * Can download multiple at one time .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					Need domain name .
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|MultpleAction")
	static void GetObjectsToMemory(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL);
	
	/**
	 * Multiple URLs need to be specified to remove multiple objects from the server .
	 *
	 * @param BPResponseDelegate	Proxy set relative to the blueprint.
	 * @param URL					Need domain name .
	 */
	UFUNCTION(BlueprintCallable, Category = "ReHTTP|MultpleAction")
	static void DeleteObjects(const FReHTTPBPResponseDelegate &BPResponseDelegate, const TArray<FString> &URL);

public:
};
