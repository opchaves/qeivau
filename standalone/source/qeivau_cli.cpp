#include <cxxopts.hpp>
#include <iostream>
#include <sstream>
#include <string>

#include "qeivau/qeivau.h"

using namespace qeivau;

void print_help() {
  std::cout << "Commands:\n"
            << "  set <key> <value>\n"
            << "  get <key>\n"
            << "  remove <key>\n"
            << "  has <key>\n"
            << "  keys\n"
            << "  save <filename>\n"
            << "  load <filename>\n"
            << "  help\n"
            << "  exit\n";
}

auto main(int argc, char** argv) -> int {
  cxxopts::Options options(*argv, "A simple key-value store CLI");

  options.add_options()("h,help", "Show help");

  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    print_help();
    return 0;
  }

  QeiVau store;
  print_help();
  std::string line;
  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line)) break;
    if (line.empty()) continue;
    std::istringstream iss(line);
    std::string cmd, key, value;
    iss >> cmd;
    if (cmd == "exit") break;
    if (cmd == "set") {
      iss >> key;
      std::getline(iss, value);
      if (key.empty() || value.empty()) {
        std::cerr << "set requires <key> and <value>" << std::endl;
        continue;
      }
      auto trimmed_value = value;
      trimmed_value.erase(0, trimmed_value.find_first_not_of(" \t\n\r"));
      store.set(key, trimmed_value);
      std::cout << "OK" << std::endl;
    } else if (cmd == "get") {
      iss >> key;
      if (key.empty()) {
        std::cerr << "get requires <key>" << std::endl;
        continue;
      }
      auto result = store.get(key);
      if (result) {
        std::cout << *result << std::endl;
      } else {
        std::cout << "(not found)" << std::endl;
      }
    } else if (cmd == "remove") {
      iss >> key;
      if (key.empty()) {
        std::cerr << "remove requires <key>" << std::endl;
        continue;
      }
      if (store.remove(key)) {
        std::cout << "Removed" << std::endl;
      } else {
        std::cout << "(not found)" << std::endl;
      }
    } else if (cmd == "has") {
      iss >> key;
      if (key.empty()) {
        std::cerr << "has requires <key>" << std::endl;
        continue;
      }
      std::cout << (store.has(key) ? "true" : "false") << std::endl;
    } else if (cmd == "keys") {
      auto keys = store.keys();
      for (const auto& k : keys) {
        std::cout << k << " ";
      }
      std::cout << std::endl;
    } else if (cmd == "save") {
      std::string filename;
      iss >> filename;
      if (filename.empty()) {
        std::cerr << "save requires <filename>" << std::endl;
        continue;
      }
      store.persist(filename);
      std::cout << "Saved to " << filename << std::endl;
    } else if (cmd == "load") {
      std::string filename;
      iss >> filename;
      if (filename.empty()) {
        std::cerr << "load requires <filename>" << std::endl;
        continue;
      }
      store.load(filename);
      std::cout << "Loaded from " << filename << std::endl;
    } else if (cmd == "help") {
      print_help();
    } else {
      std::cout << "Unknown command. Type 'help'." << std::endl;
    }
  }
  return 0;
}
