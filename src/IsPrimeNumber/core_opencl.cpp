#include <exception>
#include <fstream>
#include <sstream>
#include <iostream>
#include "core.h"



void OpenCLFactorAlgorithm::checkErrorCode(cl_int errNum, std::string message)
{
    if (errNum != CL_SUCCESS)
        throw std::runtime_error(message);
}

OpenCLFactorAlgorithm::OpenCLFactorAlgorithm() :
    platformIds(nullptr), numPlatforms(0), clDevice(0), clContext(0), clProgram(0), clCommandQueue(0) {}

OpenCLFactorAlgorithm::~OpenCLFactorAlgorithm()
{
    if (platformIds != NULL) delete[] platformIds;
    if (clContext != 0) clReleaseContext(clContext);
    if (clProgram != 0) clReleaseProgram(clProgram);
    if (clCommandQueue != 0) clReleaseCommandQueue(clCommandQueue);
}

void OpenCLFactorAlgorithm::getPlatforms()
{
    cl_int errNum;
    errNum = clGetPlatformIDs(0, NULL, &numPlatforms);
    checkErrorCode(errNum, "未找到支持OpenCL的设备！");
    if (numPlatforms == 0) throw std::runtime_error("未找到支持OpenCL的设备！");
    platformIds = new cl_platform_id[numPlatforms];
    errNum = clGetPlatformIDs(numPlatforms, platformIds, NULL);
    checkErrorCode(errNum, "未找到支持OpenCL的设备！");
}

void OpenCLFactorAlgorithm::chooseDevice()
{
    cl_int totalErrNum;
    cl_int errNum;
    for (int i = 0; i != numPlatforms; ++i)
    {
        cl_device_id *currentDevices;
        cl_uint numDevices;
        errNum = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_GPU, 0, NULL, &numDevices);
        if (errNum != CL_SUCCESS) continue;
        if (numDevices == 0) continue;
        currentDevices = new cl_device_id[numDevices];
        errNum = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_GPU, numDevices, currentDevices, NULL);
        if (errNum != CL_SUCCESS)
        {
            delete[] currentDevices;
            continue;
        }
        clDevice = currentDevices[0];
        clPlatform = platformIds[i];
        delete[] currentDevices;
        totalErrNum = CL_SUCCESS;
        break;
    }
    checkErrorCode(totalErrNum, "未找到支持OpenCL的设备！");
}

void OpenCLFactorAlgorithm::createContext()
{
    cl_int errNum;
    cl_context_properties properties[] =
    {
        CL_CONTEXT_PLATFORM, (cl_context_properties)clPlatform, 0
    };
    clContext = clCreateContext(properties, 1, &clDevice, NULL, NULL, &errNum);
    checkErrorCode(errNum, "OpenCL初始化失败！");
}

void OpenCLFactorAlgorithm::buildProgram()
{
    cl_int errNum;
#ifdef DEBUG
    std::ifstream kernelFile("D:/Gits/IsPrimeNumber/core.cl", std::ios::in);
#else
    std::ifstream kernelFile("core.cl", std::ios::in);
#endif // DEBUG
    if (!kernelFile.is_open())
        throw std::runtime_error("未找到内核文件！");
    std::ostringstream oss;
    oss << kernelFile.rdbuf();
    std::string srcStdStr = oss.str();
    const char *srcStr = srcStdStr.c_str();
    clProgram = clCreateProgramWithSource(clContext, 1, (const char **)&srcStr, NULL, &errNum);
    checkErrorCode(errNum, "OpenCL初始化失败！");
    errNum = clBuildProgram(clProgram, 0, NULL, NULL, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        char buildLog[16384];
        clGetProgramBuildInfo(clProgram, clDevice, CL_PROGRAM_BUILD_LOG, sizeof(buildLog), buildLog, NULL);
        std::string errLog(buildLog);
        throw std::runtime_error("内核构建失败：\n" + errLog);
    }
}

void OpenCLFactorAlgorithm::createCommandQueue()
{
    cl_int errNum;
    clCommandQueue = clCreateCommandQueue(clContext, clDevice, 0, &errNum);
    checkErrorCode(errNum, "OpenCL初始化失败！");
}

void OpenCLFactorAlgorithm::init_Core()
{
    getPlatforms();
    chooseDevice();
    createContext();
    buildProgram();
    createCommandQueue();
}

bool OpenCLFactorAlgorithm::Init()
{
    try
    {
        init_Core();
        return true;
    }
    catch (std::exception ex)
    {
        std::cout << "\n" << ex.what() << std::endl;
        return false;
    }
}

std::set<int> *OpenCLFactorAlgorithm::GetFactor(unsigned int number)
{
    std::set<int> *resultOutput = getFactorByZeroOrOne(number);
    if (resultOutput != nullptr)
        return resultOutput;
    unsigned int numberInput = number / 2;
    cl_int errNum;
    cl_kernel clKernel = clCreateKernel(clProgram, "getFactors", &errNum);
    checkErrorCode(errNum, "OpenCL初始化失败！");
    int *bufferResult = new int[numberInput];
    cl_mem buffer = clCreateBuffer(clContext, CL_MEM_WRITE_ONLY, sizeof(int) * numberInput, bufferResult, &errNum);
    if (errNum != CL_SUCCESS)
    {
        clReleaseKernel(clKernel);
        delete[] bufferResult;
        throw std::runtime_error("OpenCL初始化失败！");
    }
    errNum = clSetKernelArg(clKernel, 0, sizeof(cl_int), &number);
    errNum |= clSetKernelArg(clKernel, 1, sizeof(cl_mem), &buffer);
    if (errNum != CL_SUCCESS)
    {
        clReleaseMemObject(buffer);
        clReleaseKernel(clKernel);
        delete[] bufferResult;
        throw std::runtime_error("OpenCL初始化失败！");
    }
    size_t globeWorkSize[1] = { numberInput };
    size_t localWorkSize[1] = { 1 };
    errNum = clEnqueueNDRangeKernel(clCommandQueue, clKernel, 1, NULL, globeWorkSize, localWorkSize, 0, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        clReleaseMemObject(buffer);
        clReleaseKernel(clKernel);
        delete[] bufferResult;
        throw std::runtime_error("OpenCL初始化失败！");
    }
    int *result = new int[numberInput];
    errNum = clEnqueueReadBuffer(clCommandQueue, buffer, CL_TRUE, 0, sizeof(int) * numberInput, result, 0, NULL, NULL);
    if (errNum != CL_SUCCESS)
    {
        clReleaseMemObject(buffer);
        clReleaseKernel(clKernel);
        delete[] bufferResult;
        delete[] result;
        throw std::runtime_error("无法读取缓冲区！");
    }
    resultOutput = marshalResult(result, numberInput);
    clReleaseMemObject(buffer);
    clReleaseKernel(clKernel);
    delete[] bufferResult;
    delete[] result;
    return resultOutput;
}