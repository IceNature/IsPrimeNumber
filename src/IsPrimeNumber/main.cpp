#include <iostream>
#include <algorithm>
#include <exception>
#include "core.h"

void showVersionInformation()
{
    std::cout << "素数检查工具   Ver. 1.0.1" << "\n"
              << "Create by IceNature" << std::endl;
}

void outputResult(FactorAlgorithm *currentAlgorithm)
{
    unsigned int number = 0;
    std::cout << "请输入一个数：" << std::flush;
    std::cin >> number;
    std::set<int> *result = currentAlgorithm->GetFactor(number);
    std::string numType;
    if (result->size() > 2)
    {
        numType = "和数";
    }
    else
    {
        numType = "素数";
    }
    std::cout << number << "是一个" << numType << ",因数有" << result->size() << "个：\n" << std::flush;
    std::copy(result->begin(), result->end(), std::ostream_iterator<int>(std::cout, "，"));
    std::cout << "\n" << std::endl;
    delete result;
}

int main()
{
    showVersionInformation();
    std::cout << "正在初始化..." << std::flush;
    FactorAlgorithm *currentAlgorithm = GetSuitableAlgorithm();
    std::cout << "完成！\n" << "当前模式：" << currentAlgorithm->CurrentMode() << std::endl;
    bool circulationControl = true;
    while (circulationControl)
    {
        try
        {
            outputResult(currentAlgorithm);
        }
        catch (std::exception ex)
        {
            std::cerr << ex.what() << "\n" << std::endl;
        }
        std::cout << "继续？（Y/N）" << std::flush;
        char type;
        std::cin >> type;
        circulationControl = (type == 'Y' || type == 'y');
    }
    delete currentAlgorithm;
    return 0;
}
