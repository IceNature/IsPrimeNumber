#include <CL/cl.h>
#include <string>
#include <set>


class FactorAlgorithm
{
public:
    virtual std::set<int> *GetFactor(unsigned int) = 0;
    virtual std::string CurrentMode() = 0;
    virtual ~FactorAlgorithm() {}
protected:
    std::set<int> *getFactorByZeroOrOne(unsigned int);
	std::set<int> *marshalResult(int*, int);
};

class OpenCLFactorAlgorithm : public FactorAlgorithm
{
public:
    OpenCLFactorAlgorithm();
    ~OpenCLFactorAlgorithm() override;
    std::set<int> *GetFactor(unsigned int) override;
    std::string CurrentMode() override
    {
        return "GPU - OpenCL";
    }
    bool Init();
private:
    cl_platform_id *platformIds;
    cl_uint numPlatforms;
    cl_platform_id clPlatform;
    cl_device_id clDevice;
    cl_context clContext;
    cl_program clProgram;
    cl_command_queue clCommandQueue;
    void init_Core();
    void createclContext();
    void createclProgram();
    void createclCommandQueue();
    static void checkErrorCode(cl_int, std::string);
    void getPlatforms();
    void chooseDevice();
    void createContext();
    void buildProgram();
    void createCommandQueue();
};

class MultiThreadFactorAlgorithm : public FactorAlgorithm
{
public:
    std::set<int> *GetFactor(unsigned int) override;
    std::string CurrentMode() override
    {
        return "CPU";
    }
};

FactorAlgorithm *GetSuitableAlgorithm();
