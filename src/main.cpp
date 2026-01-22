#include "http_server.hpp"
#include "CppCache.hpp"

const std::string jwt_secret_key = "asdfghjkl";



int main(){
    http_server::serversocket server(8080);
    
    std::shared_ptr<CppCache::LRUCache> mycache = std::make_shared<CppCache::LRUCache>(50);


    //Get Request with json response
    server.add_route("GET" , "/home" , [&server](const std::string&body){
        myjson::Json res;
        res["name"] = "hello";
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

    // server.add_route("POST" , "/add_in_cache" , [&server](const std::string &body){
    server.add_route("POST" , "/cacheAdd" , [&server , mycache](const std::string &body){
        myjson::Json payload;
        payload = myjson::Json::parse(body);
        std::string key = payload["key"].as_string();
        std::string value = payload["value"].as_string();

        mycache->push_data(key , value);

        return server.return_json(payload.dump());

    });


    server.add_route("POST" , "/cacheGet" , [&server , mycache](const std::string &body){
        myjson::Json payload;
        payload = myjson::Json::parse(body);
        std::string key = payload["key"].as_string();

        std::string value = mycache->get_data(key);
        payload[key] = value;

        return server.return_json(payload.dump());

    });
    server.listen_server();


    return 0;
}