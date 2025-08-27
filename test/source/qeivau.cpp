#include "qeivau/qeivau.h"

#include <doctest/doctest.h>

#include <fstream>
#include <iostream>

using namespace qeivau;

TEST_CASE("QeiVau set and get") {
  QeiVau store;
  store.set("foo", "bar");
  store.set("name", "paulo chaves");
  CHECK(store.getString("foo").value() == "bar");
  CHECK(store.getString("name").value() == "paulo chaves");
  CHECK(!store.getString("baz"));
}

TEST_CASE("QeiVau remove") {
  QeiVau store;
  store.set("a", "1");
  CHECK(store.remove("a"));
  CHECK(!store.has("a"));
  CHECK(!store.remove("a"));
}

TEST_CASE("QeiVau has") {
  QeiVau store;
  store.set("x", "y");
  CHECK(store.has("x"));
  CHECK(!store.has("z"));
}

TEST_CASE("QeiVau keys") {
  QeiVau store;
  store.set("one", "1");
  store.set("two", "2");
  auto keys = store.keys();
  CHECK(keys.size() == 2);
  CHECK((keys[0] == "one" || keys[0] == "two"));
  CHECK((keys[1] == "one" || keys[1] == "two"));
}

TEST_CASE("QeiVau persist and load") {
  QeiVau store;
  store.set("alpha", "1");
  store.set("beta", "2");
  store.set("gamma", 3);
  store.set("delta", 4.5f);
  std::string filename = "test_store.txt";
  store.persist(filename);

  QeiVau loaded;
  loaded.load(filename);
  CHECK(loaded.getString("alpha").value() == "1");
  CHECK(loaded.getString("beta").value() == "2");
  CHECK(loaded.getInt("gamma").value() == 3);
  CHECK(loaded.getFloat("delta").value() == 4.5f);
  CHECK(!loaded.getString("nonexistent"));

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error if file cannot be opened") {
  QeiVau store;
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
    out << "name:string=out\n";
    out << "age:int=30\n";
    out << "height:float=5.9\n";
    out << "invalid=nonsense\n";
  }

  try {
    QeiVau loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Malformed line: invalid=nonsense");
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error on invalid int") {
  std::string filename = "test_invalid_int_store.txt";
  {
    std::ofstream out(filename);
    out << "num:int=50\n";
    out << "age:int=thirty\n";
  }

  try {
    QeiVau loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Invalid int 'age': stoi: no conversion");
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error on invalid float") {
  std::string filename = "test_invalid_float_store.txt";
  {
    std::ofstream out(filename);
    out << "num:int=50\n";
    out << "height:float=five.nine\n";
  }

  try {
    QeiVau loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Invalid float 'height': stof: no conversion");
  }

  std::remove(filename.c_str());
}

TEST_CASE("QeiVau should throw error on unknown type") {
  std::string filename = "test_unknown_type_store.txt";
  {
    std::ofstream out(filename);
    out << "name:string=paulo\n";
    out << "value:double=3.14\n";
  }

  try {
    QeiVau loaded;
    loaded.load(filename);
  } catch (const std::exception& e) {
    CHECK(std::string(e.what()) == "Unknown type 'double' for key 'value'.");
  }

  std::remove(filename.c_str());
}
