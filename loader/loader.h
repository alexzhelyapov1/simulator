#ifndef MODULE
    #define MODULE "Loader"
    #include "logging.h"
#endif
#include "machine.h"
#include <iostream>
#include <memory>

#ifndef LOADER_H
#define LOADER_H

namespace Loader {

class Loader {
  private:
    std::weak_ptr<Machine::Machine> machine;

  public:
    Loader(std::shared_ptr<Machine::Machine> machine) : machine(machine) {}
    ~Loader() {}

    uint64_t loadElf(const std::string &path, std::shared_ptr<Machine::Hart> &hart) const;
};

} // namespace Loader

#endif