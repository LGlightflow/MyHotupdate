// 
#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

//Synchronous asynchronous thread, which is easy to use 
struct RETHREAD_API FAbandonable :FNonAbandonableTask
{
	FAbandonable(const FSimpleDelegate &InThreadDelegate);

	//Where threads really execute logic 
	void DoWork();

	//ID
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FAbandonable, STATGROUP_ThreadPoolAsyncTasks);
	}
protected:

	//Bound events 
	FSimpleDelegate ThreadDelegate;
};