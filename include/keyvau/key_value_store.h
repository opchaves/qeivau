#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace keyvau {

  class KeyValueStore {
  public:
    // Set a value for a key
    void set(const std::string& key, const std::string& value);

    // Get the value for a key. Returns std::nullopt if not found
    std::optional<std::string> get(const std::string& key) const;

    // Remove a key. Returns true if removed, false if not found
    bool remove(const std::string& key);

    // Check if a key exists
    bool has(const std::string& key) const;

    // List all keys
    std::vector<std::string> keys() const;

  private:
    std::unordered_map<std::string, std::string> store_;
  };

}  // namespace keyvau
