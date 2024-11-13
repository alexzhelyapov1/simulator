#include "simulator.h"

int main(int argc, char **argv)
{
    if(argc < 2)
    {
        return 1;
    }

    auto simulator = std::make_shared<Simulator::Simulator>(0xFFFFFFFF);
    simulator->StartSimulationOnSimpleInterpreter(argv[1]);

}