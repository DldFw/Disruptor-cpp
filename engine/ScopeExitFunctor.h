#pragma once
#include <functional>
class ScopeExitFunctor
{
	public:
		explicit ScopeExitFunctor(const std::function< void() >& func);

		~ScopeExitFunctor();

	private:
		std::function< void() > m_func;
};

