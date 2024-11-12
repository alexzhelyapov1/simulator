#include "machine.h"
#include "hart.h"
#include <memory>
#include "loader.h"

namespace Machine {

std::shared_ptr<Hart> Machine::CreateHart() {
  auto newHart = std::make_shared<Hart>(shared_from_this());
  harts.push_back(newHart);
  return newHart;
}

std::shared_ptr<Hart> Machine::CreateHart(const RegValue& PC) {
  auto newHart = std::make_shared<Hart>(shared_from_this(), PC);
  harts.push_back(newHart);
  return newHart;
}

std::shared_ptr<Loader::Loader> Machine::CreateLoader() {
    loader = std::make_shared<Loader::Loader>(shared_from_this());
    return loader;
}

} // namespace Machine