#pragma once

#include <cstdint>
#include <functional>
#include <memory>

class FizzBuzzEvent
{
public:
    void reset();

    static const std::function< FizzBuzzEvent() >& eventFactory();

    std::int64_t value = 0;
    bool fizz = false;
    bool buzz = false;
};


