#include "machine.h"
#include "hart.h"
#include "loader.h"
#include <memory>

namespace Machine {

std::shared_ptr<Hart> Machine::CreateHart(const Simulator::Simulator *simulator) {
    auto newHart = std::make_shared<Hart>(*this, simulator);
    harts.push_back(newHart);
    return newHart;
}

std::shared_ptr<Hart> Machine::CreateHart(const RegValue &PC, const Simulator::Simulator *simulator) {
    auto newHart = std::make_shared<Hart>(*this, PC, simulator);
    harts.push_back(newHart);
    return newHart;
}

} // namespace Machine