#include "hart.h"

namespace Machine
{

RegValue Hart::getReg(const RegId &reg) 
{ 
  return Regfile[reg];
}

void Hart::setReg(const RegId &reg, const RegValue &val) 
{
  if (reg == 0) {
    return;
  }
  Regfile[reg] = val;
}

std::shared_ptr<Instr> Hart::Decode(const Word& instrCode) 
{
  auto inst = instCache->get(instrCode);
  if(inst != nullptr) {
      return inst;
  }
  inst = std::make_shared<Instr>(instrCode);
  instCache->put(inst);
  return inst;
}

void Hart::SetPC(const RegValue &pc)
{
  PC = pc;
}

const RegValue &Hart::GetPC()
{
  return PC;
}

void Hart::RunSimpleInterpreterWithInstCache()
{

}

template <typename ValType>
ValType Hart::loadMem(RegValue address)
{
  auto hostAddress = MMU(address);
  machine.lock()->loadMem<ValType>(hostAddress);
}

template <typename ValType>
void Hart::storeMem(RegValue address, ValType val)
{
  auto hostAddress = MMU(address);
  machine.lock()->storeMem(hostAddress, val);
}

inline const RegValue &Hart::MMU(RegValue vaddress)
{
  //TODO insert MMU here
  return std::move(vaddress);
}

}