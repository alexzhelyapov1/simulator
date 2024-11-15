#include "intBitCache.h"
#include "loader.h"
#include "machine.h"
#include <memory>

class myInt {
  public:
    int val;
    myInt() : val(-1){};
    myInt(int val) : val(val) {}
    operator int() const { return val; }
};

int main() {
    auto machine = std::make_shared<Machine::Machine>(0xFFFFFFF);
    auto hart = machine->CreateHart();

    auto cache = std::make_shared<IntBitCache<myInt, 2, 0>>();
    cache->put(std::make_shared<myInt>(3));
    auto val3 = cache->get(3);
    std::cout << "val: " << *val3 << std::endl;
    cache->put(std::make_shared<myInt>(7));
    std::cout << "val: " << *(cache->get(7)) << std::endl;
    cache->put(std::make_shared<myInt>(5));
    std::cout << "val: " << *(cache->get(5)) << std::endl;
    if (cache->get(3) == nullptr) {
        std::cout << "val \"7\" rewrites \"3\"" << std::endl;
    }
    std::cout << "val: " << *(val3) << std::endl;
}