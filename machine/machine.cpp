#include "machine.h"
#include "hart.h"
#include "loader.h"
#include <memory>

namespace Machine {

std::shared_ptr<Hart> Machine::CreateHart() {
    auto newHart = std::make_shared<Hart>(*this);
    newHart->special_regs[0] = reinterpret_cast<RegValue>(&page_table);
    newHart->special_regs[1] = static_cast<RegValue>(MMUMode::BASE_MODE);
    harts.push_back(newHart);
    return newHart;
}

std::shared_ptr<Hart> Machine::CreateHart(const RegValue &PC) {
    auto newHart = std::make_shared<Hart>(*this, PC);
    newHart->special_regs[0] = reinterpret_cast<RegValue>(&page_table);
    newHart->special_regs[1] = static_cast<RegValue>(MMUMode::BASE_MODE);
    harts.push_back(newHart);
    return newHart;
}

} // namespace Machine