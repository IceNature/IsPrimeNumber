#include "core.h"

FactorAlgorithm *GetSuitableAlgorithm()
{
    OpenCLFactorAlgorithm *openCLAlgorithm = new OpenCLFactorAlgorithm();
    if (openCLAlgorithm->Init())
        return openCLAlgorithm;
    else
        delete openCLAlgorithm;
    return new MultiThreadFactorAlgorithm;
}
