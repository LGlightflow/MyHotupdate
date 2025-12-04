// 
#pragma once

#include "CoreMinimal.h"
#include "Core/ReThreadType.h"

//Request for process 
struct RETHREAD_API FCoroutinesRequest
{
	FCoroutinesRequest(float InIntervalTime);

	//Complete request or not 
	bool bCompleteRequest;

	//Time interval of each frame 
	float IntervalTime;
};

//Program interface object 
class RETHREAD_API ICoroutinesObject :public TSharedFromThis<ICoroutinesObject>
{
	friend class ICoroutinesContainer;
public:
	ICoroutinesObject();
	virtual ~ICoroutinesObject(){}

	bool operator==(const ICoroutinesObject& ThreadHandle)
	{
		return this->Handle == ThreadHandle.Handle;
	}

	//Wakeup process 
	FORCEINLINE void Awaken() { bAwaken = true; }
protected:

	virtual void Update(FCoroutinesRequest &CoroutinesRequest) = 0;

protected:

	//Process object container, which stores process objects 
	static TArray<TSharedPtr<ICoroutinesObject>> Array;
	uint8 bAwaken : 1;
	FThreadHandle Handle;
};

//Process handle 
typedef TWeakPtr<ICoroutinesObject> FCoroutinesHandle;

//Process object 
class RETHREAD_API FCoroutinesObject :public ICoroutinesObject
{
public:
	FCoroutinesObject(const FSimpleDelegate &InDelegate);
	FCoroutinesObject(float InTotalTime,const FSimpleDelegate &InDelegate);

	FCoroutinesObject(const FOnGoingDelegate& InDelegate);
	FCoroutinesObject(float InTotalTime, const FOnGoingDelegate& InDelegate);

	//Once registered, every frame will be updated 
	virtual void Update(FCoroutinesRequest &CoroutinesRequest) final;
private:

	//Agent required for registration 
	FSimpleDelegate SimpleDelegate;

	//Agent required for registration 
	FOnGoingDelegate OnGoingDelegate;

	//Total waiting time 
	const float TotalTime;

	//Current time, mainly used to record time 
	float RuningTime;
};