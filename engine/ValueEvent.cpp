#include "stdafx.h"
#include "ValueEvent.h"

const std::function< ValueEvent() >& ValueEvent::eventFactory()
{
	static std::function< ValueEvent() > result([] { return ValueEvent(); });
	return result;
}

