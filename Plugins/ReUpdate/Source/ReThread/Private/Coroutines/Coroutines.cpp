// 

#include "Coroutines/Coroutines.h"

ICoroutinesObject::ICoroutinesObject()
	:bAwaken(false)
{

}

TArray<TSharedPtr<ICoroutinesObject>> ICoroutinesObject::Array;

FCoroutinesObject::FCoroutinesObject(float InTotalTime, const FSimpleDelegate &InDelegate)
	:ICoroutinesObject()
	,SimpleDelegate(InDelegate)
	,TotalTime(InTotalTime)
	,RuningTime(0.f)
{

}

FCoroutinesObject::FCoroutinesObject(const FOnGoingDelegate& InDelegate)
	:ICoroutinesObject()
	, OnGoingDelegate(InDelegate)
	, TotalTime(INDEX_NONE)
	, RuningTime(0.f)
{
}

FCoroutinesObject::FCoroutinesObject(float InTotalTime, const FOnGoingDelegate& InDelegate)
	:ICoroutinesObject()
	, OnGoingDelegate(InDelegate)
	, TotalTime(InTotalTime)
	, RuningTime(0.f)
{
}

FCoroutinesObject::FCoroutinesObject(const FSimpleDelegate &InDelegate)
	:ICoroutinesObject()
	, SimpleDelegate(InDelegate)
	, TotalTime(INDEX_NONE)
	, RuningTime(0.f)
{

}

void FCoroutinesObject::Update(FCoroutinesRequest &CoroutinesRequest)
{
	if (TotalTime != INDEX_NONE)
	{
		RuningTime += CoroutinesRequest.IntervalTime;

		//计算OnGoing时间
		if (OnGoingDelegate.IsBound())
		{
			if (TotalTime != 0.f)
			{
				float Ratio = RuningTime / TotalTime;
				OnGoingDelegate.ExecuteIfBound(Ratio, CoroutinesRequest.IntervalTime);
			}
		}
		
		if (RuningTime >= TotalTime)
		{
			SimpleDelegate.ExecuteIfBound();
			CoroutinesRequest.bCompleteRequest = true;
		}
	}
	else
	{
		if (bAwaken)
		{
			SimpleDelegate.ExecuteIfBound();
			CoroutinesRequest.bCompleteRequest = true;
		}
	}
}

FCoroutinesRequest::FCoroutinesRequest(float InIntervalTime)
	:bCompleteRequest(false)
	,IntervalTime(InIntervalTime)
{

}
