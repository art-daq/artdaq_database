# configuration_dbproviders.cpp

**Path:** `artdaq-database/ConfigurationDB/configuration_dbproviders.cpp`

**Implements:** [configuration_dbproviders.h](./configuration_dbproviders.h.md)

**Purpose:** Implements the `validate_dbprovider_name()` function that validates database provider names against the list of supported providers (filesystem, mongo, ucon). This validation ensures that only recognized storage backends are used for database operations.

## Implementation Overview

This implementation file provides a single validation function that checks provider names against the three supported storage backends. The function uses case-sensitive string comparison and throws an exception for invalid provider names.

## Key Algorithms

### Provider Name Validation

The validation algorithm performs the following steps:

**Steps:**
1. Assert that the provider string is not empty using `confirm()`
2. Compare the provider name against each valid provider literal using `db::not_equal()`
3. If no match is found, throw a `runtime_error` with the invalid provider name
4. Catch any exceptions, log them at TRACE level 10, and re-throw

**Why this approach:** The validation is performed using string comparison against predefined literals from the configuration API. This ensures consistency with other parts of the codebase that use the same literals and provides clear error messages for invalid inputs.

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/ConfigurationDB/configuration_dbproviders.h` | Header for this implementation |
| `artdaq-database/ConfigurationDB/configuration_common.h` | Common configuration definitions and namespace aliases |
| `artdaq-database/SharedCommon/shared_exceptions.h` | Exception types (`runtime_error`) |

## Internal Functions

### `validate_dbprovider_name(std::string const& provider) -> void`

**Brief:** Validates that the given provider name matches one of the three supported storage providers.

**Called by:** Detail layer functions before dispatching operations

**Purpose:** Ensures only valid provider names are used, providing clear error messages for invalid inputs.

**Implementation Details:**

```cpp
void cf::validate_dbprovider_name(std::string const& provider) try {
  confirm(!provider.empty());

  if (db::not_equal(provider, prov::filesystem) &&
      db::not_equal(provider, prov::mongo) &&
      db::not_equal(provider, prov::ucon)) {
    throw db::runtime_error("validate_dbprovider_name")
        << "Invalid database provider; database provider=" << provider << ".";
  }
} catch (std::exception& ex) {
  TLOG(10) << "validate_dbprovider_name() Error:" << ex.what();
  throw;
}
```

**Key Implementation Notes:**

1. **`confirm()` assertion**: Uses debug assertion to verify the provider is not empty. This catches programming errors during development but may be disabled in release builds.

2. **`db::not_equal()` comparison**: Uses the database utility function for string comparison, providing consistent comparison semantics across the codebase.

3. **Function-try block**: The entire function body is wrapped in a try-catch block for logging before re-throwing, ensuring all exceptions are traced.

4. **TRACE logging**: Errors are logged at level 10 before being re-thrown, aiding in debugging without losing exception context.

## Namespace Aliases

| Alias | Full Namespace | Purpose |
|-------|----------------|---------|
| `db` | `artdaq::database` | Database root namespace |
| `cf` | `artdaq::database::configuration` | Configuration namespace |
| `prov` | `artdaq::database::configapi::literal::provider` | Provider name literals |

## TRACE Configuration

The file defines its own TRACE_NAME for logging:

```cpp
#ifdef TRACE_NAME
#undef TRACE_NAME
#endif
#define TRACE_NAME "configuration_dbproviders.cpp"
```

This ensures log messages from this file are properly tagged for filtering and debugging.

## Performance Considerations

- **Constant time**: The function performs a fixed number of string comparisons (3 at most)
- **No memory allocation**: Uses references throughout, no dynamic memory allocation
- **Minimal overhead**: Suitable for use in hot paths before dispatch operations

## Error Handling Strategy

The function uses a two-layer error handling approach:

1. **Debug assertions** (`confirm()`): Catch programming errors like empty provider strings during development
2. **Exceptions**: Throw `runtime_error` for invalid provider names that should be handled at runtime

Errors are logged before being re-thrown to aid in debugging without losing exception context.

## Testing Notes

- **Unit tests:** Provider validation is tested in ConfigurationDB unit tests
- **Key test cases:**
  - Valid provider names: "filesystem", "mongo", "ucon"
  - Invalid provider names: "mongodb", "file", "MySQL", empty string
  - Case sensitivity: "Mongo" vs "mongo"

## Maintenance Notes

### Adding a New Provider

When adding a new storage provider, update this function:

```cpp
// Before: Only three providers
if (db::not_equal(provider, prov::filesystem) &&
    db::not_equal(provider, prov::mongo) &&
    db::not_equal(provider, prov::ucon)) {

// After: Add new provider check
if (db::not_equal(provider, prov::filesystem) &&
    db::not_equal(provider, prov::mongo) &&
    db::not_equal(provider, prov::ucon) &&
    db::not_equal(provider, prov::newprovider)) {
```

Also add the new provider literal to `configuraion_api_literals.h`:

```cpp
namespace provider {
constexpr auto mongo = "mongo";
constexpr auto filesystem = "filesystem";
constexpr auto ucon = "ucon";
constexpr auto newprovider = "newprovider";  // Add new provider
}  // namespace provider
```

## See Also

- [configuration_dbproviders.h](./configuration_dbproviders.h.md) - Header file with function declaration
- [shared_exceptions.h](../SharedCommon/shared_exceptions.h.md) - Exception types
- [configuraion_api_literals.h](../SharedCommon/configuraion_api_literals.h.md) - Provider name literals

## Usage in the Codebase

This function is called by the detail layer functions before dispatching operations to ensure the requested provider is valid:

```cpp
#include "artdaq-database/ConfigurationDB/configuration_dbproviders.h"
#include "artdaq-database/ConfigurationDB/configuration_common.h"

namespace cf = artdaq::database::configuration;
namespace apiliteral = artdaq::database::configapi::literal;

// Example usage in detail_managedocument.cpp
void detail::read_document(ManageDocumentOperation const& opts, std::string& result) {
  // Validate provider before dispatch
  cf::validate_dbprovider_name(opts.provider());

  // Now dispatch to provider-specific implementation
  if (opts.provider() == apiliteral::provider::filesystem) {
    filesystem::readDocument(opts, queryPayload);
  } else if (opts.provider() == apiliteral::provider::mongo) {
    mongo::readDocument(opts, queryPayload);
  } else if (opts.provider() == apiliteral::provider::ucon) {
    ucon::readDocument(opts, queryPayload);
  }
}
```

---

**Documentation generated for artdaq-database ConfigurationDB module**
