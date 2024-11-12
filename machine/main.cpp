#include "intBitCache.h"
#include "machine.h"
#include <memory>
#include "loader.h"

class myInt {
public:
  int val;
  myInt() : val(-1){};
  myInt(int val) : val(val) {}
  size_t cacheId() { return val; }
};

int main() {
  auto machine = std::make_shared<Machine::Machine>(0xFFFFFFF);
  auto hart = machine->CreateHart();
  auto loader = machine->CreateLoader();
  loader->loadElf("/home/alex/mipt/simulator/build/a.out");

  auto cache = std::make_shared<IntBitCache<myInt, 2, 0>>();
  cache->put(std::make_shared<myInt>(3));
  auto val3 = cache->get(3);
  std::cout << "val: " << val3->cacheId() << std::endl;
  cache->put(std::make_shared<myInt>(7));
  std::cout << "val: " << cache->get(7)->cacheId() << std::endl;
  cache->put(std::make_shared<myInt>(5));
  std::cout << "val: " << cache->get(5)->cacheId() << std::endl;
  if (cache->get(3) == nullptr) {
    std::cout << "val \"7\" rewrites \"3\"" << std::endl;
  }
  std::cout << "val: " << val3->cacheId() << std::endl;
}