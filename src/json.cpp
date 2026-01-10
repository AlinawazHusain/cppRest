#include "json.hpp"

#include <sstream>
#include <stdexcept>
#include <cctype>

namespace myjson {

/* =========================
   Constructors
   ========================= */

Json::Json() : value(nullptr) {}
Json::Json(std::nullptr_t) : value(nullptr) {}
Json::Json(bool b) : value(b) {}
Json::Json(int n) : value(static_cast<double>(n)) {}
Json::Json(double n) : value(n) {}
Json::Json(const char* s) : value(std::string(s)) {}
Json::Json(const std::string& s) : value(s) {}
Json::Json(const array& a) : value(a) {}
Json::Json(const object& o) : value(o) {}

/* =========================
   Type checks
   ========================= */

bool Json::is_null()   const { return std::holds_alternative<std::nullptr_t>(value); }
bool Json::is_bool()   const { return std::holds_alternative<bool>(value); }
bool Json::is_number() const { return std::holds_alternative<double>(value); }
bool Json::is_string() const { return std::holds_alternative<std::string>(value); }
bool Json::is_array()  const { return std::holds_alternative<array>(value); }
bool Json::is_object() const { return std::holds_alternative<object>(value); }

/* =========================
   Accessors
   ========================= */

const std::string& Json::as_string() const {
    return std::get<std::string>(value);
}

double Json::as_number() const {
    return std::get<double>(value);
}

bool Json::as_bool() const {
    return std::get<bool>(value);
}

/* =========================
   Object / Array access
   ========================= */

Json& Json::operator[](const std::string& key) {
    if (!is_object())
        value = object{};
    return std::get<object>(value)[key];
}

const Json& Json::operator[](const std::string& key) const {
    return std::get<object>(value).at(key);
}

Json& Json::operator[](size_t index) {
    return std::get<array>(value).at(index);
}

const Json& Json::operator[](size_t index) const {
    return std::get<array>(value).at(index);
}

/* =========================
   Serialization
   ========================= */

std::string Json::dump() const {
    if (is_null()) return "null";

    if (auto b = std::get_if<bool>(&value))
        return *b ? "true" : "false";

    if (auto n = std::get_if<double>(&value)) {
        std::ostringstream oss;
        oss << *n;
        return oss.str();
    }

    if (auto s = std::get_if<std::string>(&value))
        return "\"" + *s + "\"";

    if (auto a = std::get_if<array>(&value)) {
        std::string r = "[";
        for (size_t i = 0; i < a->size(); ++i) {
            if (i) r += ",";
            r += (*a)[i].dump();
        }
        return r + "]";
    }

    if (auto o = std::get_if<object>(&value)) {
        std::string r = "{";
        bool first = true;
        for (const auto& [k, v] : *o) {
            if (!first) r += ",";
            first = false;
            r += "\"" + k + "\":" + v.dump();
        }
        return r + "}";
    }

    return "";
}

/* =========================
   Parser (internal)
   ========================= */

class Parser {
public:
    explicit Parser(const std::string& text) : s(text) {}

    Json parse() {
        skip_ws();
        Json result = parse_value();
        skip_ws();
        if (pos != s.size())
            throw std::runtime_error("Trailing characters in JSON");
        return result;
    }

private:
    const std::string& s;
    size_t pos = 0;

    void skip_ws() {
        while (pos < s.size() && std::isspace(static_cast<unsigned char>(s[pos])))
            ++pos;
    }

    Json parse_value() {
        if (pos >= s.size())
            throw std::runtime_error("Unexpected end of JSON");

        char c = s[pos];

        if (c == '"') return parse_string();
        if (c == '{') return parse_object();
        if (c == '[') return parse_array();
        if (std::isdigit(c) || c == '-') return parse_number();

        if (s.compare(pos, 4, "true") == 0)  { pos += 4; return Json(true); }
        if (s.compare(pos, 5, "false") == 0) { pos += 5; return Json(false); }
        if (s.compare(pos, 4, "null") == 0)  { pos += 4; return Json(nullptr); }

        throw std::runtime_error("Invalid JSON value");
    }

    Json parse_string() {
        ++pos; // skip opening "
        std::string result;

        while (pos < s.size() && s[pos] != '"') {
            result += s[pos++];
        }

        if (pos >= s.size())
            throw std::runtime_error("Unterminated string");

        ++pos; // closing "
        return Json(result);
    }
    

    Json parse_number() {
        size_t start = pos;
        while (pos < s.size() &&
            (std::isdigit(s[pos]) || s[pos] == '-' || s[pos] == '.' ||
            s[pos] == 'e' || s[pos] == 'E' || s[pos] == '+')) // handle exponent
            ++pos;

        return Json(std::stod(s.substr(start, pos - start)));
    }


    Json parse_array() {
        Json::array arr;
        ++pos; // '['
        skip_ws();

        if (pos < s.size() && s[pos] == ']') {
            ++pos;
            return arr;
        }

        while (true) {
            arr.push_back(parse_value());
            skip_ws();

            if (pos >= s.size())
                throw std::runtime_error("Unterminated array");

            if (s[pos] == ']') {
                ++pos;
                break;
            }

            if (s[pos] != ',')
                throw std::runtime_error("Expected ',' in array");

            ++pos;
            skip_ws();
        }

        return arr;
    }

    Json parse_object() {
        Json::object obj;
        ++pos; // '{'
        skip_ws();

        if (pos < s.size() && s[pos] == '}') {
            ++pos;
            return obj;
        }

        while (true) {
            if (s[pos] != '"')
                throw std::runtime_error("Expected string key");

            std::string key = parse_string().as_string();
            skip_ws();

            if (s[pos] != ':')
                throw std::runtime_error("Expected ':' after key");

            ++pos;
            skip_ws();

            obj[key] = parse_value();
            skip_ws();

            if (s[pos] == '}') {
                ++pos;
                break;
            }

            if (s[pos] != ',')
                throw std::runtime_error("Expected ',' in object");

            ++pos;
            skip_ws();
        }

        return obj;
    }
};

/* =========================
   Public parse()
   ========================= */

Json Json::parse(const std::string& text) {
    if(text == "") return {};
    return Parser(text).parse();
}

} // namespace myjson
