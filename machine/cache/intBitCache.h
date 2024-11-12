#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>
#include <stdexcept>
#include <mutex>
#include <memory>
#include <array>

#ifndef INT_BIT_CACHE_H
#define INT_BIT_CACHE_H

template <typename Value, size_t bitSize, size_t bitShift>
class IntBitCache {
private:
    std::array<std::shared_ptr<Value>, 1 << bitSize> cacheArr;
    size_t bitMask;
public:
    IntBitCache() 
    {
        bitMask = ((1 << bitSize) - 1) << bitShift;
    }

    void put(std::shared_ptr<Value> val)
    {
        cacheArr[(val->cacheId() & bitMask) >> bitShift] = val;
    }

    std::shared_ptr<Value> get(size_t key)
    {
        std::shared_ptr<Value> findVal = cacheArr[(key & bitMask) >> bitShift];
        if(findVal != nullptr && findVal->cacheId() == key)
        {
            return findVal;
        }
        return std::shared_ptr<Value>(nullptr);
    }

};

#endif