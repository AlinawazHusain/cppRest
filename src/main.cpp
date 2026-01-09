#include "http_server.hpp"

int main(){
    http_server::serversocket server(8080);
    
    server.add_route("GET" , "/home" , [&server](const std::string&body){

        return server.return_json(R"({"name" : "hello" , "data": 44})");
    });
    
    server.add_route("GET", "/html", [&server](const std::string& body) {
        std::string hhh  = R"(
            <!DOCTYPE html>
            <html>
            <head>
            <title>My Minimal Page</title>
            </head>
            <body>
            <h1>Hello, World!</h1>
            <p>This is a minimal HTML response from C++ server.</p>
            </body>
            </html>
            )";
            return server.return_html("index.html" , true);
        });
        

    server.listen_server();


    return 0;
}