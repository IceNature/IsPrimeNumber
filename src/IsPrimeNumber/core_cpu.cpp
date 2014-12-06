#include "core.h"
#include <thread>

void subThreadAlgorithm(int *result, unsigned int number, int begin, int end)
{
	for (int i = begin; i < end; ++i)
	{
		if (number % (i + 1) == 0)
			result[i] = number / (i + 1);
		else
			result[i] = 0;
	}
}

std::set<int> *MultiThreadFactorAlgorithm::GetFactor(unsigned int number)
{
	std::set<int> *resultOutput = getFactorByZeroOrOne(number);
	if (resultOutput != nullptr)
		return resultOutput;
	unsigned int numberInput = number / 2;
	int *result = new int[numberInput];
	std::thread firstThread(subThreadAlgorithm, result, number, 0, numberInput / 2);
	std::thread secondThread(subThreadAlgorithm, result, number, numberInput / 2, numberInput);
	firstThread.join();
	secondThread.join();
	resultOutput = marshalResult(result, numberInput);
	delete[] result;
	return resultOutput;
}


