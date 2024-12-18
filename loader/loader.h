#ifndef MODULE
    #define MODULE "Loader"
    #include "logging.h"
#endif
#include "machine.h"
#include <iostream>
#include <memory>

#ifndef LOADER_H
#define LOADER_H

namespace Simulator {
    class Simulator;
}

namespace Loader {

class Loader {
  private:
    std::weak_ptr<Machine::Machine> machine;
    const Simulator::Simulator *simulator;

  public:
    Loader(std::shared_ptr<Machine::Machine> machine, const Simulator::Simulator *simulator):
        machine(machine), simulator(simulator) {}
    ~Loader() {}

    uint64_t loadElf(const std::string &path, Machine::Hart *hart) const;
};

} // namespace Loader

#endif