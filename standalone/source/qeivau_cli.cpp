#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "qeivau/qeivau.h"

using StringStore = qeivau::QeiVau<std::string, std::string>;
using StringListStore
    = qeivau::QeiVau<std::string, std::vector<std::string>, qeivau::ListSerializer<std::string>>;

void print_help() {
  std::cout << "Commands:\n"
            << "  set <key> <value>\n"
            << "  get <key>\n"
            << "  remove <key>\n"
            << "  has <key>\n"
            << "  keys\n"
            << "  save <filename>\n"
            << "  help\n"
            << "  exit\n";
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

  StringStore string_store;
  StringListStore string_list_store;

  if (result.count("filename")) {
    try {
      std::ifstream in(result["filename"].as<std::string>());
      if (!in) {
        std::cerr << "Error loading file: Could not open file: "
                  << result["filename"].as<std::string>() << std::endl;
        return 1;
      }
      std::string line;
      while (std::getline(in, line)) {
        auto type_pos = line.find(':');
        auto eq_pos = line.find('=');
        if (type_pos != std::string::npos && eq_pos != std::string::npos && type_pos < eq_pos) {
          std::string key = line.substr(0, type_pos);
          std::string type = line.substr(type_pos + 1, eq_pos - type_pos - 1);
          std::string value_str = line.substr(eq_pos + 1);
          // Detect list by brackets
          if (!value_str.empty() && value_str.front() == '[' && value_str.back() == ']') {
            string_list_store.set(key, qeivau::ListSerializer<std::string>::deserialize(value_str));
          } else {
            string_store.set(key, value_str);
          }
        }
      }
    } catch (const std::exception& e) {
      std::cerr << "Error loading file: " << e.what() << std::endl;
      return 1;
    }
  }

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
      // Detect list input: [a,b,c]
      if (!trimmed_value.empty() && trimmed_value.front() == '[' && trimmed_value.back() == ']') {
        string_list_store.set(key, qeivau::ListSerializer<std::string>::deserialize(trimmed_value));
        std::cout << "OK (string list)" << std::endl;
      } else {
        string_store.set(key, trimmed_value);
        std::cout << "OK" << std::endl;
      }
    } else if (cmd == "get") {
      iss >> key;
      if (key.empty()) {
        std::cerr << "get requires <key>" << std::endl;
        continue;
      }
      auto result = string_store.get(key);
      if (result) {
        std::cout << *result << std::endl;
        continue;
      }
      auto list_result = string_list_store.get(key);
      if (list_result) {
        std::cout << "[";
        for (size_t i = 0; i < list_result->size(); ++i) {
          if (i > 0) std::cout << ",";
          std::cout << (*list_result)[i];
        }
        std::cout << "]" << std::endl;
        continue;
      }
      std::cout << "(not found)" << std::endl;
    } else if (cmd == "remove") {
      iss >> key;
      if (key.empty()) {
        std::cerr << "remove requires <key>" << std::endl;
        continue;
      }
      bool removed = string_store.remove(key) || string_list_store.remove(key);
      if (removed) {
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
      bool found = string_store.has(key) || string_list_store.has(key);
      std::cout << (found ? "true" : "false") << std::endl;
    } else if (cmd == "keys") {
      std::vector<std::string> all_keys;
      auto add_keys = [&](const std::vector<std::string>& keys) {
        for (const auto& k : keys) all_keys.push_back(k);
      };
      add_keys(string_store.keys());
      add_keys(string_list_store.keys());
      for (const auto& k : all_keys) {
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
      // Save all data in a single file in a single call
      std::ofstream out(filename);
      if (!out) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        continue;
      }
      // Save string values
      for (const auto& key : string_store.keys()) {
        auto val = string_store.get(key);
        if (val) {
          out << key << ":string=" << *val << "\n";
        }
      }
      // Save string list values
      for (const auto& key : string_list_store.keys()) {
        auto val = string_list_store.get(key);
        if (val) {
          out << key << ":string=" << qeivau::ListSerializer<std::string>::serialize(*val) << "\n";
        }
      }
      std::cout << "Saved to " << filename << std::endl;
    } else if (cmd == "help") {
      print_help();
    } else {
      std::cout << "Unknown command. Type 'help'." << std::endl;
    }
  }
  return 0;
}
