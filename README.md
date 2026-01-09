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
- Multi-threaded client handling (one thread per connection)
- No third-party dependencies (POSIX sockets only)

---

## Project Structure

├── http_server.hpp # Public API definitions
├── http_server.cpp # Server implementation
├── main.cpp # Example usage
├── templates/
│ └── index.html # HTML templates (optional for html rendering)



---

## Requirements

- Linux or Unix-like operating system
- C++17 or newer
- POSIX-compliant networking environment

---

## Build Instructions

Compile the project using `g++` with pthread support:

```bash
g++ -std=c++17 main.cpp http_server.cpp -o server -pthread
```

##Example usage

```cpp
#include "http_server.hpp"

int main() {
    http_server::serversocket server(8080);

    server.add_route("GET", "/home", [&server](const std::string& body) {
        return server.return_json(R"({"name":"hello","data":44})");
    });

    server.add_route("GET", "/html", [&server](const std::string& body) {
        return server.return_html("index.html", true);
    });

    server.listen_server();
    return 0;
}

```