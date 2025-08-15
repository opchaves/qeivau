#include "keyvau/key_value_store.h"
#include <optional>

namespace keyvau {

void KeyValueStore::set(const std::string& key, const std::string& value) {
    store_[key] = value;
}

std::optional<std::string> KeyValueStore::get(const std::string& key) const {
    auto it = store_.find(key);
    if (it != store_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool KeyValueStore::remove(const std::string& key) {
    return store_.erase(key) > 0;
}

bool KeyValueStore::has(const std::string& key) const {
    return store_.find(key) != store_.end();
}

std::vector<std::string> KeyValueStore::keys() const {
    std::vector<std::string> result;
    result.reserve(store_.size());
    for (const auto& pair : store_) {
        result.push_back(pair.first);
    }
    return result;
}

} // namespace keyvau
