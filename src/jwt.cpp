#include "jwt.hpp"
#include <ctime>
#include <sstream>
#include <vector>
#include <functional>
#include <stdexcept>


namespace jwt {

/* =========================
   Base64 URL encoding/decoding
   ========================= */

static const char* BASE64 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static std::string base64_url_encode(const std::string& in) {
    std::string out;
    int val = 0, valb = -6;
    for (unsigned char c : in) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            out.push_back(BASE64[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) out.push_back(BASE64[((val << 8) >> (valb + 8)) & 0x3F]);
    while (out.size() % 4) out.push_back('=');

    for (char& c : out) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }
    out.erase(out.find_last_not_of('=') + 1);
    return out;
}

static std::string base64_url_decode(std::string in) {
    for (char& c : in) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    while (in.size() % 4) in.push_back('=');

    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[BASE64[i]] = i;

    std::string out;
    int val = 0, valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

/* =========================
   Simple deterministic "signature"
   ========================= */

static std::string sign(const std::string& data, const std::string& secret) {
    std::hash<std::string> hasher;
    size_t h = hasher(secret + "." + data);
    return std::to_string(h);
}

/* =========================
   Create JWT
   ========================= */

std::string Jwt::create(const myjson::Json& payload,
                        const std::string& secret,
                        int expire_seconds)
{
    if (secret.empty()) throw std::runtime_error("Secret key cannot be empty");

    myjson::Json header;
    header["alg"] = "CUSTOM";
    header["typ"] = "JWT";

    myjson::Json body = payload;
    body["iat"] = static_cast<int>(std::time(nullptr));

    if (expire_seconds > 0) {
        body["exp"] = static_cast<int>(std::time(nullptr)) + expire_seconds;
    }

    std::string h = base64_url_encode(header.dump());
    std::string p = base64_url_encode(body.dump());
    std::string data = h + "." + p;
    std::string sig = base64_url_encode(sign(data, secret));

    return data + "." + sig;
}

/* =========================
   Verify JWT
   ========================= */

myjson::Json Jwt::verify(const std::string& token,
                         const std::string& secret,
                         bool check_expiration)
{
    if (secret.empty()) throw std::runtime_error("Secret key cannot be empty");

    size_t p1 = token.find('.');
    size_t p2 = token.find('.', p1 + 1);
    if (p1 == std::string::npos || p2 == std::string::npos) {
        throw std::runtime_error("Invalid token format");
    }

    std::string h = token.substr(0, p1);
    std::string p = token.substr(p1 + 1, p2 - p1 - 1);
    std::string s = token.substr(p2 + 1);

    std::string data = h + "." + p;
    std::string expected = base64_url_encode(sign(data, secret));

    if (expected != s) {
        throw std::runtime_error("Invalid signature");
    }

    myjson::Json payload = myjson::Json::parse(base64_url_decode(p));
    if (check_expiration && payload.is_object()) {
        if (payload["exp"].is_number()) {
            int exp = static_cast<int>(payload["exp"].as_number());
            if (std::time(nullptr) > exp) {
                throw std::runtime_error("Token expired");
            }
        }
    }
    return payload;
}

} // namespace jwt
