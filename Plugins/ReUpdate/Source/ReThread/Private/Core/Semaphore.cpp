// 

#include "Core/Semaphore.h"
#include "HAL/Event.h"
#include "ThreadPlatform.h"

FSemaphore::FSemaphore()
	:Event(FPlatformProcess::GetSynchEventFromPool())
	,bWait(false)
{

}

FSemaphore::~FSemaphore()
{
	FPlatformProcess::ReturnSynchEventToPool(Event);
}

void FSemaphore::Wait()
{
	bWait = true;
	Event->Wait();
}

void FSemaphore::Wait(uint32 WaitTime, const bool bIgnoreThreadIdleStats)
{
	bWait = true;
	Event->Wait(WaitTime, bIgnoreThreadIdleStats);
}

void FSemaphore::Trigger()
{
	Event->Trigger();
	bWait = false;
}