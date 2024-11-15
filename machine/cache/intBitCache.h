#include <array>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#ifndef INT_BIT_CACHE_H
#define INT_BIT_CACHE_H

template <typename Value, size_t bitSize, size_t bitShift> class IntBitCache {
  private:
    std::array<std::shared_ptr<Value>, 1 << bitSize> cacheArr;
    size_t bitMask;

  public:
    IntBitCache() { bitMask = ((1 << bitSize) - 1) << bitShift; }

    void put(std::shared_ptr<Value> val) { cacheArr[(*(val)&bitMask) >> bitShift] = val; }

    std::shared_ptr<Value> get(size_t key) {
        std::shared_ptr<Value> findVal = cacheArr[(key & bitMask) >> bitShift];
        if (findVal != nullptr && *(findVal) == key) {
            return findVal;
        }
        return std::shared_ptr<Value>(nullptr);
    }
};

#endif