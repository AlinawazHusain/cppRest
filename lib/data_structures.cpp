#include "data_structures.hpp"

data_structures::LRUMapping::LRUMapping(const std::string &key): key(key) , prev(nullptr) , next(nullptr){}

