#include "stdafx.h"
#include "FunctionEvent.h"


const std::function< FunctionEvent() >& FunctionEvent::eventFactory()
{
	static std::function< FunctionEvent() > result([] { return FunctionEvent(); });
	return result;
}

