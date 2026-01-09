#include "http_server.hpp"



http_server::serversocket::serversocket(int port){
    this->port = port;
    server_fd = socket(AF_INET , SOCK_STREAM , 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd , (sockaddr*)&addr , sizeof(addr));
}



void http_server::serversocket::listen_server(){
    int is_listening = listen(server_fd , 10);
    if(is_listening == -1){
        std::cout<<"Unable to start server ..."<<std::endl;
        return;
    }

    char host[256];
    gethostname(host, sizeof(host));
    struct hostent* hent = gethostbyname(host);
    char* ip = inet_ntoa(*(struct in_addr*)hent->h_addr);

    std::cout << "Server Started on : http://" << ip << ":" << this->port << std::endl;
    this->start_client_acception();
}


void http_server::serversocket::start_client_acception(){
    while(true){
        int client = accept(this->server_fd , nullptr , nullptr);
        if(client <0) continue;

        std::thread(&http_server::serversocket::handle_request, this , client).detach();
    }
}





void http_server::serversocket::handle_request(int client){
    char buffer[4096];
    int rec = recv(client , buffer , sizeof(buffer) , 0);


    sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    getpeername(client, (sockaddr*)&addr, &addr_len);
    std::string client_ip = inet_ntoa(addr.sin_addr);


    HttpResponse res;
    std::string req(buffer, rec);
    res = this->process_request(buffer , rec);


    std::string http_res = "HTTP/1.1 " + std::to_string(res.status_code) + " OK\r\n";
    http_res += "Content-Type: " + res.content_type + "\r\n";
    http_res += "Content-Length: " + std::to_string(res.body.size()) + "\r\n";
    http_res += "Connection: close\r\n\r\n";
    http_res += res.body;

    send(client, http_res.c_str(), http_res.size(), 0);
    close(client);

    std::string method, path;
    std::istringstream iss(buffer);
    iss >> method >> path; // extract first line method and path
    std::cout << client_ip << " - " << method << " " << path << " " << res.status_code << std::endl;
}





http_server::serversocket::HttpResponse http_server::serversocket::process_request(char* buffer , int rec){
    std::string req(buffer , rec);
    std::istringstream iss(req);

    size_t pos = req.find("\r\n\r\n");
    std::string header_str;
    std::string body;


    if (pos != std::string::npos) {
    header_str = req.substr(0, pos);
    body = req.substr(pos + 4);
    } else {
        // no body
        header_str = req;
        body = "";
    }



    std::string method , path, version;
    iss>>method>>path>>version;
    std::unordered_map<std::string , std::string> headers;

    std::string line;
    while (std::getline(iss, line)) {
        
        if (line == "\r") continue; // skip empty line
        size_t colon = line.find(":");

        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            // trim spaces
            while (!value.empty() && value[0] == ' ') value.erase(0,1);
            while (!value.empty() && (value.back() == '\r' || value.back() == '\n')) value.pop_back();
            headers[key] = value;
        }
    }

    std::string key = method + " " + path;

    if (this->routes.find(key) != this->routes.end()) {
        // pass the body to the handler
        return this->routes[key](body);
    }

    return {"404 Not Found", "text/plain", 404};
}






void http_server::serversocket::add_route(const std::string& method, const std::string& path, handler_t handler){
    std::string key = method + " " + path;
    this->routes[key] = handler;
}



http_server::serversocket::HttpResponse http_server::serversocket::return_json(std::string json_str, int status_code){
    
    http_server::serversocket::HttpResponse res;
    res.content_type = "application/json";
    res.body = json_str;
    res.status_code = status_code;
    return res;
}



http_server::serversocket::HttpResponse http_server::serversocket::return_html(std::string html_source , bool is_file , int status_code){

    http_server::serversocket::HttpResponse res;

    res.content_type = "text/html";
    res.status_code = status_code;

    if (is_file) {
        // Build path inside ./templates
        std::filesystem::path file_path = std::filesystem::current_path() / "templates" / html_source;

        // Check if file exists and is a regular file
        if (!std::filesystem::exists(file_path) || !std::filesystem::is_regular_file(file_path)) {
            res.body = "<h1>404 Not Found</h1>";
            res.status_code = 404;
            return res;
        }

        // Read file content
        std::ifstream file(file_path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        res.body = buffer.str();
    } else {
        // Direct HTML string
        res.body = html_source;
    }

    return res;
}

