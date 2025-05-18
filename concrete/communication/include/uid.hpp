#ifndef __UID_HPP__
#define __UID_HPP__

#include <cstdint>
#include <iostream>

#include "imessage.hpp"
extern "C"
{
#include "uid_c.h"
}

namespace ilrd
{

    class UID :  public IMessage 
    {
    public:
        explicit UID();
        ~UID() = default;

        ilrd_uid_t GetUID() const;
        
        struct HashUID // To make Uid hashable
        {
            size_t operator()(const UID &) const;
        };
        
        uint64_t GetBufferSize() const override;
        char *ToBuffer(char *buffer) const override;
        char *FromBuffer(char *buffer) override;

        
        private:
        ilrd_uid_t m_UID;
        
    }; // class UID
    
    bool operator==(const UID& lhs, const UID& rhs);
    
    std::ostream& operator<<(std::ostream& out, const UID& uid);
}

#endif /* __UID_HPP__ */


/*
    from c20 - Any comparison has to be outside the class (non-member function).
    the compiler is auto generated a reversed function to answer 
    implicit convertion problam. moving it out the class will cause ambiguity.  
*/