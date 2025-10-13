#include <cxxopts.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "qeivau/qeivau.h"

using QeiVauStore
    = qeivau::QeiVau<std::string, qeivau::Value, qeivau::VariantSerializer<qeivau::Value>>;

void print_help() {
  std::cout << "Commands:\n"
            << "  SET <key> <value>\n"
            << "  GET <key>\n"
            << "  DEL <key>\n"
            << "  HAS <key>\n"
            << "  KEYS\n"
            << "  SAVE <filename>\n"
            << "  CLEAR\n"
            << "  HELP\n"
            << "  EXIT\n";
}

void handle_set(std::string& key, std::string& value, QeiVauStore& store) {
  auto trimmed_value = qeivau::trim_spaces(value);
  store.set(key, qeivau::VariantSerializer<qeivau::Value>::deserialize(trimmed_value));
  std::cout << "OK" << std::endl;
}

void handle_get(std::string& key, QeiVauStore& store) {
  auto result = store.get(key);
  if (result) {
    std::cout << qeivau::VariantSerializer<qeivau::Value>::serialize(*result) << std::endl;
  } else {
    std::cout << "(not found)" << std::endl;
  }
}

void handle_del(std::string& key, QeiVauStore& store) {
  bool removed = store.remove(key);
  if (removed) {
    std::cout << "Removed" << std::endl;
  } else {
    std::cout << "(not found)" << std::endl;
  }
}

void handle_has(std::string& key, QeiVauStore& store) {
  bool found = store.has(key);
  std::cout << (found ? "true" : "false") << std::endl;
}

void handle_keys(QeiVauStore& store) {
  std::vector<std::string> all_keys = store.keys();
  for (const auto& k : all_keys) {
    std::cout << k << " ";
  }
  std::cout << std::endl;
}

void handle_save(std::string& filename, QeiVauStore& store) {
  store.persist(filename);
  std::cout << "Saved to " << filename << std::endl;
}

int main(int argc, char** argv) {
  cxxopts::Options options(*argv, "A simple key-value store CLI");
  options.add_options()("h,help", "Show help");
  options.add_options()("f,filename", "Specify the filename", cxxopts::value<std::string>());
  auto result = options.parse(argc, argv);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    print_help();
    return 0;
  }

  QeiVauStore qv_store;

  if (result.count("filename")) {
    qv_store.load(result["filename"].as<std::string>());
  }

  std::string line;

  while (true) {
    std::cout << "> ";
    if (!std::getline(std::cin, line)) break;
    if (line.empty()) continue;

    std::istringstream iss(line);
    std::string cmd, key, value;

    iss >> cmd;
    iss >> key;

    if (cmd == "EXIT" || cmd == "exit") {
      break;
    }
    if (cmd == "HELP" || cmd == "help") {
      print_help();
      continue;
    }
    if (cmd == "KEYS" || cmd == "keys") {
      handle_keys(qv_store);
      continue;
    }
    if (cmd == "CLEAR" || cmd == "clear") {
      qv_store = QeiVauStore();  // Reset the store
      std::cout << "Store cleared" << std::endl;
      continue;
    }

    if (key.empty()) {
      std::cerr << cmd << " requires <key>" << std::endl;
      continue;
    }

    if (cmd == "SET" || cmd == "set") {
      std::getline(iss, value);
      handle_set(key, value, qv_store);
    } else if (cmd == "GET" || cmd == "get") {
      handle_get(key, qv_store);
    } else if (cmd == "DEL" || cmd == "del") {
      handle_del(key, qv_store);
    } else if (cmd == "HAS" || cmd == "has") {
      handle_has(key, qv_store);
    } else if (cmd == "SAVE" || cmd == "save") {
      handle_save(key, qv_store);
    } else {
      std::cout << "Unknown command. Type 'HELP'." << std::endl;
    }
  }
  return 0;
}
