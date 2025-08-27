#pragma once
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using Value = std::variant<int, float, std::string>;

namespace qeivau {

  class QeiVau {
  public:
    // Set a value for a key
    void set(const std::string& key, const int& value);
    void set(const std::string& key, const float& value);
    void set(const std::string& key, const std::string& value);

    // Get the value for a key. Returns std::nullopt if not found
    std::optional<int> getInt(const std::string& key) const;
    std::optional<float> getFloat(const std::string& key) const;
    std::optional<std::string> getString(const std::string& key) const;

    // Remove a key. Returns true if removed, false if not found
    bool remove(const std::string& key);

    // Check if a key exists
    bool has(const std::string& key) const;

    // List all keys
    std::vector<std::string> keys() const;

    void persist(const std::string& filename) const;

    void load(const std::string& filename);

  private:
    std::unordered_map<std::string, Value> store_;
  };

}  // namespace qeivau
