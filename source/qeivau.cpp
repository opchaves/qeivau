#include "qeivau/qeivau.h"

#include <fstream>
#include <iostream>
#include <optional>

namespace qeivau {

  void QeiVau::set(const std::string& key, const int& value) { store_[key] = value; }
  void QeiVau::set(const std::string& key, const float& value) { store_[key] = value; }
  void QeiVau::set(const std::string& key, const std::string& value) { store_[key] = value; }

  std::optional<std::string> QeiVau::getString(const std::string& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
      if (auto str_ptr = std::get_if<std::string>(&it->second)) {
        return *str_ptr;
      }
    }
    return std::nullopt;
  }

  std::optional<int> QeiVau::getInt(const std::string& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
      if (auto int_ptr = std::get_if<int>(&it->second)) {
        return *int_ptr;
      }
    }
    return std::nullopt;
  }

  std::optional<float> QeiVau::getFloat(const std::string& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
      if (auto float_ptr = std::get_if<float>(&it->second)) {
        return *float_ptr;
      }
    }
    return std::nullopt;
  }

  bool QeiVau::remove(const std::string& key) { return store_.erase(key) > 0; }

  bool QeiVau::has(const std::string& key) const { return store_.find(key) != store_.end(); }

  std::vector<std::string> QeiVau::keys() const {
    std::vector<std::string> result;
    result.reserve(store_.size());
    for (const auto& pair : store_) {
      result.push_back(pair.first);
    }
    return result;
  }

  void QeiVau::persist(const std::string& filename) const {
    std::ofstream out(filename);
    if (!out) {
      throw std::runtime_error("Error opening file for writing: " + filename);
    }

    for (const auto& pair : store_) {
      out << pair.first << ":";
      if (std::holds_alternative<int>(pair.second)) {
        out << "int=" << std::get<int>(pair.second);
      } else if (std::holds_alternative<float>(pair.second)) {
        out << "float=" << std::get<float>(pair.second);
      } else if (std::holds_alternative<std::string>(pair.second)) {
        out << "string=" << std::get<std::string>(pair.second);
      }
      out << "\n";
    }
  }

  void QeiVau::load(const std::string& filename) {
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line)) {
      auto type_pos = line.find(':');
      auto eq_pos = line.find('=');
      if (type_pos != std::string::npos && eq_pos != std::string::npos && type_pos < eq_pos) {
        std::string key = line.substr(0, type_pos);
        std::string type = line.substr(type_pos + 1, eq_pos - type_pos - 1);
        std::string value = line.substr(eq_pos + 1);
        if (type == "int") {
          try {
            store_[key] = std::stoi(value);
          } catch (...) {
            // ignore parse error
          }
        } else if (type == "float") {
          try {
            store_[key] = std::stof(value);
          } catch (...) {
            // ignore parse error
          }
        } else if (type == "string") {
          store_[key] = value;
        }
      }
    }
  }

}  // namespace qeivau
