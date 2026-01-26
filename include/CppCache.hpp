#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include "data_structures.hpp"
#include <string>
#include <mutex>



namespace  CppCache{

    /**
     * @brief Represent a LRU Cache
     * 
     * Public member functions
     * - `LRUCache - constructor` - Return type void , Initializes with available memory size for caching
     * - `push_data` - Return type void , Add data in cache
     * - `get_data` - Return type std::string , find and return data for specific key , else Exception message as string
     */
    class LRUCache{
        private:

        mutable std::mutex mtx;

        mutable data_structures::LRUMapping* lru_mapping;
        mutable data_structures::LRUMapping* last_node;

        size_t available_memory;
        size_t consumed_memory;

        std::unordered_map<std::string , std::pair<data_structures::LRUMapping*, std::string>> mycache;

        void clear_data(const size_t & need );

        size_t size_need(const std::string&key , const std::string&value)const;

        
        public:

            /**
             * @brief Initialises with memory assigned for caching
             * @param avail_mem_bytes Memory in bytes want to assign for caching
             */
            LRUCache(int avail_mem_bytes);


            /**
             * @brief Push data in cache
             * @param key Must be unique key for data to be unique identifier
             * @param value String data you want to store in cache
             */
            void push_data(const std::string &key , const std::string &value);



            /**
             * @brief Get data from cache if available else Exception message
             * @param key Key for data want to access from cache
             * @returns std::string data
             */
            std::string get_data(const std::string &key) const;

    };
}