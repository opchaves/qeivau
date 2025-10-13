#pragma once
#include <sstream>
#include <string>
#include <type_traits>
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
      // Remove brackets if present
      if (!s.empty() && s.front() == '[' && s.back() == ']') {
        s = s.substr(1, s.size() - 2);
      }
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
}  // namespace qeivau
