// 

#include "Abandonable/Abandonable.h"

FAbandonable::FAbandonable(const FSimpleDelegate &InThreadDelegate)
	:ThreadDelegate(InThreadDelegate)
{

}

void FAbandonable::DoWork()
{
	ThreadDelegate.ExecuteIfBound();
}