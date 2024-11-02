#include <iostream>
#include <list>
#include <unordered_map>
#include <utility>
#include <stdexcept>
#include <mutex>
#include <memory>

#ifndef INT_BIT_CACHE_H
#define INT_BIT_CACHE_H

template <typename Value, size_t bitSize, size_t bitShift>
class IntBitCache {
private:
    std::shared_ptr<Value> cacheArr[1 << bitSize] {};
    size_t bitMask {((1 << bitSize) - 1) << bitShift};
public:
    IntBitCache() {};

    void put(std::shared_ptr<Value> val)
    {
        cacheArr[(val->cacheId() & bitMask) >> bitShift] = val;
    }

    std::shared_ptr<Value> get(size_t key)
    {
        std::shared_ptr<Value> findVal = cacheArr[(key & bitMask) >> bitShift];
        if(findVal->cacheId() == key)
        {
            return findVal;
        }
        return std::shared_ptr<Value>(nullptr);
    }

};

#endif