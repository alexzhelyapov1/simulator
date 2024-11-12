#include "machine.h"
#include "hart.h"
#include "loader.h"
#include <memory>

namespace Machine {

std::shared_ptr<Hart> Machine::CreateHart() {
    auto newHart = std::make_shared<Hart>(shared_from_this());
    harts.push_back(newHart);
    return newHart;
}

std::shared_ptr<Hart> Machine::CreateHart(const RegValue &PC) {
    auto newHart = std::make_shared<Hart>(shared_from_this(), PC);
    harts.push_back(newHart);
    return newHart;
}

} // namespace Machine