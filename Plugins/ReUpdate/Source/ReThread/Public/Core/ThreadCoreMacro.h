
#pragma once
#include "Misc/ScopeLock.h"
#include "Abandonable/Abandonable.h"
#include "Async/TaskGraphInterfaces.h"

DEFINE_LOG_CATEGORY_STATIC(ThreadLOG, Log, All);

#define MUTEX_LOCL FScopeLock ScopeLock(&Mutex) 

#define USE_UE_THREAD_POOL_ASYNCTASK(ThreadDelegate) \
(new FAutoDeleteAsyncTask<FAbandonable>(ThreadDelegate))->StartBackgroundTask()

#define ASYNCTASK_UOBJECT(Object,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FDelegate::CreateUObject(Object,##__VA_ARGS__))

#define ASYNCTASK_Raw(Object,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FDelegate::CreateRaw(Object,##__VA_ARGS__))

#define ASYNCTASK_SP(Object,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FDelegate::CreateSP(Object,##__VA_ARGS__))

#define ASYNCTASK_Lambda(...) \
USE_UE_THREAD_POOL_ASYNCTASK(FDelegate::CreateLambda(##__VA_ARGS__))

#define ASYNCTASK_UFunction(Object,...) \
USE_UE_THREAD_POOL_ASYNCTASK(FDelegate::CreateUFunction(Object,##__VA_ARGS__))

#define USE_UE_THREAD_POOL_SYNCTASK(ThreadDelegate) \
{FAsyncTask<FAbandonable> *Abandonable = new FAsyncTask<FAbandonable>(ThreadDelegate); \
Abandonable->StartBackgroundTask(); \
Abandonable->EnsureCompletion(); \
delete Abandonable; }

#define SYNCTASK_UOBJECT(Object,...) \
USE_UE_THREAD_POOL_SYNCTASK(FDelegate::CreateUObject(Object,##__VA_ARGS__))

#define SYNCTASK_Raw(Object,...) \
USE_UE_THREAD_POOL_SYNCTASK(FDelegate::CreateRaw(Object,##__VA_ARGS__))

#define SYNCTASK_SP(Object,...) \
USE_UE_THREAD_POOL_SYNCTASK(FDelegate::CreateSP(Object,##__VA_ARGS__))

#define SYNCTASK_Lambda(...) \
USE_UE_THREAD_POOL_SYNCTASK(FDelegate::CreateLambda(##__VA_ARGS__))

#define SYNCTASK_UFunction(Object,...) \
USE_UE_THREAD_POOL_SYNCTASK(FDelegate::CreateUFunction(Object,##__VA_ARGS__))

//The calling thread can set the pre task 
#define CALL_THREAD(OtherTask,CallThreadName,InTaskDeletegate) \
FDelegateGraphTask::CreateAndDispatchWhenReady(InTaskDeletegate,TStatId(),OtherTask,CallThreadName);

#define CALL_THREAD_UOBJECT(OtherTask,CallThreadName,Object,...) \
CALL_THREAD(OtherTask,CallThreadName,FDelegate::CreateUObject(Object,##__VA_ARGS__))

#define CALL_THREAD_Raw(OtherTask,CallThreadName,Object,...) \
CALL_THREAD(OtherTask,CallThreadName,FDelegate::CreateRaw(Object,##__VA_ARGS__))

#define CALL_THREAD_SP(OtherTask,CallThreadName,Object,...) \
CALL_THREAD(OtherTask,CallThreadName,FDelegate::CreateSP(Object,##__VA_ARGS__))

#define CALL_THREAD_Lambda(OtherTask,CallThreadName,...) \
CALL_THREAD(OtherTask,CallThreadName,FDelegate::CreateLambda(##__VA_ARGS__))

#define CALL_THREAD_UFunction(OtherTask,CallThreadName,Object,...) \
CALL_THREAD(OtherTask,CallThreadName,FDelegate::CreateUFunction(Object,##__VA_ARGS__))


#define WAITING_OTHER_THREADS_COMPLETED(EventRef) \
FTaskGraphInterface::Get().WaitUntilTaskCompletes(EventRef);

#define WAITING_OTHER_THREADS_COMPLETED_MULTI(EventRef) \
FTaskGraphInterface::Get().WaitUntilTasksComplete(EventRef);

#define _THREAD_INFO_MSG(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_CYAN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(ThreadLOG, Log, TEXT("%s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define _THREAD_INFO_MSG_WARNING(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_CYAN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(ThreadLOG, Error, TEXT("%s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}

#define _THREAD_INFO_MSG_ERROR(Format, ...) \
{ \
	SET_WARN_COLOR(COLOR_CYAN); \
	const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
	UE_LOG(ThreadLOG, Error, TEXT("%s"), *Msg); \
	CLEAR_WARN_COLOR(); \
}