#include "simulator.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        return 1;
    }

    auto simulator = std::make_shared<Simulator::Simulator>(0xFFFFFFFF);
    if(argc < 3)
    {
        simulator->StartSimulationOnSimpleInterpreter(argv[1]);
        return 0;
    }
    simulator->StartSimulationOnSimpleInterpreter(argv[1], argv[2]);

}