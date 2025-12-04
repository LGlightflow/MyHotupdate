// 
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "Interface/ProxyInterface.h"
#include "Core/Semaphore.h"

//UE4 RunnableThreads, you can create thread instances 
class RETHREAD_API FThreadRunnable : public FRunnable, public IThreadProxy
{
public:
	FThreadRunnable(bool InSuspendAtFirst = false);
	virtual ~FThreadRunnable();

	//Create a secure thread 
	virtual void CreateSafeThread();

	//Wake up thread 
	virtual void WakeupThread();

	//Whether the thread is suspended 
	virtual bool IsSuspend();

	//Let the startup thread wait until the current task is completed 
	virtual void WaitAndCompleted();

	//Block start thread 
	virtual void BlockingAndCompletion();

private:

	//Where threads actually execute 
	virtual uint32 Run();

	virtual bool Init();
	virtual void Stop();
	virtual void Exit();

private:

	
	FThreadSafeCounter					StopTaskCounter; //Thread safety count .
	bool								bSuspendAtFirst; //Whether to block the thread for the first time .
	class FRunnableThread*				Thread;			 //Thread specific instance 
	FName								RunnableName;	 //Thread name will be extended later 
	FSemaphore					ThreadEvent;     //Semaphore blocking thread 
	FSemaphore					StartUpEvent;	 //Semaphore block start thread 
	FSemaphore					WaitExecuteEvent;//Semaphore suspend start thread 
	FCriticalSection					Mutex;			 //lock 

	static int32						ThreadCount;	 //Thread sequence count 
};