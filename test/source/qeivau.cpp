#include "qeivau/qeivau.h"

#include <doctest/doctest.h>

#include <fstream>
#include <map>
#include <string>
#include <variant>
#include <vector>

using QeiVauStore
    = qeivau::QeiVau<std::string, qeivau::Value, qeivau::VariantSerializer<qeivau::Value>>;

TEST_CASE("QeiVau set and get (string)") {
  QeiVauStore store;
  store.set("foo", std::string("bar"));
  store.set("name", std::string("paulo chaves"));
  auto foo = store.get("foo");
  REQUIRE(foo);
  CHECK(std::get<std::string>(*foo) == "bar");
  auto name = store.get("name");
  REQUIRE(name);
  CHECK(std::get<std::string>(*name) == "paulo chaves");
  CHECK(!store.get("baz"));
}

TEST_CASE("QeiVau set and get (list)") {
  QeiVauStore store;
  store.set("fruits", std::vector<std::string>{"apple", "banana", "cherry"});
  store.set("empty", std::vector<std::string>{});
  auto fruits = store.get("fruits");
  REQUIRE(fruits);
  CHECK(std::get<std::vector<std::string>>(*fruits).size() == 3);
  CHECK(std::get<std::vector<std::string>>(*fruits)[0] == "apple");
  CHECK(std::get<std::vector<std::string>>(*fruits)[1] == "banana");
  CHECK(std::get<std::vector<std::string>>(*fruits)[2] == "cherry");
  auto empty = store.get("empty");
  REQUIRE(empty);
  CHECK(std::get<std::vector<std::string>>(*empty).empty());
  CHECK(!store.get("missing"));
}

TEST_CASE("QeiVau set and get (map)") {
  QeiVauStore store;
  store.set("config", std::map<std::string, std::string>{{"host", "localhost"}, {"port", "8080"}});
  auto config = store.get("config");
  REQUIRE(config);
  const auto& m = std::get<std::map<std::string, std::string>>(*config);
  CHECK(m.size() == 2);
  CHECK(m.at("host") == "localhost");
  CHECK(m.at("port") == "8080");
  CHECK(!store.get("missing"));
}

TEST_CASE("QeiVau remove") {
  QeiVauStore store;
  store.set("a", std::string("1"));
  store.set("b", std::vector<std::string>{"x"});
  store.set("c", std::map<std::string, std::string>{{"k", "v"}});
  CHECK(store.remove("a"));
  CHECK(store.remove("b"));
  CHECK(store.remove("c"));
  CHECK(!store.has("a"));
  CHECK(!store.has("b"));
  CHECK(!store.has("c"));
  CHECK(!store.remove("a"));
}

TEST_CASE("QeiVau has") {
  QeiVauStore store;
  store.set("x", std::string("y"));
  store.set("nums", std::vector<std::string>{"1", "2"});
  store.set("env", std::map<std::string, std::string>{{"PATH", "/usr/bin"}});
  CHECK(store.has("x"));
  CHECK(store.has("nums"));
  CHECK(store.has("env"));
  CHECK(!store.has("z"));
  CHECK(!store.has("other"));
}

TEST_CASE("QeiVau keys") {
  QeiVauStore store;
  store.set("one", std::string("1"));
  store.set("two", std::vector<std::string>{"a", "b"});
  store.set("three", std::map<std::string, std::string>{{"k", "v"}});
  auto keys = store.keys();
  CHECK(keys.size() == 3);
  CHECK((keys[0] == "one" || keys[0] == "two" || keys[0] == "three"));
  CHECK((keys[1] == "one" || keys[1] == "two" || keys[1] == "three"));
  CHECK((keys[2] == "one" || keys[2] == "two" || keys[2] == "three"));
}

TEST_CASE("QeiVau persist and load") {
  QeiVauStore store;
  store.set("alpha", std::string("1"));
  store.set("beta", std::vector<std::string>{"x", "y"});
  store.set("gamma", std::map<std::string, std::string>{{"a", "b"}, {"c", "d"}});
  std::string filename = "build/test_variant_store.txt";
  store.persist(filename);

  QeiVauStore loaded;
  loaded.load(filename);
  auto alpha = loaded.get("alpha");
  REQUIRE(alpha);
  CHECK(std::get<std::string>(*alpha) == "1");
  auto beta = loaded.get("beta");
  REQUIRE(beta);
  CHECK(std::get<std::vector<std::string>>(*beta).size() == 2);
  CHECK(std::get<std::vector<std::string>>(*beta)[0] == "x");
  CHECK(std::get<std::vector<std::string>>(*beta)[1] == "y");
  auto gamma = loaded.get("gamma");
  REQUIRE(gamma);
  const auto& gm = std::get<std::map<std::string, std::string>>(*gamma);
  CHECK(gm.at("a") == "b");
  CHECK(gm.at("c") == "d");
  CHECK(!loaded.get("nonexistent"));
  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error if file cannot be opened") {
  QeiVauStore store;
  try {
    store.load("non_existent_file.txt");
    FAIL("Expected exception not thrown");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Could not open file: non_existent_file.txt");
  }
}

TEST_CASE("QeiVau load with missing key") {
  std::string filename = "build/test_invalid_variant_store.txt";
  {
    std::ofstream out(filename);
    out << "name=true\n";
    out << "=nonsense\n";
  }

  try {
    QeiVauStore loaded;
    loaded.load(filename);
    FAIL("Expected exception not thrown");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()).find("Key cannot be empty") != std::string::npos);
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau load with invalid list value") {
  std::string filename = "build/test_invalid_list_variant_store.txt";
  {
    std::ofstream out(filename);
    out << "invalid=[a,b\n";  // missing closing bracket
  }

  try {
    QeiVauStore loaded;
    loaded.load(filename);
    FAIL("Expected exception not thrown");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()).find("Mismatched brackets") != std::string::npos);
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau load with invalid map value") {
  std::string filename = "build/test_invalid_map_variant_store.txt";
  {
    std::ofstream out(filename);
    out << "invalid={key1:value1,key2\n";  // missing closing brace and value for key2
  }

  try {
    QeiVauStore loaded;
    loaded.load(filename);
    FAIL("Expected exception not thrown");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()).find("Mismatched brackets") != std::string::npos);
  }

  std::remove(filename.c_str());
}
