#include "qeivau/qeivau.h"

#include <doctest/doctest.h>

#include <fstream>
#include <iostream>

using Store = qeivau::QeiVau<std::string, std::string>;

TEST_CASE("QeiVau set and get") {
  Store store;
  store.set("foo", "bar");
  store.set("name", "paulo chaves");
  CHECK(store.get("foo").value() == "bar");
  CHECK(store.get("name").value() == "paulo chaves");
  CHECK(!store.get("baz"));
}

TEST_CASE("QeiVau remove") {
  Store store;
  store.set("a", "1");
  CHECK(store.remove("a"));
  CHECK(!store.has("a"));
  CHECK(!store.remove("a"));
}

TEST_CASE("QeiVau has") {
  Store store;
  store.set("x", "y");
  CHECK(store.has("x"));
  CHECK(!store.has("z"));
}

TEST_CASE("QeiVau keys") {
  Store store;
  store.set("one", "1");
  store.set("two", "2");
  auto keys = store.keys();
  CHECK(keys.size() == 2);
  CHECK((keys[0] == "one" || keys[0] == "two"));
  CHECK((keys[1] == "one" || keys[1] == "two"));
}

TEST_CASE("QeiVau persist and load") {
  Store store;
  store.set("alpha", "1");
  store.set("beta", "2");
  store.set("gamma", "3");
  store.set("delta", "4.5");
  std::string filename = "test_store.txt";
  store.persist(filename);

  Store loaded;
  loaded.load(filename);
  CHECK(loaded.get("alpha").value() == "1");
  CHECK(loaded.get("beta").value() == "2");
  CHECK(loaded.get("gamma").value() == "3");
  CHECK(loaded.get("delta").value() == "4.5");
  CHECK(!loaded.get("nonexistent"));
  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error if file cannot be opened") {
  Store store;
  try {
    store.load("non_existent_file.txt");
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Could not open file: non_existent_file.txt");
  }
}

TEST_CASE("QeiVau load with invalid key value") {
  std::string filename = "test_invalid_store.txt";
  {
    std::ofstream out(filename);
    out << "invalid=nonsense\n";
  }

  try {
    Store loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Malformed line: invalid=nonsense");
  }

  std::remove(filename.c_str());
}
