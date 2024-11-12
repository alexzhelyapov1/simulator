#include "hart.h"
#include "loader.h"
#include <memory>

namespace Machine
{
    class Machine;
    class Hart;
}

namespace Loader
{
    class Loader;
} // namespace Loader


namespace Simulator
{

class Simulator
{
private:
    std::shared_ptr<Machine::Machine> machine;
    std::shared_ptr<Loader::Loader> loader;
public:
    Simulator(Machine::RegValue memSize) 
    {
        machine = std::make_shared<Machine::Machine>(memSize);
        loader = std::make_shared<Loader::Loader>(machine);
    };

    void StartSimulationOnSimpleInterpreter(const std::string &filePath);
};

} // namespace Simulator