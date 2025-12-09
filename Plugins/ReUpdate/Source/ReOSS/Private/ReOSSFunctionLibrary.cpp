// Copyright (C) RenZhai.2020.All Rights Reserved.


#include "ReOSSFunctionLibrary.h"
#include "ReOSSManage.h"
#include "Async/TaskGraphInterfaces.h"
#include "ReOSSMacro.h"
#include "Misc/ScopeLock.h"

FCriticalSection OSSMutex;

FString UReOSSFunctionLibrary::GetEndpoint()
{
	return RE_OSS.GetEndpoint();
}

void UReOSSFunctionLibrary::InitAccounts(const FString &InAccessKeyId, const FString &InAccessKeySecret, const FString &InEndpoint)
{
	RE_OSS.InitAccounts(InAccessKeyId, InAccessKeySecret, InEndpoint);
}

void UReOSSFunctionLibrary::InitConf(const FClientConfiguration &InConf)
{
	RE_OSS.InitConf(InConf);
}

bool UReOSSFunctionLibrary::DoesBucketExist(const FString &InBucketName)
{
	return RE_OSS.DoesBucketExist(InBucketName);
}

bool UReOSSFunctionLibrary::CreateBucket(const FString &InBucketName, EOSSStorageType OSSStorageType /*= EOSSStorageType::IA*/, EAcl Acl /*= EAcl::PUBLICREAD*/)
{
	return RE_OSS.CreateBucket(InBucketName, OSSStorageType, Acl);
}

bool UReOSSFunctionLibrary::ListBuckets(TArray<FString> &InBucketName)
{
	return RE_OSS.ListBuckets(InBucketName);
}

bool UReOSSFunctionLibrary::GetBucketsInfo(const FString &InBucketName, FBucketInfo &InBucketInfo)
{
	return RE_OSS.GetBucketsInfo(InBucketName, InBucketInfo);
}

FString UReOSSFunctionLibrary::GetBucketLocation(const FString &InBucketName)
{
	return RE_OSS.GetBucketLocation(InBucketName);
}

//int64 UReOSSFunctionLibrary::GetBucketsStorageCapacity(const FString &InBucketName)
//{
//	return RE_OSS.GetBucketsStorageCapacity(InBucketName);
//}

bool UReOSSFunctionLibrary::DeleteBucket(const FString &InBucketName)
{
	return RE_OSS.DeleteBucket(InBucketName);
}

bool UReOSSFunctionLibrary::SetBucketAcl(const FString &InBucketName, EAcl BucketAcl)
{
	return RE_OSS.SetBucketAcl(InBucketName, BucketAcl);
}

EAcl UReOSSFunctionLibrary::GetBucketAcl(const FString InBucketName)
{
	return RE_OSS.GetBucketAcl(InBucketName);
}

bool UReOSSFunctionLibrary::SetBucketReferer(const FString &InBucketName, const TArray<FString> &URLReferer, bool bAllowEmptyRefere /*= false*/)
{
	return RE_OSS.SetBucketReferer(InBucketName, URLReferer, bAllowEmptyRefere);
}

bool UReOSSFunctionLibrary::GetBucketReferer(const FString &InBucketName, TArray<FString> &BucketReferers)
{
	return RE_OSS.GetBucketReferer(InBucketName, BucketReferers);
}

bool UReOSSFunctionLibrary::DeleteBucketReferer(const FString &InBucketName, bool bAllowEmptyRefere /*= true*/)
{
	return RE_OSS.DeleteBucketReferer(InBucketName, bAllowEmptyRefere);
}

bool UReOSSFunctionLibrary::GetObjectMeta(const FString &InBucketName, const FString &InObjectName, FOSSObjectMeta &OSSObjectInfo)
{
	return RE_OSS.GetObjectMeta(InBucketName, InObjectName, OSSObjectInfo);
}

EAcl UReOSSFunctionLibrary::GetObjectAcl(const FString &InBucketName, const FString &InObjectName)
{
	return RE_OSS.GetObjectAcl(InBucketName, InObjectName);
}

