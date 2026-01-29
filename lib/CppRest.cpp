#include "CppRest.hpp"


cpp_rest::serversocket::serversocket(int port){
    this->port = port;
    server_fd = socket(AF_INET , SOCK_STREAM , 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd , (sockaddr*)&addr , sizeof(addr));
}



void cpp_rest::serversocket::listen_server(){
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


void cpp_rest::serversocket::make_non_blocking(int fd){
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}



void cpp_rest::serversocket::start_client_acception(){
    this->make_non_blocking(server_fd);

    #if defined(_WIN32) || defined(_WIN64)

    std::vector<pollfd> fds;

    pollfd server_pool{};
    server_pool.fd = this->server_fd;
    server_pool.events = POLLIN;
    fds.push_back(server_pool);

    while(true){
        int ready = poll(fds.data() , fds.size() , -1);
        if(ready < 0) continue;
        
        for(size_t i = 0; i<fds.size(); i++){
            if(fds[i].revents && POLLIN){
                if(fds[i].fd == server_fd){
                    int client = accept(this->server_fd , nullptr , nullptr);
                    if(client>=0){
                        this->make_non_blocking(client);
                        pollfd client_fd{};
                        client_fd.fd = client;
                        client_fd.events = POLLIN;
                        fds.push_back(client_fd);
                    }
                }
                else{
                    handle_request(fds[i].fd);
                }
            }
        }
    }

    #elif defined(__linux__)
    
    int epfd = epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = this->server_fd;
    epoll_ctl(epfd , EPOLL_CTL_ADD , this->server_fd , &ev);

    epoll_event events[100];

    while(true){
        int n = epoll_wait(epfd , events , 100 , -1);
        
        for(int i = 0; i<n; i++){
            int fd = events[i].data.fd;

            if(fd == server_fd){
                while(true){
                    int client = accept(this->server_fd, nullptr , nullptr);
                    if(client <0){
                        if(errno == EAGAIN || errno == EWOULDBLOCK) break;
                        else break;
                    }
                    this->make_non_blocking(client);
    
                    epoll_event client_ev{};
                    client_ev.events = EPOLLIN | EPOLLET;
                    client_ev.data.fd = client;
                    epoll_ctl(epfd , EPOLL_CTL_ADD , client , &client_ev);
                }

            }
            else{
                handle_request_async(epfd , fd);
            }
        }
    }

    
    #endif
}



void cpp_rest::serversocket::handle_request_async(int epfd , int client){
    char buffer[4096];

    int rec;
    while(true){

        rec = recv(client , buffer , sizeof(buffer) , 0);
        if(rec > 0) break;
        else if(rec == 0){
            epoll_ctl(epfd, EPOLL_CTL_DEL, client, nullptr);
            close(client);
            return;
        }
        else {                            
            if(errno == EAGAIN || errno == EWOULDBLOCK) 
                return;           
            else {
                perror("recv");
                epoll_ctl(epfd, EPOLL_CTL_DEL, client, nullptr);
                close(client);
                return;
            }
        }
    }






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

    size_t total_send = 0;
    while(total_send < http_res.size()){

        int sent = send(client, http_res.c_str(), http_res.size(), 0);

        if(sent >0) total_send += sent;

        else if(sent == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) continue;
        
        else {
            perror("send");
            epoll_ctl(epfd, EPOLL_CTL_DEL, client, nullptr);
            close(client);
            return;
        }
    }
    epoll_ctl(epfd, EPOLL_CTL_DEL, client, nullptr);
    close(client);

    std::string method, path;
    std::istringstream iss(buffer);
    iss >> method >> path; // extract first line method and path
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::cout << client_ip << " - " << method << " " << path << " " << res.status_code <<"  "<< std::ctime(&now_time)<<std::endl;
}





void cpp_rest::serversocket::handle_request(int client){
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
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::cout << client_ip << " - " << method << " " << path << " " << res.status_code <<"  "<< std::ctime(&now_time)<<std::endl;
}





cpp_rest::serversocket::HttpResponse cpp_rest::serversocket::process_request(char* buffer , int rec) {
    std::string req(buffer, rec);
    std::istringstream iss(req);

    size_t pos = req.find("\r\n\r\n");
    std::string header_str;
    std::string body;

    if (pos != std::string::npos) {
        header_str = req.substr(0, pos);
        body = req.substr(pos + 4);
    } else {
        header_str = req;
        body = "";
    }

    std::string method, path, version;
    iss >> method >> path >> version;
    std::unordered_map<std::string, std::string> headers;

    std::string line;
    while (std::getline(iss, line)) {
        if (line == "\r") continue; // skip empty line
        size_t colon = line.find(":");
        if (colon != std::string::npos) {
            std::string key = line.substr(0, colon);
            std::string value = line.substr(colon + 1);
            while (!value.empty() && value[0] == ' ') value.erase(0,1);
            while (!value.empty() && (value.back() == '\r' || value.back() == '\n')) value.pop_back();
            headers[key] = value;
        }
    }

    std::string key = method + " " + path;
    nlohmann::json payload;
    bool have_auth = false;
    auto route_data = this->routes.find(key);
    if (route_data != this->routes.end()) {
        // Check if JWT is required for this route
        if (route_data->second.have_jwt) {  // second.second == JWT required
            // Check for Authorization header
            have_auth = true;
            if (headers.find("Authorization") == headers.end()) {
                return {"401 Unauthorized", "text/plain", 401};
            }

            std::string token = headers["Authorization"];

            // Make sure token starts with "Bearer "
            if (token.rfind("Bearer ", 0) != 0) {
                return {"401 Unauthorized", "text/plain", 401};
            }

            token = token.substr(7); // remove "Bearer "
            // Remove quotes and spaces
            token.erase(std::remove(token.begin(), token.end(), '"'), token.end());
            token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());

            // Verify JWT safely
            try {
                payload = jwt::Jwt::verify(token, route_data->second.jwt_secret_key, true);
                // Optionally, you can store payload for handlers
            } catch (const std::exception& e) {
                std::cerr << "JWT verification failed: " << e.what() << std::endl;
                return {"401 Unauthorized", "text/plain", 401};
            }
        }

        // Call the route handler with the request body
        auto& handler = route_data->second.req_handler;
        std::variant<nlohmann::json, std::monostate> body_variant;

        std::string content_type = (headers.find("Content-Type") !=  headers.end())? headers["Content-Type"] : "application/json";
        // Decide what to push
        if (content_type == "application/x-www-form-urlencoded") {
            std::string form_data = handle_form_data(body);
            body_variant = nlohmann::json(form_data);
        } 
        else if (content_type == "application/json" || content_type == "text/plain") {
            if (!body.empty()) {
                body_variant = nlohmann::json::parse(body);
            } else {
                body_variant = std::monostate{};
            }
        }
        else if (body.empty()) {
            body_variant = std::monostate{};
        }


        
        try {
        // Call the handler with the variant
        return handler(body_variant);
        } 
        catch (const std::bad_any_cast& e) {
        // Happens if std::any_cast fails
        std::cerr << "Handler error (bad_any_cast): " << e.what() << "\n";
        return { "Internal Server Error: bad request body", "text/plain", 400 };
        } 
        catch (const std::runtime_error& e) {
            // For your dto.get_val errors etc.
            std::cerr << "Handler runtime error: " << e.what() << "\n";
            return { std::string("Runtime error: ") + e.what(), "text/plain", 400 };
        } 
        catch (const std::exception& e) {
            // Generic exceptions
            std::cerr << "Handler exception: " << e.what() << "\n";
            return { std::string("Internal Server Error: ") + e.what(), "text/plain", 500 };
        }
        catch (...) {
            // Catch all
            std::cerr << "Unknown handler exception\n";
            return { "Unknown Internal Server Error", "text/plain", 500 };
        }
    }

    return {"404 Not Found", "text/plain", 404};
}







