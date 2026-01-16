# confjsondb.cpp

**Path:** `artdaq-database/DataFormats/Conf/confjsondb.cpp`

**Implements:** [confjsondb.h](./confjsondb.h.md)

**Purpose:** Implementation of CONF to/from database JSON conversion with metadata handling. Orchestrates ConfReader, ConfWriter, JsonReader, and JsonWriter to provide complete conversion functionality with origin tracking.

## Implementation Overview

This file combines multiple components to provide high-level CONF format conversion:

- **conf_to_json**: Parses CONF, adds metadata, serializes to JSON string
- **json_to_conf**: Parses JSON, extracts document, generates CONF string

The implementation uses a pipeline pattern where each step processes the data and passes it to the next.

## Key Algorithms

### conf_to_json Conversion

**Steps:**
1. Validate inputs (confirm macros)
2. Create JSON root structure with `document` and `origin` nodes
3. Populate origin metadata (format: "conf", source: "conf_to_json", timestamp)
4. Use ConfReader to parse CONF into document node
5. Use JsonWriter to serialize complete structure to string
6. Swap result into output parameter if successful

**Why this approach:** The structured approach separates concerns - ConfReader handles parsing, while this function handles metadata and serialization orchestration.

### json_to_conf Conversion

**Steps:**
1. Validate inputs (confirm macros)
2. Use JsonReader to parse JSON string into AST
3. Extract document node using lambda helper
4. Use ConfWriter to generate CONF text from document
5. Swap result into output parameter if successful

## Internal Functions

### `conf_to_json()`

**Brief:** Orchestrates CONF parsing with metadata addition and JSON serialization.

**Implementation Pattern:**
```cpp
bool conf_to_json(std::string const& conf, std::string& json) {
    confirm(!conf.empty());
    confirm(json.empty());

    auto json_root = jsn::object_t{};
    json_root[literal::document] = jsn::object_t{};
    json_root[literal::origin] = jsn::object_t{};

    auto get_object = [&json_root](std::string const& name) -> auto& {
        return boost::get<jsn::object_t>(json_root[name]);
    };

    // Add origin metadata
    get_object(literal::origin)[literal::format] = std::string("conf");
    get_object(literal::origin)[literal::source] = std::string("conf_to_json");
    get_object(literal::origin)[literal::timestamp] = artdaq::database::timestamp();

    // Parse CONF into document node
    auto& json_node = get_object(literal::document);
    auto reader = conf::ConfReader{};
    result = reader.read(conf, json_node);

    // Serialize to JSON string
    auto writer = JsonWriter{};
    result = writer.write(json_root, json1);

    if (result) json.swap(json1);
    return result;
}
```

### `json_to_conf()`

**Brief:** Orchestrates JSON parsing with CONF generation.

**Implementation Pattern:**
```cpp
bool json_to_conf(std::string const& json, std::string& conf) {
    confirm(!json.empty());
    confirm(conf.empty());

    auto json_root = jsn::object_t{};
    auto reader = JsonReader{};
    result = reader.read(json, json_root);

    if (!result) {
        TLOG(13) << "json_to_conf: Unable to read JSON buffer";
        return result;
    }

    auto& json_node = get_object(literal::document);
    auto writer = ConfWriter{};
    result = writer.write(json_node, conf1);

    if (result) conf.swap(conf1);
    return result;
}
```

### `debug::enableConfJson()`

**Brief:** Enables TRACE logging for the conversion module.

## Performance Considerations

- **Multiple parsing/serialization:** Each conversion involves parse + serialize steps
- **String allocations:** Temporary strings created during conversion
- **Metadata overhead:** Origin object added to every conversion

## Error Handling Strategy

1. **Precondition validation:** Uses `confirm()` macro
2. **Step-by-step checking:** Each reader/writer call checked for success
3. **Early return:** Returns false immediately on failure
4. **TRACE logging:** Logs failures for debugging

## Testing Notes

- **Unit tests:** Would be in `test/DataFormats/Conf/`
- **Key test cases:**
  - Round-trip conversion (CONF -> JSON -> CONF)
  - Empty input handling
  - Malformed input handling

## Maintenance Notes

- **Lambda Pattern:** Uses lambda `get_object` for cleaner nested access
- **Swap Idiom:** Uses swap for exception-safe assignment
- **TRACE Levels:** Level 12 for conf_to_json, level 13 for json_to_conf
- **Metadata Fields:** Uses shared_literals.h constants for field names
- **Timestamp:** Calls `artdaq::database::timestamp()` for current time

### TRACE Logging

| Function | Level | Message |
|----------|-------|---------|
| conf_to_json | 12 | "conf_to_json: begin" |
| conf_to_json | 12 | "conf_to_json: end" |
| json_to_conf | 13 | "json_to_conf: begin" |
| json_to_conf | 13 | "json_to_conf: Unable to read JSON buffer" |
| json_to_conf | 13 | "json_to_conf: end" |
| enableConfJson | 10 | "artdaq::database::confjson trace_enable" |

## See Also

- [confjsondb.h](./confjsondb.h.md) - Function declarations
- [conf_reader.h](./conf_reader.h.md) - CONF parsing
- [conf_writer.h](./conf_writer.h.md) - CONF generation
- [../Json/json_common.h](../Json/json_common.h.md) - JsonReader, JsonWriter
