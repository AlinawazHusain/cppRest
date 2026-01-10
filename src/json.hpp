#pragma once

#include <string>
#include <variant>
#include <vector>
#include <map>

namespace myjson {

class Json {
public:
    using array  = std::vector<Json>;
    using object = std::map<std::string, Json>;

    // Constructors
    Json();                      // null
    Json(std::nullptr_t);
    Json(bool b);
    Json(int n);
    Json(double n);
    Json(const char* s);
    Json(const std::string& s);
    Json(const array& a);
    Json(const object& o);

    // Type checks
    bool is_null()   const;
    bool is_bool()   const;
    bool is_number() const;
    bool is_string() const;
    bool is_array()  const;
    bool is_object() const;

    // Accessors (safe, const-correct)
    const std::string& as_string() const;
    double as_number() const;
    bool as_bool() const;

    // Object / Array access
    Json& operator[](const std::string& key);
    const Json& operator[](const std::string& key) const;

    Json& operator[](size_t index);
    const Json& operator[](size_t index) const;

    // Serialization
    /**
     * @brief Return a string from json object
     */
    std::string dump() const;

    // Parsing
    /**
     * @brief Return a myjson::json object from the string of valid json type
     */
    static Json parse(const std::string& text);

private:
    using value_t = std::variant<
        std::nullptr_t,
        bool,
        double,
        std::string,
        array,
        object
    >;

    value_t value;
};

} // namespace myjson
