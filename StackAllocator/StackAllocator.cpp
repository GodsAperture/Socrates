#include "StackAllocator.hpp"

    StackAllocator::StackAllocator(int64_t input){
        size = input;
        head = malloc(size * 8);
        current = head;
    }

    void StackAllocator::clear(){
        std::memset(head, 0, ((int64_t) current) - ((int64_t) head));
        current = head;
    }