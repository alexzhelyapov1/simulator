#include "hart.h"
#include "machine.h"

namespace Machine {

RegValue Hart::getReg(const RegId &reg) { return Regfile[reg]; }

void Hart::setReg(const RegId &reg, const RegValue &val) {
  if (reg == 0) {
    return;
  }
  Regfile[reg] = val;
}

std::shared_ptr<Instr> Hart::decode(const Word &instrCode) {
  auto inst = instCache->get(instrCode);
  if (inst != nullptr) {
    return inst;
  }
  inst = std::make_shared<Instr>(instrCode);
  instCache->put(inst);
  return inst;
}

void Hart::setPC(const RegValue &pc) { PC = pc; }

const RegValue &Hart::getPC() { return PC; }

void Hart::RunSimpleInterpreterWithInstCache() {
  while (true) {
    auto instCode = loadMem<Word>(PC);
    auto inst = decode(instCode);
    inst->handler(*this, inst);
    PC += sizeof(Word);
  }
}

void Hart::exceptionReturn() { throw std::runtime_error("EXCEPTION RETURN FROM HART"); }

inline const RegValue &Hart::MMU(RegValue vaddress) {
  // TODO insert MMU here
  return std::move(vaddress);
}

} // namespace Machine