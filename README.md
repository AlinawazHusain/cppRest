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
│── build
    |── Makefile
    │── CppRest

├── src
    │── CppCache.cpp
    │── CppCache.hpp
    │── data_structure.cpp
    │── data_structure.hpp
    │── http_server.hpp # Public API definitions
    │── http_server.cpp # Server implementation
    │── json.cpp
    │── json.hpp
    │── jwt.cpp
    │── jwt.hpp
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
build.sh
```

Run the project:

```bash
run.sh
```

## Example usage

```cpp
#include "http_server.hpp"

const std::string jwt_secret_key = "asdfghjkl";

int main(){
    http_server::serversocket server(8080);
    


    //Get Request with json response
    server.add_route("GET" , "/home" , [&server](const std::string&body){
        myjson::Json res;
        res["name"] = "hallo";
        res["data"] = 44;
        return server.return_json(res.dump());
    });
    

    //Get Request with html response
    server.add_route("GET", "/html", [&server](const std::string& body) {
            return server.return_html("index.html" , true);
        });
        
    

    //Get Request with jwt token generation
    server.add_route("GET" , "/gettoken" , [&server](const std::string &body){
        myjson::Json payload;
        payload["id"] = "ABCD";
        payload["user"] = "admin";
        std::string token = jwt::Jwt::create(payload , jwt_secret_key);

        payload["token"] = token;
        return server.return_json(payload.dump());
    });
    


    //Get Request with jwt verification from Authentication Bearer
    server.add_route("GET" , "/verifytoken" , [&server](const std::string &body){
        myjson::Json payload;
        payload["id"] = "ABCD";
        payload["user"] = "admin";
        return server.return_json(payload.dump());
    } , true , jwt_secret_key);



    //Post Request with input as json or form data and reading it as well
    server.add_route("POST" , "/postbody" , [&server](const std::string &body){
        std::cout<<body<<std::endl;
        myjson::Json payload;
        
        payload = myjson::Json::parse(body);
        // payload["id"] = "ABCD";
        // payload["user"] = "admin";
        return server.return_json(payload.dump());
    });

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
server.add_route("POST" , "/cacheAdd" , [&server , mycache](const std::string &body){});


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