
#pragma once
#include "CoreMinimal.h"

//Mainly used as guid 
struct RETHREAD_API FReThreadHandle : public TSharedFromThis<FReThreadHandle>
{
	FReThreadHandle();

	bool operator==(const FReThreadHandle& ReThreadHandle)
	{
		return
			this->GUIDTypeA == ReThreadHandle.GUIDTypeA &&
			this->GUIDTypeB == ReThreadHandle.GUIDTypeB &&
			this->GUIDTypeC == ReThreadHandle.GUIDTypeC &&
			this->GUIDTypeD == ReThreadHandle.GUIDTypeD;
	}

protected:
	uint64 GUIDTypeA;
	uint64 GUIDTypeB;
	uint64 GUIDTypeC;
	uint64 GUIDTypeD;
};

//Thread state 
enum class EThreadState :uint8
{
	THREAD_LEISURELY,	
	THREAD_WORKING,	
	THREAD_ERROR,		
};

typedef TWeakPtr<FReThreadHandle> FThreadHandle;
typedef TFunction<void()> FThreadLambda;

DECLARE_DELEGATE_TwoParams(FOnGoingDelegate, float, float);