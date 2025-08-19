# QeiVau Key-Value Store

QeiVau is a simple, in-memory key-value store library and CLI tool implemented in C++. It provides basic operations for managing string keys and values, as well as persistence to and from files.

---

## Features

### 1. Key-Value Store Class (`qeivau::QeiVau`)

- **Set a value**

  - `void set(const std::string& key, const std::string& value);`
  - Stores or updates the value for a given key.

- **Get a value**

  - `std::optional<std::string> get(const std::string& key) const;`
  - Retrieves the value for a key. Returns `std::nullopt` if the key does not exist.

- **Remove a key**

  - `bool remove(const std::string& key);`
  - Removes the key from the store. Returns `true` if the key was removed, `false` if not found.

- **Check key existence**

  - `bool has(const std::string& key) const;`
  - Returns `true` if the key exists, `false` otherwise.

- **List all keys**

  - `std::vector<std::string> keys() const;`
  - Returns a vector containing all keys in the store.

- **Persist to file**

  - `void persist(const std::string& filename) const;`
  - Saves all key-value pairs to a file, one per line in `key=value` format.

- **Load from file**
  - `void load(const std::string& filename);`
  - Loads key-value pairs from a file in `key=value` format.

---

## 2. Command-Line Interface (CLI)

The CLI provides an interactive shell for managing the key-value store.

### Supported Commands

- `set <key> <value>`: Set a value for a key.
- `get <key>`: Retrieve the value for a key.
- `remove <key>`: Remove a key.
- `has <key>`: Check if a key exists.
- `keys`: List all keys.
- `save <filename>`: Save the store to a file.
- `load <filename>`: Load the store from a file.
- `help`: Show available commands.
- `exit`: Exit the CLI.

### Usage Example

```sh
> set name Alice
OK
> get name
Alice
> has name
true
> keys
name
> save store.env
Saved to store.env
> remove name
Removed
> load store.env
Loaded from store.env
> keys
name
> exit
```

---

## 3. Error Handling

- File operations (`persist`, `load`) throw exceptions if the file cannot be opened or written.
- CLI prints error messages for invalid commands or missing arguments.

---

## 4. Unit Tests

Unit tests (using [doctest](https://github.com/doctest/doctest)) cover:

- Setting and getting values
- Removing keys
- Checking key existence
- Listing keys

---

## 5. File Format

Persistence uses a simple `.env`-like format:

```
key1=value1
key2=value2
```

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

-----

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

The code is modular and can be extended with new features, such as value escaping, more complex file formats, or additional
