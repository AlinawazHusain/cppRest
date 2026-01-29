#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace jwt {

class Jwt {
public:
    /**
     * Create a JWT token.
     * @param payload JSON payload (custom fields)
     * @param secret Secret key (mandatory)
     * @param expire_seconds Optional expiration in seconds (0 = no expiration)
     * @return JWT token string
     */
    static std::string create(
        const nlohmann::json& payload,
        const std::string& secret,
        int expire_seconds = 0
    );

    /**
     * Verify a JWT token.
     * @param token JWT token string
     * @param secret Secret key used to sign
     * @param check_expiration Check expiration if present
     * @return JSON payload if valid, empty JSON if invalid
     * @throws std::runtime_error on invalid token or signature
     */
    static nlohmann::json verify(
        const std::string& token,
        const std::string& secret,
        bool check_expiration = true
    );
};

} // namespace jwt
