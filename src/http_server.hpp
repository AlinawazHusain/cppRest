#pragma once

#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
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
#include "jwt.hpp"
#include <ctime>
#include <chrono>

// #if defined(_WIN32) || defined(_WIN64)
#include <poll.h>
// #else
#include <sys/epoll.h>
#include <fcntl.h>
// #endif

namespace http_server{

    /**
     * @brief Represent a server interface socket
     * 
     * Public member functions
     * - `listen_server` - Return type void , start server listining from clients
     * - `add_route` - Return type void , Add custom request in server
     * - `return_json` - Return type HttpResponse , help to send myjson::json object 
     * - `return_html` - Return type HttpResponse , help to send html resonse from both html file and raw html string
     */
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
        std::unordered_map<std::string, std::pair<handler_t , std::pair<std::string , bool>>> routes;


        void start_client_acception();

        void handle_request(int client);

        void handle_request_async(int epfd , int client);

        std::string handle_form_data(std::string & query);

        std::string url_decode(const std::string &src);

        HttpResponse process_request(char* buffer , int rec);

        void make_non_blocking(int fd);

        public:

        /**
         * @brief Constructor for server socket 
         * @param port : int port number of server
         */
        serversocket(int port);


        /**
         * @brief Start server listening
         */
        void listen_server();


        /**
         * @brief Add a route handler
         * @param method HTTP method (GET, POST)
         * @param path Route path
         * @param handler function to run on request 
         * @param include_jwt option to include jwt authentication
         * @param jwt_secret_key Secret key for jwt auth
         */
        void add_route(const std::string& method, const std::string& path, handler_t handler , bool include_jwt = false , std::string jwt_secret_key = "");


        /**
         * @brief Return a Json to client
         * @param json_str A string of json object , i.e myjson::json.dump()
         * @param status_code Status code for response , default 200
         */
        HttpResponse return_json(std::string json_str, int status_code = 200);


        /**
         * @brief Return html to render for client
         * @param html_source a html file path or a raw html string , (For path the html file must be in templates folder and just give path like "index.html")
         * @param is_file true of html_source is a file false for raw html string
         * @param status_code Status code for response , default 200
         */
        HttpResponse return_html(std::string html_source , bool is_file = false, int status_code = 200);
    };
}