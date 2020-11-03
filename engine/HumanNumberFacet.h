#pragma once

#include <locale>

struct HumanNumberFacet : std::numpunct< char >
{
	char do_thousands_sep() const override
	{
		return ' ';
	}

	std::string do_grouping() const override
	{
		return "\3";
	}
};

