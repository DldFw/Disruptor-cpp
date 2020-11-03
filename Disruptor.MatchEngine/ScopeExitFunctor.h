#pragma once

#include <functional>


namespace Disruptor
{
namespace UtilTools
{

    class ScopeExitFunctor
    {
    public:
        explicit ScopeExitFunctor(const std::function< void() >& func);

        ~ScopeExitFunctor();

    private:
        std::function< void() > m_func;
    };
    
} // namespace UtilTools
} // namespace Disruptor
