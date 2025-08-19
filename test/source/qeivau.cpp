#include "qeivau/qeivau.h"

#include <doctest/doctest.h>

using namespace qeivau;

TEST_CASE("QeiVau set and get") {
  QeiVau store;
  store.set("foo", "bar");
  store.set("name", "paulo chaves");
  CHECK(store.get("foo").value() == "bar");
  CHECK(store.get("name").value() == "paulo chaves");
  CHECK(!store.get("baz"));
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
