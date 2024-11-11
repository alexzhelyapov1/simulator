#include "machine.h"
#include "hart.h"
#include <memory>

namespace Machine 
{

std::shared_ptr<Hart> Machine::CreateHart() {
  auto newHart = std::make_shared<Hart>(shared_from_this());
  harts.push_back(newHart);
  return newHart;
}

template <typename ValType>
ValType Machine::loadMem(RegValue address)
{
  return mem->loadMem<ValType>(address);
}

template <typename ValType>
void Machine::storeMem(RegValue address, ValType val)
{
  return mem->storeMem(address, val);
}

}