bool UReOSSFunctionLibrary::SetObjectAcl(const FString &InBucketName, const FString &InObjectName, EAcl InAcl)
{
	return RE_OSS.SetObjectAcl(InBucketName, InObjectName, InAcl);
}

bool UReOSSFunctionLibrary::DoesObjectExist(const FString &InBucketName, const FString &InObjectName)
{
	return RE_OSS.DoesObjectExist(InBucketName, InObjectName);
}

bool UReOSSFunctionLibrary::ListObjects(const FString &InBucketName, TArray<FOSSObjectInfo> &ObjectName, int32 MaxNumber /*= 100*/)
{
	return RE_OSS.ListObjects(InBucketName, ObjectName, MaxNumber);
}

bool UReOSSFunctionLibrary::ListObjectsByFilePaths(const FString &InBucketName, const FString &ServerFileRelativePaths, TArray<FOSSObjectInfo> &ObjectName, int32 MaxNumber /*= 100*/)
{
	return RE_OSS.ListObjectsByPrefix(InBucketName, ServerFileRelativePaths,ObjectName, MaxNumber);
}

bool UReOSSFunctionLibrary::RecursiveListObjectsByFilePaths(const FString &InBucketName, const FString &ServerFileRelativePaths, TArray<FOSSObjectInfo> &ObjectName, int32 MaxNumber /*= 100*/)
{
	return RE_OSS.RecursiveListObjectsByPrefix(InBucketName, ServerFileRelativePaths, ObjectName, MaxNumber);
}
//
//bool UReOSSFunctionLibrary::SetObjectStorageType(const FString &InBucketName, const FString &InObjectName, EOSSStorageType OSSStorageType)
//{
//	return RE_OSS.SetObjectStorageType(InBucketName, InObjectName, OSSStorageType);
//}
//
//EOSSStorageType UReOSSFunctionLibrary::GetObjectStorageType(const FString &InBucketName, const FString &InObjectName)
//{
//	return RE_OSS.GetObjectStorageType(InBucketName, InObjectName);
//}

bool UReOSSFunctionLibrary::CreateSymlink(const FString &InBucketName, const FString &InObjectName, const FString &InLinkObjectName)
{
	return RE_OSS.CreateSymlink(InBucketName, InObjectName, InLinkObjectName);
}

FString UReOSSFunctionLibrary::GetSymlink(const FString &InBucketName, const FString &InLinkName)
{
	return RE_OSS.GetSymlink(InBucketName, InLinkName);
}

bool UReOSSFunctionLibrary::GetObjectToLocal(const FString& InBucketName, const FString& InObjectName, const FString& InLocalPaths, const FRange& Range)
{
	return RE_OSS.GetObjectToLocal(InBucketName, InObjectName, InLocalPaths, Range, nullptr);
}

bool UReOSSFunctionLibrary::GetObjectToLocalByURL(const FString& URL, const FString& InLocalPaths, FRange Range)
{
	return RE_OSS.GetObjectToLocalByURL(URL, InLocalPaths, Range);
}

