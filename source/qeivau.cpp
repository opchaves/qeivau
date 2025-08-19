#include "qeivau/qeivau.h"

#include <fstream>
#include <optional>

namespace qeivau {

  void QeiVau::set(const std::string& key, const std::string& value) { store_[key] = value; }

  std::optional<std::string> QeiVau::get(const std::string& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
      return it->second;
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
      out << pair.first << "=" << pair.second << "\n";
    }
  }

  void QeiVau::load(const std::string& filename) {
    std::ifstream in(filename);
    std::string line;
    while (std::getline(in, line)) {
      auto pos = line.find('=');
      if (pos != std::string::npos) {
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        store_[key] = value;
      }
    }
  }

}  // namespace qeivau
