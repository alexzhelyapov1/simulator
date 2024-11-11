#include "intBitCache.h"
#include "machine.h"
#include <memory>

class myInt
{
public:
  int val;
  myInt() : val(-1) {};
  myInt(int val) : val(val) {}
  size_t cacheId() {return val;}
};

int main() {
  auto machine = std::make_shared<Machine::Machine>(1024000);
  auto hart = machine->CreateHart();

  auto cache = std::make_shared<IntBitCache<myInt, 2, 0>>();
  cache->put(std::make_shared<myInt>(3));
  auto val3 = cache->get(3);
  std::cout << "val: " << val3->cacheId() << std::endl;
  cache->put(std::make_shared<myInt>(7));
  std::cout << "val: " << cache->get(7)->cacheId() << std::endl;
  cache->put(std::make_shared<myInt>(5));
  std::cout << "val: " << cache->get(5)->cacheId() << std::endl;
  if(cache->get(3) == nullptr)
  {
    std::cout << "val \"7\" rewrites \"3\"" << std::endl;
  }
  std::cout << "val: " << val3->cacheId() << std::endl;
  // cache->put(5, 3);
  // std::cout << "val: " << cache->get(3) << std::endl;
  // if (cache->contains(7)) {
  //   cache->get(7);
  // } else {
  //   std::cout << "no such elem on key: 7" << std::endl;
  // }
}