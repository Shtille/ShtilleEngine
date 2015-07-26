#pragma once
#ifndef __SHT_COUNTING_POINTER_H__
#define __SHT_COUNTING_POINTER_H__

namespace sht {
    
    //! Simple class for manually handling reference counting
    // Note: this class do not deallocate data for pointer
    // That was done due to possibility of using protected destructors.
    template <class T>
    class CountingPointer {
    public:
        CountingPointer()
        : pointer_(nullptr)
        , count_(0)
        {
            
        }
        CountingPointer(T * pointer)
        : pointer_(pointer)
        , count_(1)
        {
            
        }
        void IncreaseCount()
        {
            ++count_;
        }
        void DecreaseCount()
        {
            --count_;
        }
        
        T * pointer() const { return pointer_; }
        int count() const { return count_; }
        
    private:
        T* pointer_;
        int count_;
    };
    
} // namespace sht

#endif