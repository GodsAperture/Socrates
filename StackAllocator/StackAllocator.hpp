#ifndef STACKALLOCATOR_h__
#define STACKALLOCATOR_h__

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <cstring>

class StackAllocator{
public:
    void* head = nullptr;
    void* current = nullptr;
    int64_t size = 0;

    StackAllocator(int64_t input);

    template<typename T>
    T* allocate(){
        int64_t memorySize = sizeof(T);
        memorySize = memorySize + (7L & (8 - (7L & memorySize)));
        T* result = new (current) T();

        current = (void*) (((char*) current) + memorySize);

        return result;
    }

    void clear();
};
#endif