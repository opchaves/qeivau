#include "qeivau/qeivau.h"

#include <doctest/doctest.h>

#include <fstream>
#include <iostream>

using StringStore = qeivau::QeiVau<std::string, std::string>;
using ListStore
    = qeivau::QeiVau<std::string, std::vector<std::string>, qeivau::ListSerializer<std::string>>;

TEST_CASE("QeiVau set and get (string)") {
  StringStore store;
  store.set("foo", "bar");
  store.set("name", "paulo chaves");
  CHECK(store.get("foo").value() == "bar");
  CHECK(store.get("name").value() == "paulo chaves");
  CHECK(!store.get("baz"));
}

TEST_CASE("QeiVau set and get (list)") {
  ListStore store;
  store.set("fruits", {"apple", "banana", "cherry"});
  store.set("empty", {});
  auto fruits = store.get("fruits");
  REQUIRE(fruits);
  CHECK(fruits->size() == 3);
  CHECK((*fruits)[0] == "apple");
  CHECK((*fruits)[1] == "banana");
  CHECK((*fruits)[2] == "cherry");
  auto empty = store.get("empty");
  REQUIRE(empty);
  CHECK(empty->empty());
  CHECK(!store.get("missing"));
}

TEST_CASE("QeiVau remove (string)") {
  StringStore store;
  store.set("a", "1");
  CHECK(store.remove("a"));
  CHECK(!store.has("a"));
  CHECK(!store.remove("a"));
}

TEST_CASE("QeiVau remove (list)") {
  ListStore store;
  store.set("letters", {"a", "b"});
  CHECK(store.remove("letters"));
  CHECK(!store.has("letters"));
  CHECK(!store.remove("letters"));
}

TEST_CASE("QeiVau has (string)") {
  StringStore store;
  store.set("x", "y");
  CHECK(store.has("x"));
  CHECK(!store.has("z"));
}

TEST_CASE("QeiVau has (list)") {
  ListStore store;
  store.set("nums", {"1", "2"});
  CHECK(store.has("nums"));
  CHECK(!store.has("other"));
}

TEST_CASE("QeiVau keys (string)") {
  StringStore store;
  store.set("one", "1");
  store.set("two", "2");
  auto keys = store.keys();
  CHECK(keys.size() == 2);
  CHECK((keys[0] == "one" || keys[0] == "two"));
  CHECK((keys[1] == "one" || keys[1] == "two"));
}

TEST_CASE("QeiVau keys (list)") {
  ListStore store;
  store.set("a", {"x"});
  store.set("b", {"y", "z"});
  auto keys = store.keys();
  CHECK(keys.size() == 2);
  CHECK((keys[0] == "a" || keys[0] == "b"));
  CHECK((keys[1] == "a" || keys[1] == "b"));
}

TEST_CASE("QeiVau persist and load (string)") {
  StringStore store;
  store.set("alpha", "1");
  store.set("beta", "2");
  store.set("gamma", "3");
  store.set("delta", "4.5");
  std::string filename = "test_store.txt";
  store.persist(filename);

  StringStore loaded;
  loaded.load(filename);
  CHECK(loaded.get("alpha").value() == "1");
  CHECK(loaded.get("beta").value() == "2");
  CHECK(loaded.get("gamma").value() == "3");
  CHECK(loaded.get("delta").value() == "4.5");
  CHECK(!loaded.get("nonexistent"));
  std::remove(filename.c_str());
}

TEST_CASE("QeiVau persist and load (list)") {
  ListStore store;
  store.set("letters", {"a", "b", "c"});
  store.set("empty", {});
  std::string filename = "test_list_store.txt";
  store.persist(filename);

  ListStore loaded;
  loaded.load(filename);
  auto letters = loaded.get("letters");
  REQUIRE(letters);
  CHECK(letters->size() == 3);
  CHECK((*letters)[0] == "a");
  CHECK((*letters)[1] == "b");
  CHECK((*letters)[2] == "c");
  auto empty = loaded.get("empty");
  REQUIRE(empty);
  CHECK(empty->empty());
  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error if file cannot be opened (string)") {
  StringStore store;
  try {
    store.load("non_existent_file.txt");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Could not open file: non_existent_file.txt");
  }
}

TEST_CASE("QeiVau should throw error if file cannot be opened (list)") {
  ListStore store;
  try {
    store.load("non_existent_file.txt");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Could not open file: non_existent_file.txt");
  }
}

TEST_CASE("QeiVau load with invalid key value (string)") {
  std::string filename = "test_invalid_store.txt";
  {
    std::ofstream out(filename);
    out << "invalid=nonsense\n";
  }

  try {
    StringStore loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Malformed line: invalid=nonsense");
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau load with invalid key value (list)") {
  std::string filename = "test_invalid_list_store.txt";
  {
    std::ofstream out(filename);
    out << "invalid:string=[a,b\n";  // missing closing bracket
  }

  try {
    ListStore loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()).find("Type mismatch for key 'invalid'") != std::string::npos);
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau load with type mismatch (list string)") {
  std::string filename = "test_type_mismatch_list.txt";
  {
    std::ofstream out(filename);
    out << "foo:int=42\n";  // type is int, but store expects list
  }

  try {
    ListStore loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()).find("Type mismatch") != std::string::npos);
  }

  std::remove(filename.c_str());
}