cpp_rest::serversocket::HttpResponse cpp_rest::serversocket::return_json(nlohmann::json body, int status_code){
    
    cpp_rest::serversocket::HttpResponse res;
    std::string body_str;
    if constexpr (std::is_same_v<decltype(body), nlohmann::json>) {
        body_str = body.dump();
    } else {
        body_str = body;
    }
    res.content_type = "application/json";
    res.body = body_str;
    res.status_code = status_code;
    return res;
}



cpp_rest::serversocket::HttpResponse cpp_rest::serversocket::return_html(std::string html_source , bool is_file , int status_code){

    cpp_rest::serversocket::HttpResponse res;

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




std::string cpp_rest::serversocket::handle_form_data(std::string &body){
    std::unordered_map<std::string,std::string> data;
    size_t start = 0;

    while (start < body.size()) {
        size_t eq = body.find('=', start);
        size_t amp = body.find('&', start);

        if (eq == std::string::npos) break;

        std::string key = body.substr(start, eq - start);
        std::string value;
        if (amp == std::string::npos) {
            value = body.substr(eq + 1);
            start = body.size();
        } else {
            value = body.substr(eq + 1, amp - eq - 1);
            start = amp + 1;
        }
        key = this->url_decode(key);
        value = this->url_decode(value);
        data[key] = value;
        start = (amp == std::string::npos) ? body.size() : amp + 1;
    }

    std::stringstream ss;
    ss << "{";

    bool first = true;
    for (const auto& [key, value] : data) {
        if (!first) ss << ",";
        first = false;

        // Escape quotes in key and value
        std::string k = key;
        std::string v = value;
        size_t pos = 0;
        while ((pos = k.find('"', pos)) != std::string::npos) { k.insert(pos, "\\"); pos += 2; }
        pos = 0;
        while ((pos = v.find('"', pos)) != std::string::npos) { v.insert(pos, "\\"); pos += 2; }

        ss << "\"" << k << "\":\"" << v << "\"";
    }

    ss << "}";
    return ss.str();

}



// decode percent-encoded strings like a=hello%20world
std::string cpp_rest::serversocket::url_decode(const std::string &src) {
    std::string ret;
    char ch;
    int i, ii;
    for (i = 0; i < src.length(); i++) {
        if (src[i] == '%') {
            sscanf(src.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            ret += ch;
            i += 2;
        } else if (src[i] == '+') {
            ret += ' ';
        } else {
            ret += src[i];
        }
    }
    return ret;
}
