#include "CppRest.hpp"
#include "CppCache.hpp"

const std::string jwt_secret_key = "asdfghjkl";



int main(){
    cpp_rest::serversocket server(8080);
    
    std::shared_ptr<CppCache::LRUCache> mycache = std::make_shared<CppCache::LRUCache>(50);


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
    


    // //Get Request with jwt verification from Authentication Bearer
    // server.add_route<std::monostate>("GET" , "/verifytoken" , [&server](const std::monostate&){
    //     myjson::Json payload;
    //     payload["id"] = "ABCD";
    //     payload["user"] = "admin";
    //     return server.return_json(payload.dump());
    // } , true , jwt_secret_key);



    
    
    server.listen_server();


    return 0;
}