#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <sstream>
#include <cstring>
#include <arpa/inet.h>
#include <netdb.h>
#include <unordered_map>
#include <map>
#include <string>
#include <fstream>
#include <functional>
#include <filesystem>

namespace http_server{
    class serversocket{
        public:
        struct HttpResponse {
            std::string body;
            std::string content_type = "text/plain"; // default
            int status_code = 200;                    // default OK
        };

        private:
        int server_fd;
        int port;

        using handler_t = std::function<HttpResponse(const std::string& body)>;
        std::unordered_map<std::string, handler_t> routes;


        void start_client_acception();

        void handle_request(int client);

        HttpResponse process_request(char* buffer , int rec);

        public:


        serversocket(int port);

        void listen_server();

        void add_route(const std::string& method, const std::string& path, handler_t handler);

        HttpResponse return_json(std::string json_str, int status_code = 200);

        HttpResponse return_html(std::string html_source , bool is_file = false, int status_code = 200);
    };
}