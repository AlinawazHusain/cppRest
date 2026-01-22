#include "CppCache.hpp"


CppCache::LRUCache::LRUCache(int avail_mem_bytes){
    std::string head_data = "head";
    this->available_memory = avail_mem_bytes;
    this->consumed_memory = head_data.size();

    this->lru_mapping = new data_structures::LRUMapping(head_data);

    this->last_node = this->lru_mapping;
}


std::string CppCache::LRUCache::get_data(const std::string &key)const{
    std::lock_guard<std::mutex> lock(this->mtx);

    auto it = mycache.find(key);
    if(it == this->mycache.end()) return "Exception :: Data not found with this key";

    
    data_structures::LRUMapping* node = it->second.first;
    if(node != this->last_node){
        
        node->prev->next = node->next;
        if(node->next)node->next->prev = node->prev;
    
        node->prev = this->last_node;
        node->next = nullptr;
        this->last_node->next = node;
        this->last_node = node;
    }
    
    return it->second.second;
    
}

void CppCache::LRUCache::clear_data(const size_t & need ){


    data_structures::LRUMapping* node = this->lru_mapping->next;

    while(node != nullptr  && this->consumed_memory + need >= this->available_memory){

        std::string key = node->key;
        auto it = this->mycache.find(key);

        if(it != this->mycache.end()){

            size_t value_size = it->second.second.size();;
            
            this->mycache.erase(it);
    
            data_structures::LRUMapping* next_node = node->next;

            node->prev->next = next_node;
            if (next_node) next_node->prev = node->prev;
            if (node == last_node) last_node = node->prev;
    
    
            delete node;
            node = next_node;
    
            size_t decreased_size = sizeof(data_structures::LRUMapping) +value_size+key.size();
            this->consumed_memory -= decreased_size;
        }

        else break;
    }
}

size_t CppCache::LRUCache::size_need(const std::string&key , const std::string&value)const{
    size_t need = sizeof(data_structures::LRUMapping)*2;

    size_t key_size = key.size();
    size_t value_size = value.size();

    return need + value_size + key_size;

}



void CppCache::LRUCache::push_data(const std::string &key , const std::string &value){

    std::lock_guard<std::mutex> lock(this->mtx);
    size_t need = this->size_need(key , value);

    this->clear_data(need);

    auto it = this->mycache.find(key);
    if(it != this->mycache.end()){
        size_t prev_data = it->second.second.size();
        size_t new_data = value.size();
        this->consumed_memory += (new_data - prev_data);
        it->second.second = value;
        data_structures::LRUMapping* node = it->second.first;
        if(node != last_node){
            if(node->prev) node->prev->next = node->next;
            if(node->next) node->next->prev = node->prev;
            node->prev = last_node;
            node->next = nullptr;
            last_node->next = node;
            last_node = node;
        }
        return;
    }

    data_structures::LRUMapping* curr_node = new data_structures::LRUMapping(key);

    this->last_node->next = curr_node;
    curr_node->prev = this->last_node;
    this->last_node = curr_node;

    this->mycache.insert({key , {curr_node , value}});
    this->consumed_memory += need;
}