bool UReOSSFunctionLibrary::GetImageObjectByResize(const FVector2D &Size, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectByResize(Size, InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageObjectByCrop(const FVector4 &Size, int32 Ratio, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectByCrop(Size, Ratio,InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageObjectBySharpen(const int32 Sharpen, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectBySharpen(Sharpen, InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageObjectByWaterMark(const FString WaterMarkObjectName, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectByWaterMark(WaterMarkObjectName, InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageObjectByRotate(const int32 Rotate, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectByRotate(Rotate, InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageObjectByStyle(const FString OSSStyleName, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectByStyle(OSSStyleName, InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageObjectByCascade(const TArray<FString> &CascadeCommand, const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageObjectByCascade(CascadeCommand, InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::GetImageInfo(const FString &InBucketName, const FString &InObjectName, const FString &InFileNametoSave)
{
	return RE_OSS.GetImageInfo(InBucketName, InObjectName, InFileNametoSave);
}

bool UReOSSFunctionLibrary::AbortMultipartUpload(const FString &InBucketName, const FString &InObjectName, const FString &InUploadId)
{
	return RE_OSS.AbortMultipartUpload(InBucketName, InObjectName, InUploadId);
}

bool UReOSSFunctionLibrary::GeneratePresignedUrl(FString &URL, const FString &InBucketName, const FString &InObjectName)
{
	return RE_OSS.GeneratePresignedUrl(URL, InBucketName, InObjectName);
}

bool UReOSSFunctionLibrary::UploadPart(FString &InUploadId, const FString &InBucketName, const FString &InObjectName, const FString &InLocalPaths, int32 PartSize /*= 1024 * 1024 * 10*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	return RE_OSS.UploadPart(InUploadId, InBucketName, InObjectName, InLocalPaths, PartSize,OSSMeta);
}

bool UReOSSFunctionLibrary::PutByteObjectMemoryByURL(const FString& URL, const TArray<uint8>& Data, const TMap<FString, FString>& OSSMeta)
{
	return RE_OSS.PutObjectMemoryByURL(URL, Data, OSSMeta);
}

bool UReOSSFunctionLibrary::ResumableUploadObject(const FString &InBucketName, const FString &InObjectName, const FString &InUploadFilePath, int64 PartSize, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	return RE_OSS.ResumableUploadObject(InBucketName, InObjectName, InUploadFilePath, PartSize, nullptr, OSSMeta);
}

//bool UReOSSFunctionLibrary::AppendObject(const FString &InBucketName, const FString &InLocalPaths, const FString &InObjectName /*=""*/)
//{
//	return RE_OSS.AppendObject(InBucketName,InLocalPaths,InObjectName);
//}

bool UReOSSFunctionLibrary::PutObject(const FString &InBucketName, const FString &InLocalPaths, const FString &InObjectName /*= FString()*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	return RE_OSS.PutObject(InBucketName, InLocalPaths, InObjectName,nullptr,OSSMeta);
}

bool UReOSSFunctionLibrary::PutObjectByURL(const FString& URL, const FString& InLocalPaths, const TMap<FString, FString> &OSSMeta)
{
	return RE_OSS.PutObjectByURL(URL, InLocalPaths, OSSMeta);
}

bool UReOSSFunctionLibrary::PutStringObjectByMemory(const FString &InBucketName, const FString &InObjectName, const FString &Data, const TMap<FString, FString> &OSSMeta)
{
	return RE_OSS.PutObjectByMemory(InBucketName,InObjectName, Data,nullptr, OSSMeta);
}

bool UReOSSFunctionLibrary::PutByteObjectByMemory(const FString& InBucketName, const FString& InObjectName, const TArray<uint8>& Data, const TMap<FString, FString>& OSSMeta)
{
	return RE_OSS.PutObjectByMemory(InBucketName, InObjectName, Data, nullptr, OSSMeta);
}

bool UReOSSFunctionLibrary::PutStringObjectMemoryByURL(const FString& URL, const FString& Data, const TMap<FString, FString>& OSSMeta)
{
	return RE_OSS.PutObjectMemoryByURL(URL, Data, OSSMeta);
}

bool UReOSSFunctionLibrary::GetObjectStringToMemory(const FString& InBucketName, const FString& InObjectName, FString& ContextString, FRange Range)
{
	return RE_OSS.GetObjectToMemory(InBucketName, InObjectName, ContextString, Range, nullptr);
}

bool UReOSSFunctionLibrary::GetObjectByteToMemory(const FString& InBucketName, const FString& InObjectName, TArray<uint8>& OutData, FRange Range)
{
	return RE_OSS.GetObjectToMemory(InBucketName, InObjectName, OutData, Range, nullptr);
}

bool UReOSSFunctionLibrary::GetObjectStringToMemoryByURL(const FString& URL, FString& ContextString, FRange Range)
{
	return RE_OSS.GetObjectToMemoryByURL(URL, ContextString, Range);
}

bool UReOSSFunctionLibrary::GetObjectByteToMemoryByURL(const FString& URL, TArray<uint8>& OutBytes, FRange Range)
{
	return RE_OSS.GetObjectToMemoryByURL(URL, OutBytes, Range);
}

bool UReOSSFunctionLibrary::ResumableDownloadObject(const FString &InBucketName, const FString &InObjectName, const FString &InDownloadFilePath, int32 PartSize /*= 1024 * 1024 * 10*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}

	return RE_OSS.ResumableDownloadObject(InBucketName, InObjectName, InDownloadFilePath, PartSize, NULL);
}

bool UReOSSFunctionLibrary::CopyObject(const FString &InSourceBucketName, const FString &InSourceObjectName, const FString &InCopyBucketName, const FString &InCopyObjectName)
{
	return RE_OSS.CopyObject(InSourceBucketName, InSourceObjectName, InCopyBucketName, InCopyObjectName);
}
//
//bool UReOSSFunctionLibrary::CopyPart(FString &UploadID, const FString &InSourceBucketName, const FString &InSourceObjectName, const FString &InCopyBucketName, const FString &InCopyObjectName)
//{
//	return RE_OSS.CopyPart(UploadID, InSourceBucketName, InSourceObjectName, InCopyBucketName, InCopyObjectName);
//}

bool UReOSSFunctionLibrary::DeleteObject(const FString &InBucketName, const FString &InObjectName)
{
	return RE_OSS.DeleteObject(InBucketName, InObjectName);
}

bool UReOSSFunctionLibrary::DeleteObjectByPrefix(const FString &InBucketName, const FString &InPrefix)
{
	return RE_OSS.DeleteObjectByPrefix(InBucketName, InPrefix);
}

bool UReOSSFunctionLibrary::DeleteObjectByFilePath(const FString &InBucketName, const FString &InServerFilePath)
{
	return RE_OSS.DeleteObjectByPrefix(InBucketName, InServerFilePath);
}

bool UReOSSFunctionLibrary::DeleteObjects(const FString &InBucketName, const TArray<FString> &InObjectName)
{
	return RE_OSS.DeleteObjects(InBucketName, InObjectName);
}

bool UReOSSFunctionLibrary::ListMultipartUploads(const FString &InBucketName, TArray<FMultipartPartsUploadsInfo> &MultipartPartsInfo, int32 MaxParts /*= 1000*/)
{
	return RE_OSS.ListMultipartUploads(InBucketName, MultipartPartsInfo, MaxParts);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectToLocal)
void UReOSSFunctionLibrary::AsyncGetObjectToLocal(const FString &InBucketName, const FString& InObjectName, const FString& InLocalPaths, FRange Range, FProgressCallbackDelegate InProgressCallback)
{
	AsyncGetObjectToLocal_Delegate = InProgressCallback;
	RE_OSS.AsyncGetObjectToLocal(InBucketName, InObjectName, InLocalPaths, Range, AsyncGetObjectToLocal_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectToLocalByURL)
DECLARATION_END_CALLBACK_DELEGATE(AsyncGetObjectToLocalByURL)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectToLocalByURL)
void UReOSSFunctionLibrary::AsyncGetObjectToLocalByURL(const FString& URL, const FString& InLocalPaths, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectToLocalByURL_Delegate = InProgressCallback;
	AsyncGetObjectToLocalByURL_Start_Delegate = InStartCallback;
	AsyncGetObjectToLocalByURL_End_Delegate = InEndCallback;
	RE_OSS.AsyncGetObjectToLocalByURL(URL, InLocalPaths, Range, AsyncGetObjectToLocalByURL_Local, AsyncGetObjectToLocalByURL__Start_Delegate,AsyncGetObjectToLocalByURL__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectStringToMemory)
DECLARATION_END_CALLBACK_DELEGATE(AsyncGetObjectStringToMemory)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectStringToMemory)
void UReOSSFunctionLibrary::AsyncGetObjectStringToMemory(const FString& InBucketName, const FString& InObjectName, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectStringToMemory_Delegate = InProgressCallback;
	AsyncGetObjectStringToMemory_Start_Delegate = InStartCallback;
	AsyncGetObjectStringToMemory_End_Delegate = InEndCallback;
	RE_OSS.AsyncGetObjectToMemory(InBucketName, InObjectName, Range, AsyncGetObjectStringToMemory_Local, AsyncGetObjectStringToMemory__Start_Delegate, AsyncGetObjectStringToMemory__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectBytesToMemory)
DECLARATION_END_BYTE_CALLBACK_DELEGATE(AsyncGetObjectBytesToMemory)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectBytesToMemory)
void UReOSSFunctionLibrary::AsyncGetObjectBytesToMemory(const FString& InBucketName, const FString& InObjectName, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBytesBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectBytesToMemory_Delegate = InProgressCallback;
	AsyncGetObjectBytesToMemory_Start_Delegate = InStartCallback;
	AsyncGetObjectBytesToMemory_End_Delegate = InEndCallback;
	RE_OSS.AsyncGetObjectToMemory(InBucketName, InObjectName, Range, AsyncGetObjectBytesToMemory_Local, AsyncGetObjectBytesToMemory__Start_Delegate, AsyncGetObjectBytesToMemory__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectStringToMemoryByURL)
DECLARATION_END_CALLBACK_DELEGATE(AsyncGetObjectStringToMemoryByURL)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectStringToMemoryByURL)
void UReOSSFunctionLibrary::AsyncGetObjectStringToMemoryByURL(const FString& URL, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectStringToMemoryByURL_Delegate = InProgressCallback;
	AsyncGetObjectStringToMemoryByURL_End_Delegate = InEndCallback;
	AsyncGetObjectStringToMemoryByURL_Start_Delegate = InStartCallback;
	RE_OSS.AsyncGetObjectToMemoryByURL(URL, Range, AsyncGetObjectStringToMemoryByURL_Local, AsyncGetObjectStringToMemoryByURL__Start_Delegate, AsyncGetObjectStringToMemoryByURL__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncGetObjectByteToMemoryByURL)
DECLARATION_END_BYTE_CALLBACK_DELEGATE(AsyncGetObjectByteToMemoryByURL)
DECLARATION_START_CALLBACK_DELEGATE(AsyncGetObjectByteToMemoryByURL)
void UReOSSFunctionLibrary::AsyncGetObjectByteToMemoryByURL(const FString& URL, FRange Range, FProgressCallbackDelegate InProgressCallback, FBufferByOSSCallbackDelegate InStartCallback, FBytesBufferByOSSCallbackDelegate InEndCallback)
{
	AsyncGetObjectByteToMemoryByURL_Delegate = InProgressCallback;
	AsyncGetObjectByteToMemoryByURL_End_Delegate = InEndCallback;
	AsyncGetObjectByteToMemoryByURL_Start_Delegate = InStartCallback;
	RE_OSS.AsyncGetObjectToMemoryByURL(URL, Range, AsyncGetObjectByteToMemoryByURL_Local, AsyncGetObjectByteToMemoryByURL__Start_Delegate, AsyncGetObjectByteToMemoryByURL__End_Delegate);
}

DECLARATION_CALLBACK_DELEGATE(AsyncResumableDownloadObject)
void UReOSSFunctionLibrary::AsyncResumableDownloadObject(const FString &InBucketName, const FString &InObjectName, const FString &InDownloadFilePath, int32 PartSize,FProgressCallbackDelegate InProgressCallback)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 10;
	}
	AsyncResumableDownloadObject_Delegate = InProgressCallback;
	RE_OSS.AsyncResumableDownloadObject(InBucketName, InObjectName, InDownloadFilePath, PartSize, AsyncResumableDownloadObject_Local);
}

DECLARATION_CALLBACK_DELEGATE_PART(AsyncUploadPart)
FString UReOSSFunctionLibrary::AsyncUploadPart(const FString &InBucketName, const FString &InObjectName, const FString &InLocalPaths, int32 PartSize, FCallbackUploadPartDelegate InProgressCallback, const TMap<FString, FString> &OSSMeta)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	AsyncUploadPart_Delegate = InProgressCallback;
	return RE_OSS.AsyncUploadPart(InBucketName, InObjectName, InLocalPaths, PartSize, AsyncUploadPart_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObject)
void UReOSSFunctionLibrary::AsyncPutObject(const FString &InBucketName,  const FString &InObjectName /*= FString()*/,const FString &InLocalPaths, FProgressCallbackDelegate InProgressCallback /*= nullptr*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	AsyncPutObject_Delegate = InProgressCallback;
	RE_OSS.AsyncPutObject(InBucketName, InLocalPaths,InObjectName, AsyncPutObject_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectByURL)
void UReOSSFunctionLibrary::AsyncPutObjectByURL(const FString& URL, const FString& InLocalPaths, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString> &OSSMeta)
{
	AsyncPutObjectByURL_Delegate = InProgressCallback;
	RE_OSS.AsyncPutObjectByURL(URL, InLocalPaths, OSSMeta, AsyncPutObjectByURL_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectStringByMemory)
void UReOSSFunctionLibrary::AsyncPutObjectStringByMemory(const FString& InBucketName, const FString& InObjectName, const FString& Data, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString>& OSSMeta)
{
	AsyncPutObjectStringByMemory_Delegate = InProgressCallback;
	RE_OSS.AsyncPutObjectByMemory(InBucketName, InObjectName, Data, AsyncPutObjectStringByMemory_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectBytesByMemory)
void UReOSSFunctionLibrary::AsyncPutObjectBytesByMemory(const FString& InBucketName, const FString& InObjectName, const TArray<uint8>& Data, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString>& OSSMeta)
{
	AsyncPutObjectBytesByMemory_Delegate = InProgressCallback;
	RE_OSS.AsyncPutObjectByMemory(InBucketName, InObjectName, Data, AsyncPutObjectBytesByMemory_Local, OSSMeta);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectStringMemoryByURL)
void UReOSSFunctionLibrary::AsyncPutObjectStringMemoryByURL(const FString& URL, const FString& Data, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString>& OSSMeta)
{
	AsyncPutObjectStringMemoryByURL_Delegate = InProgressCallback;
	RE_OSS.AsyncPutObjectMemoryByURL(URL, Data, OSSMeta, AsyncPutObjectStringMemoryByURL_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncPutObjectByteMemoryByURL)
void UReOSSFunctionLibrary::AsyncPutObjectByteMemoryByURL(const FString& URL, const TArray<uint8>& Data, FProgressCallbackDelegate InProgressCallback, const TMap<FString, FString>& OSSMeta)
{
	AsyncPutObjectByteMemoryByURL_Delegate = InProgressCallback;
	RE_OSS.AsyncPutObjectMemoryByURL(URL, Data, OSSMeta,AsyncPutObjectByteMemoryByURL_Local);
}

DECLARATION_CALLBACK_DELEGATE(AsyncResumableUploadObject)
void UReOSSFunctionLibrary::AsyncResumableUploadObject(const FString &InBucketName, const FString &InObjectName, const FString &InUploadFilePath, int64 PartSize, FProgressCallbackDelegate InProgressCallback /*= nullptr*/, const TMap<FString, FString> &OSSMeta /*= TMap<FString, FString>()*/)
{
	if (PartSize == 0)
	{
		PartSize = 1024 * 1024 * 10;
	}
	AsyncResumableUploadObject_Delegate = InProgressCallback;
	RE_OSS.AsyncResumableUploadObject(InBucketName, InObjectName, InUploadFilePath, PartSize,AsyncResumableUploadObject_Local, OSSMeta);
}
