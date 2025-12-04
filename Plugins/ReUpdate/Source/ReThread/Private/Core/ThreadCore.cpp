// 

#include "Core/ReThreadType.h"
#include "Interface/ProxyInterface.h"

uint64 SpawnUniqueID()
{
	return FMath::Abs(FMath::RandRange(100, 1256343) + FDateTime::Now().GetTicks());
}

FReThreadHandle::FReThreadHandle()
	:GUIDTypeA(SpawnUniqueID())
	,GUIDTypeB(SpawnUniqueID())
	,GUIDTypeC(SpawnUniqueID())
	,GUIDTypeD(SpawnUniqueID())
{

}

IThreadProxy::IThreadProxy()
	:ReThreadHandle(MakeShareable(new FReThreadHandle))
{

}