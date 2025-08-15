#include <doctest/doctest.h>
#include <keyvau/key_value_store.h>
#include <string>

using namespace keyvau;

TEST_CASE("KeyValueStore set and get") {
    KeyValueStore store;
    store.set("foo", "bar");
    CHECK(store.get("foo").value() == "bar");
    CHECK(!store.get("baz"));
}

TEST_CASE("KeyValueStore remove") {
    KeyValueStore store;
    store.set("a", "1");
    CHECK(store.remove("a"));
    CHECK(!store.has("a"));
    CHECK(!store.remove("a"));
}

TEST_CASE("KeyValueStore has") {
    KeyValueStore store;
    store.set("x", "y");
    CHECK(store.has("x"));
    CHECK(!store.has("z"));
}

TEST_CASE("KeyValueStore keys") {
    KeyValueStore store;
    store.set("one", "1");
    store.set("two", "2");
    auto keys = store.keys();
    CHECK(keys.size() == 2);
    CHECK((keys[0] == "one" || keys[0] == "two"));
    CHECK((keys[1] == "one" || keys[1] == "two"));
}
