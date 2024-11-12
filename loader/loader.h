#include <memory>
#include <iostream>
#include <memory>
#include "machine.h"

#ifndef LOADER_H
#define LOADER_H

namespace Loader {

class Loader
{
private:
    std::weak_ptr<Machine::Machine> machine;

public:
    Loader(std::shared_ptr<Machine::Machine> machine) : machine(machine) {}
    ~Loader() {}

    void loadElf(const std::string &path) const;
};


}

#endif