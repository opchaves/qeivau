#pragma once
#include <fstream>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <vector>

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

  template <typename Key, typename Value, typename Serializer = DefaultSerializer<Value>>
  class QeiVau {
  public:
    void set(const Key& key, const Value& value);
    std::optional<Value> get(const Key& key) const;
    bool remove(const Key& key);
    bool has(const Key& key) const;
    std::vector<Key> keys() const;
    void persist(const std::string& filename) const;
    void load(const std::string& filename);

  private:
    std::unordered_map<Key, Value> store_;
  };

  // Implementation of QeiVau template

  // Set a value for a key
  template <typename Key, typename Value, typename Serializer>
  void QeiVau<Key, Value, Serializer>::set(const Key& key, const Value& value) {
    store_[key] = value;
  }

  // Get the value for a key. Returns std::nullopt if not found
  template <typename Key, typename Value, typename Serializer>
  std::optional<Value> QeiVau<Key, Value, Serializer>::get(const Key& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
      return it->second;
    }
    return std::nullopt;
  }

  // Remove a key. Returns true if removed, false if not found
  template <typename Key, typename Value, typename Serializer>
  bool QeiVau<Key, Value, Serializer>::remove(const Key& key) {
    return store_.erase(key) > 0;
  }

  // Check if a key exists
  template <typename Key, typename Value, typename Serializer>
  bool QeiVau<Key, Value, Serializer>::has(const Key& key) const {
    return store_.find(key) != store_.end();
  }

  // List all keys
  template <typename Key, typename Value, typename Serializer>
  std::vector<Key> QeiVau<Key, Value, Serializer>::keys() const {
    std::vector<Key> result;
    result.reserve(store_.size());
    for (const auto& pair : store_) {
      result.push_back(pair.first);
    }
    return result;
  }

  // Helper to get type name as string
  template <typename T> std::string get_type_name() {
    if constexpr (std::is_same_v<T, int>)
      return "int";
    else if constexpr (std::is_same_v<T, float>)
      return "float";
    else if constexpr (std::is_same_v<T, std::string>)
      return "string";
    else
      return "unknown";
  }

  // Persist to file in key:type=value format
  template <typename Key, typename Value, typename Serializer>
  void QeiVau<Key, Value, Serializer>::persist(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) {
      throw std::runtime_error("Error opening file for writing: " + filename);
    }
    for (const auto& pair : store_) {
      out << pair.first << ":" << get_type_name<Value>() << "="
          << Serializer::serialize(pair.second) << "\n";
    }
  }

  // Load from file expecting key:type=value format
  template <typename Key, typename Value, typename Serializer>
  void QeiVau<Key, Value, Serializer>::load(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) {
      throw std::runtime_error("Could not open file: " + filename);
    }
    std::string line;
    while (std::getline(in, line)) {
      auto type_pos = line.find(':');
      auto eq_pos = line.find('=');
      if (type_pos != std::string::npos && eq_pos != std::string::npos && type_pos < eq_pos) {
        Key key = line.substr(0, type_pos);
        std::string type = line.substr(type_pos + 1, eq_pos - type_pos - 1);
        std::string value_str = line.substr(eq_pos + 1);
        if (type == get_type_name<Value>()) {
          try {
            Value value = Serializer::deserialize(value_str);
            store_[key] = value;
          } catch (const std::exception& e) {
            throw std::runtime_error("Deserialization error for key '" + key + "': " + e.what());
          }
        } else {
          throw std::runtime_error("Type mismatch for key '" + key + "': expected '"
                                   + get_type_name<Value>() + "', got '" + type + "'.");
        }
      } else {
        throw std::runtime_error("Malformed line: " + line);
      }
    }
  }

}  // namespace qeivau
