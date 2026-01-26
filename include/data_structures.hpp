#pragma once

#include <iostream>
#include <string>

namespace data_structures{
    
    struct LRUMapping{
        std::string key;
        LRUMapping* prev;
        LRUMapping*  next;

        LRUMapping(const std::string & key);
    };
}