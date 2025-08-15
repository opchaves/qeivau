#include "keyvau/key_value_store.h"
#include <iostream>
#include <string>

using namespace keyvau;

void print_help() {
    std::cout << "Commands:\n"
              << "  set <key> <value>\n"
              << "  get <key>\n"
              << "  remove <key>\n"
              << "  has <key>\n"
              << "  keys\n"
              << "  help\n"
              << "  exit\n";
}

int main() {
    KeyValueStore store;
    std::string line;
    print_help();
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string cmd, key, value;
        iss >> cmd;
        if (cmd == "set") {
            iss >> key >> value;
            store.set(key, value);
            std::cout << "OK\n";
        } else if (cmd == "get") {
            iss >> key;
            auto result = store.get(key);
            if (result) {
                std::cout << *result << "\n";
            } else {
                std::cout << "(not found)\n";
            }
        } else if (cmd == "remove") {
            iss >> key;
            if (store.remove(key)) {
                std::cout << "Removed\n";
            } else {
                std::cout << "(not found)\n";
            }
        } else if (cmd == "has") {
            iss >> key;
            std::cout << (store.has(key) ? "true" : "false") << "\n";
        } else if (cmd == "keys") {
            auto keys = store.keys();
            for (const auto& k : keys) {
                std::cout << k << " ";
            }
            std::cout << "\n";
        } else if (cmd == "help") {
            print_help();
        } else if (cmd == "exit") {
            break;
        } else {
            std::cout << "Unknown command. Type 'help'.\n";
        }
    }
    return 0;
}
