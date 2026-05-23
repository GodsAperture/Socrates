#include "StackAllocator.hpp"

    StackAllocator::StackAllocator(int64_t input){
        size = input;
        allocation = malloc(size * 8);
        head = allocation;
        current = head;
    }