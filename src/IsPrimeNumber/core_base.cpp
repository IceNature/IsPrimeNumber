#include "core.h"
#include <exception>

std::set<int> *FactorAlgorithm::getFactorByZeroOrOne(unsigned int number)
{
    if (number == 1)
        return new std::set<int> {1};
    else if (number == 0)
        throw std::runtime_error("输入的数必须大于0！");
    else
        return nullptr;
}

std::set<int> *FactorAlgorithm::marshalResult(int *result, int length)
{
	std::set<int> *resultReturn = new std::set<int>();
	for(int i =0; i < length; ++i)
	{
		if(result[i])
		{
			resultReturn->insert(i + 1);
			resultReturn->insert(result[i]);
		}
	}
	return resultReturn;
}
