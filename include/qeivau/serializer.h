#pragma once
#include <map>
#include <sstream>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "qeivau/util.h"

namespace qeivau {
  // Serialization interface for custom types
  struct Serializable {
    virtual std::string serialize() const = 0;
    virtual void deserialize(const std::string&) = 0;
    virtual ~Serializable() = default;
  };

  // Helper for types that are trivially serializable (int, float, std::string)
  template <typename T> struct DefaultSerializer {
    static std::string serialize(const T& value) {
      if constexpr (std::is_same_v<T, std::string>)
        return value;
      else
        return std::to_string(value);
    }
    static T deserialize(const std::string& str) {
      if constexpr (std::is_same_v<T, int>) {
        return std::stoi(str);
      } else if constexpr (std::is_same_v<T, float>) {
        return std::stof(str);
      } else if constexpr (std::is_same_v<T, std::string>) {
        return str;
      } else {
        static_assert(sizeof(T) == 0, "No default serializer for this type.");
      }
    }
  };

  // List serializer for std::vector<T>
  template <typename T> struct ListSerializer {
    static std::string serialize(const std::vector<T>& vec) {
      std::ostringstream oss;
      oss << "[";
      for (std::size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) oss << ",";
        oss << DefaultSerializer<T>::serialize(vec[i]);
      }
      oss << "]";
      return oss.str();
    }
    static std::vector<T> deserialize(const std::string& str) {
      std::vector<T> vec;
      std::string s = str;

      if (s.empty()) return vec;
      if (s.front() != '[' || s.back() != ']') {
        throw std::invalid_argument("Invalid list format: " + str);
      }

      s = s.substr(1, s.size() - 2);
      std::size_t start = 0, end = 0;

      while ((end = s.find(',', start)) != std::string::npos) {
        std::string item = trim_spaces(s.substr(start, end - start));
        vec.push_back(DefaultSerializer<T>::deserialize(item));
        start = end + 1;
      }
      std::string last = trim_spaces(s.substr(start));
      if (!last.empty()) vec.push_back(DefaultSerializer<T>::deserialize(last));
      return vec;
    }
  };

  template <typename T> struct MapSerializer {
    static std::string serialize(const std::map<std::string, T>& m) {
      std::ostringstream oss;
      oss << "{";
      bool first = true;
      for (const auto& [k, v] : m) {
        if (!first) oss << ",";
        oss << k << ":" << DefaultSerializer<T>::serialize(v);
        first = false;
      }
      oss << "}";
      return oss.str();
    }
    static std::map<std::string, T> deserialize(const std::string& str) {
      std::map<std::string, T> m;
      std::string s = str;
      // Remove braces if present
      if (!s.empty() && s.front() == '{' && s.back() == '}') {
        s = s.substr(1, s.size() - 2);
      }
      std::size_t start = 0, end = 0;
      while ((end = s.find(',', start)) != std::string::npos) {
        std::string pair = trim_spaces(s.substr(start, end - start));
        auto colon_pos = pair.find(':');
        if (colon_pos != std::string::npos) {
          std::string key = trim_spaces(pair.substr(0, colon_pos));
          std::string value_str = trim_spaces(pair.substr(colon_pos + 1));
          m[key] = DefaultSerializer<T>::deserialize(value_str);
        }
        start = end + 1;
      }
      std::string last_pair = trim_spaces(s.substr(start));
      if (!last_pair.empty()) {
        auto colon_pos = last_pair.find(':');
        if (colon_pos != std::string::npos) {
          std::string key = trim_spaces(last_pair.substr(0, colon_pos));
          std::string value_str = trim_spaces(last_pair.substr(colon_pos + 1));
          m[key] = DefaultSerializer<T>::deserialize(value_str);
        }
      }
      return m;
    }
  };

  using Value
      = std::variant<std::string, std::vector<std::string>, std::map<std::string, std::string>>;

  template <typename Value> struct VariantSerializer {
    static std::string serialize(const Value& value) {
      if (std::holds_alternative<std::string>(value)) {
        return DefaultSerializer<std::string>::serialize(std::get<std::string>(value));
      } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return ListSerializer<std::string>::serialize(std::get<std::vector<std::string>>(value));
      } else if (std::holds_alternative<std::map<std::string, std::string>>(value)) {
        return MapSerializer<std::string>::serialize(
            std::get<std::map<std::string, std::string>>(value));
      }
      return "";
    }
    static Value deserialize(const std::string& str) {
      std::string s = trim_spaces(str);

      if (s.empty()) {
        return std::string("");
      } else if (s.front() == '"' || s.front() == '\'') {
        if (s.back() == s.front() && s.size() >= 2) {
          return DefaultSerializer<std::string>::deserialize(s.substr(1, s.size() - 2));
        } else {
          throw std::invalid_argument("Mismatched quotes in string: " + str);
        }
      } else if (s.front() == '[' || s.front() == '{') {
        if (s.back() != (s.front() == '[' ? ']' : '}')) {
          throw std::invalid_argument("Mismatched brackets in value: " + str);
        }
        if (s.front() == '[') {
          return ListSerializer<std::string>::deserialize(s);
        } else {
          return MapSerializer<std::string>::deserialize(s);
        }
      } else {
        return DefaultSerializer<std::string>::deserialize(s);
      }
    }
  };

}  // namespace qeivau
