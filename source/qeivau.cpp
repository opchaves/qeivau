#include "qeivau/qeivau.h"
#include <optional>

namespace qeivau {

void QeiVau::set(const std::string& key, const std::string& value) {
    store_[key] = value;
}

std::optional<std::string> QeiVau::get(const std::string& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool QeiVau::remove(const std::string& key) {
    return store_.erase(key) > 0;
}

bool QeiVau::has(const std::string& key) const {
    return store_.find(key) != store_.end();
}

std::vector<std::string> QeiVau::keys() const {
    std::vector<std::string> result;
    result.reserve(store_.size());
    for (const auto& pair : store_) {
        result.push_back(pair.first);
    }
    return result;
}

} // namespace qeivau
