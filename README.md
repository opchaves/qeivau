# QeiVau Key-Value Store

QeiVau is a simple, in-memory key-value store library and CLI tool implemented in C++.
It provides basic operations for managing string keys and values, as well as persistence to and from files.

---

## Features

### 1. Variant Key-Value Store Class (`qeivau::QeiVau`)

- **Variant-based values:** Supports storing values as a string, a list of strings (`std::vector<std::string>`), or a map of strings (`std::map<std::string, std::string>`), using a variant serializer.
- **Flexible serialization:** Built-in support for serializing and deserializing all supported types, easily extensible for new types.
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
- **Clear all keys and values**
  - `void clear();`
- **Persist to file**
  - `void persist(const std::string& filename) const;`
- **Load from file**
  - `void load(const std::string& filename);`

#### Example types

```cpp
using QeiVauStore = qeivau::QeiVau<std::string, qeivau::Value, qeivau::VariantSerializer<qeivau::Value>>;
```

## 2. Command-Line Interface (CLI)

The CLI provides an interactive shell for managing key-value pairs, supporting string,
list (`[item1,item2,...]`), and map (`{key1:value1,key2:value2,...}`) values. Keys are case-insensitive.

### Basic Usage

Start the CLI executable and enter commands interactively. Each command manipulates the in-memory store and provides feedback.

#### Commands

- `set <key> <value>`: Set a string value for a key.
- `set <key> [a,b,c]`: Set a list of strings for a key (use brackets and comma-separated values).
- `set <key> {k1:v1,k2:v2}`: Set a map of strings for a key (use braces and comma-separated key:value pairs).
- `get <key>`: Retrieve the value for a key (prints string, list, or map).
- `remove <key>`: Remove a key.
- `has <key>`: Check if a key exists.
- `keys`: List all keys.
- `save <filename>`: Save all data to a file.
- `load <filename>`: Load all data from a file.
- `clear`: Remove all data from the store.
- `help`: Show available commands.
- `exit`: Exit the CLI.

#### Example Session

```sh
> set name Alice
OK
> set fruits [apple,banana,cherry]
OK
> set config {mode:fast,debug:true}
OK
> get name
Alice
> get fruits
[apple,banana,cherry]
> get config
{mode:fast,debug:true}
> has name
true
> has fruits
true
> keys
name fruits config
> save store.env
Saved to store.env
> remove name
Removed
> get name
Key not found
> exit
```

## 3. Error Handling

- Comprehensive error handling for file operations, type mismatches, malformed input, and empty or whitespace-only keys.
- Exceptions are thrown for file open/write errors, deserialization errors, empty keys, and malformed lines.
- The CLI prints error messages for invalid commands, missing arguments, parsing errors, and runtime exceptions.

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

Persistence now uses a simplified format, omitting the type annotation. All values are stored as strings, lists, or maps, inferred from their syntax:

```
key=value
```

Examples:

- String: `name=Alice`
- List: `fruits=[apple,banana,cherry]`
- Map: `config={mode:fast,debug:true}`

Lists use square brackets and comma-separated values; maps use curly braces with comma-separated key:value pairs.

## 6. Build & Run

- Configure: `cmake -S standalone -B build/standalone`
- Build: `cmake --build build/standalone`
- Run: `./build/standalone/QeiVau`
  - You can also load a file at startup with `--filename <file>`
  - Use `--help` to display usage information and available options

Or use the provided `Makefile` targets:

- `make config-exe`
- `make build-exe`
- `make run`

---

## 7. Build and run test suite

Use the following commands from the project's root directory to run the test suite.

```bash
make test
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

- [ ] Add a command to allow pushing items to a list.
  - `ladd <key> <item>`: Add an item to the list stored at `<key>`.
- [ ] Add a command to allow removing items from a list
  - `ldel <key>:<index>`: Remove an item from the list stored at `<key>`.
- [ ] Allow changing an item in a list by index
