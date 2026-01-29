# Minimal HTTP Server for C++

A lightweight and minimal HTTP/1.1 server library written in modern C++.  
This project focuses on simplicity and clarity, providing a basic REST-style API for handling HTTP requests without external dependencies.

---

## Features

- Minimal HTTP/1.1 server implementation
- Route-based request handling
- Supports multiple HTTP methods (GET, POST, etc.)
- JSON and HTML response helpers
- HTML file serving from a templates directory
- Production ready for large volume of client using async Epoll(Linux) and Poll
- No third-party dependencies (POSIX sockets only)
- JWT for Authentication
- Custon Json object for json like easy processing 
- Inbuild LRU caching 

---

## Project Structure

```text
│── bin

│── build

│── include
    │── CppCache.hpp
    │── data_structure.hpp
    │── CppRest.hpp 
    │── json.hpp
    │── jwt.hpp

│── lib
    │── CppCache.cpp
    │── data_structure.cpp
    │── CppRest.cpp 
    │── json.cpp
    │── jwt.cpp

├── src
    │── main.cpp # Example usage

│── templates/
    │── index.html # HTML templates (optional for html rendering)

│── build.sh
│── run.sh
│── CMakeLists.txt
│── Dockerfile
│── README.md
│── LICENSE
```


---

## Requirements

- Linux or Unix-like operating system
- C++17 or newer
- POSIX-compliant networking environment

---

## Build Instructions

Build the project:

```bash
./build.sh < ProjectName > < EntryPointCppFile > < Build type i.e Debug /Release >
```

Build for this project Structure:
```bash

# Using defaults (CppRest + src/main.cpp + Debug)
./build.sh

# Debug build (default)
./build.sh CppRest src/main.cpp Debug

# Release build 
./build.sh CppRest src/main.cpp Release

```

If project name ,  entrypointcppfile  and build type isn't given it will keep them CppRest , src/main.cpp and Debug  by default



Run the project:

```bash
./run.sh < ProjectName default :- CppRest> < logsdir default:- logs>
```



## Installing , Using and Uninstalling of Build server in Linux

Installing:

```bash
./install-server.sh < Project name default:- CppRest >
```

Starting server:

```bash
./start-server.sh < Project name default:- CppRest > < logs dir Optionals , default :- logs >
```

Stoping server:

```bash
./stop-server.sh < Project name default:- CppRest >
```


Uninstalling:

```bash
./uninstall-server.sh < Project name default:- CppRest >
```

Server logs will be written in logs/server.log by default


## Parameter types

```cpp
    //FOR NO INPUT (VOID)
    server.add_route<std::monostate>("GET" , "/home" , [&server](const std::monostate&){...})


    //FOR "application/x-www-form-urlencoded" OR "application/json" OR "text/plain" 
    // IT will make nlohmann::json object of data from client
    server.add_route<nlohmann::json>("POST" , "/gettoken" , [&server](const nlohmann::json& body){...})

```


## Example usage

```cpp
#include "CppRest.hpp"

const std::string jwt_secret_key = "asdfghjkl";

int main(){
    cpp_rest::serversocket server(8080);
    


    //Get Request with json response
    server.add_route<std::monostate>("GET" , "/home" , [&server](const std::monostate&){
        nlohmann::json res;
        res["name"] = "hello";
        res["data"] = 44;
        return server.return_json(res);
    });
    

    // //Get Request with html response
    server.add_route<std::monostate>("GET", "/html", [&server](const std::monostate&) {
        return server.return_html("index.html" , true);
    });


    server.add_route<nlohmann::json>("POST", "/json", [&server](const nlohmann::json& body) {
        return server.return_json(body);
    });

    

    // //Get Request with jwt token generation
    server.add_route<nlohmann::json>("POST" , "/gettoken" , [&server](const nlohmann::json& body){
        nlohmann::json payload;
        payload["id"] = "ABCD";
        payload["user"] = "admin";
        std::string token = jwt::Jwt::create(payload , jwt_secret_key);
        payload["token"] = token;
        return server.return_json(payload);
    });
    
    


    //Get Request with jwt verification from Authentication Bearer
    server.add_route<std::monostate>("GET" , "/verifytoken" , [&server](const std::monostate&){
        nlohmann::json payload;
        payload["id"] = "ABCD";
        payload["user"] = "admin";
        return server.return_json(payload);
    } , true , jwt_secret_key);




    server.listen_server();


    return 0;
}

```


## Inbuild Cache Instructions

```cpp
#include "CppCache.hpp"  //Include hpp file


// Create instance with shared pointer for Best performance
std::shared_ptr<CppCache::LRUCache> mycache = std::make_shared<CppCache::LRUCache>(50);


// capture cache instance in lambda to use
server.add_route<nlohmann::json>("POST" , "/cacheAdd" , [&server , mycache](const nlohmann::json&body){});


//Available functions

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
```