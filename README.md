# QeiVau Key-Value Store

QeiVau is a simple, in-memory key-value store library and CLI tool implemented in C++.
It provides basic operations for managing string keys and values, as well as persistence to and from files.

---

## Features

### 1. Generic Key-Value Store Class (`qeivau::QeiVau`)

- **Template-based:** Supports any serializable type (e.g., `std::string`, `int`, `float`, `std::vector<std::string>`).
- **Custom serializers:** Built-in support for scalars and lists (`std::vector<string>`), easily extensible for other types.
- **Set a value**
  - `void set(const Key& key, const Value& value);`
- **Get a value**
  - `std::optional<Value> get(const Key& key) const;`
- **Remove a key**
  - `bool remove(const Key& key);`
- **Check key existence**
  - `bool has(const Key& key) const;`
- **List all keys**
  - `std::vector<Key> keys() const;`
- **Persist to file**
  - `void persist(const std::string& filename) const;`
- **Load from file**
  - `void load(const std::string& filename);`

#### Example types

```cpp
using StringStore = qeivau::QeiVau<std::string, std::string>;
using ListStore = qeivau::QeiVau<std::string, std::vector<std::string>, qeivau::ListSerializer<std::string>>;
```

---

## 2. Command-Line Interface (CLI)

The CLI provides an interactive shell for managing both string and list-of-string values.

### Supported Commands

- `set <key> <value>`: Set a string value for a key.
- `set <key> [a,b,c]`: Set a list of strings for a key (use brackets and comma-separated values).
- `get <key>`: Retrieve the value for a key (prints string or list).
- `remove <key>`: Remove a key.
- `has <key>`: Check if a key exists.
- `keys`: List all keys.
- `save <filename>`: Save all data (string and list values) to a file.
- `load <filename>`: Load all data from a file.
- `help`: Show available commands.
- `exit`: Exit the CLI.

### Usage Example

```sh
> set name Alice
OK
> set fruits [apple,banana,cherry]
OK
> get name
Alice
> get fruits
[apple,banana,cherry]
> has name
true
> has fruits
true
> keys
name
fruits
> save store.env
Saved to store.env
> remove name
Removed
> get name
Key not found
> exit
```

---

## 3. Error Handling

- Robust error handling for file operations, type mismatches, and malformed input.
- Exceptions are thrown for file errors, deserialization errors, and type mismatches.
- CLI prints error messages for invalid commands, missing arguments, or parsing errors.

---

## 4. Unit Tests

Unit tests (using [doctest](https://github.com/doctest/doctest)) cover:

- Setting and getting string and list values
- Removing keys
- Checking key existence
- Listing keys
- Persistence and loading for both types
- Error handling for malformed input and type mismatches

---

## 5. File Format

Persistence uses a unified format:

```
key:type=value
```

- For string: `name:string=Alice`
- For list: `fruits:string=[apple,banana,cherry]`

---

## 6. Build & Run

- Configure: `cmake -S standalone -B build/standalone`
- Build: `cmake --build build/standalone`
- Run: `./build/standalone/QeiVau`
  - You can also load a file at startup with `--filename <file>`
  - Use `--help` to display usage information and available options

Or use the provided `Makefile` targets:

- `make config-exe`
- `make build-exe`
- `make run-exe`

---

## 7. Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
cmake -S test -B build/test
cmake --build build/test
CTEST_OUTPUT_ON_FAILURE=1 cmake --build build/test --target test

# or simply call the executable:
./build/test/QeiVauTests
```

To collect code coverage information, run CMake with the `-DENABLE_TEST_COVERAGE=1` option.

---

## 8. Run clang-format

Use the following commands from the project's root directory to check and fix C++ and CMake source style.
This requires _clang-format_, _cmake-format_ and _pyyaml_ to be installed on the current system.

```bash
cmake -S test -B build/test

# view changes
cmake --build build/test --target format

# apply changes
cmake --build build/test --target fix-format
```

See [Format.cmake](https://github.com/TheLartians/Format.cmake) for details.
These dependencies can be easily installed using pip.

```bash
pip install clang-format==14.0.6 cmake_format==0.6.11 pyyaml
```

---

## 9. Build the documentation

The documentation is automatically built and [published](https://thelartians.github.io/ModernCppStarter) whenever a [GitHub Release](https://help.github.com/en/github/administering-a-repository/managing-releases-in-a-repository) is created.
To manually build documentation, call the following command.

```bash
cmake -S documentation -B build/doc
cmake --build build/doc --target GenerateDocs
# view the docs
open build/doc/doxygen/html/index.html
```

To build the documentation locally, you will need Doxygen, jinja2 and Pygments installed on your system.

---

## 10. Build everything at once

The project also includes an `all` directory that allows building all targets at the same time.
This is useful during development, as it exposes all subprojects to your IDE and avoids redundant builds of the library.

```bash
cmake -S all -B build
cmake --build build

# run tests
./build/test/QeiVauTests
# format code
cmake --build build --target fix-format
# run standalone
./build/standalone/QeiVau --help
# build docs
cmake --build build --target GenerateDocs
```

---

## 11. Dependencies

- C++17 or newer
- [cxxopts](https://github.com/jarro2783/cxxopts) for CLI argument parsing
- [doctest](https://github.com/doctest/doctest) for unit testing

---

## 12. Extensibility

The code is modular and can be extended with new types, custom serializers, value escaping, more complex file formats, or additional features.

---

### TODO

- [ ] Don't list commands when running the CLI
- [ ] Add a command to allow pushing items to a list.
  - `ladd <key> <item>`: Add an item to the list stored at `<key>`.
- [ ] Add a command to allow removing items from a list
  - `ldel <key>:<index>`: Remove an item from the list stored at `<key>`.
- [ ] Allow changing an item in a list by index
