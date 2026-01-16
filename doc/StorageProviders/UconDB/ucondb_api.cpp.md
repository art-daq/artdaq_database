# ucondb_api.cpp

**Path:** `artdaq-database/StorageProviders/UconDB/ucondb_api.cpp`

**Implements:** [ucondb_api.h](./ucondb_api.h.md)

**Purpose:** Implements the UconDB REST API client functions using libcurl for HTTP operations. This file provides the HTTP communication layer between the UconDB storage provider and the UconDB REST service, handling GET requests for data retrieval and PUT requests for data storage.

## Implementation Overview

This file implements all UconDB API functions using libcurl, handling:
- HTTP GET requests for reading folders, tags, objects, and object content
- HTTP PUT requests for uploading objects with metadata
- Response parsing (JSON arrays to `std::list<std::string>`)
- Error handling and timeout management
- HTTP Digest authentication for write operations

The implementation follows a consistent pattern: initialize curl handle, set options, perform request, parse response, cleanup handle, return result.

## Key Algorithms

### HTTP GET Pattern

Most read operations (`folders`, `tags`, `objects`, `get_object`) follow this pattern:

**Steps:**
1. Build URL by appending endpoint path and query parameters to base URL
2. Initialize libcurl handle with `curl_easy_init()`
3. Set options: write callback, timeout, follow redirects, max redirects, URL
4. Perform the request with `curl_easy_perform()`
5. Parse JSON array response into string list (for list functions)
6. Cleanup curl handle with `curl_easy_cleanup()`
7. Return results

**Why this approach:** Clean separation of concerns - each call manages its own curl handle, ensuring thread safety and preventing resource leaks.

### HTTP PUT Pattern (for `put_object`)

The write operation uses a different pattern to upload data:

**Steps:**
1. Build URL with all query parameters (folder, object, tv, tags, key)
2. Initialize libcurl handle
3. Set both read callback (for upload) and write callback (for response)
4. Configure for HTTP PUT mode (`CURLOPT_UPLOAD`)
5. Set content length (`CURLOPT_INFILESIZE_LARGE`)
6. Enable HTTP Digest authentication (`CURLAUTH_DIGEST`)
7. Set credentials from `provider->authentication()`
8. Perform the request
9. Check response for authorization errors
10. Cleanup and return result

### JSON Array Parsing

Response parsing uses simple string manipulation rather than a full JSON parser:

**Steps:**
1. Position stream to overwrite last `]` with space
2. Skip first `[` character
3. Split remaining content on commas
4. For each segment: trim whitespace, remove quotes with `dequote()`
5. Filter out empty strings and `"null"` values
6. Add valid strings to result list

**Why this approach:** Avoids dependency on a full JSON parser for simple array responses like `["item1", "item2", "item3"]`. The UconDB REST API returns consistently formatted simple JSON arrays.

## Static Callback Functions

### `data_write(void* buf, size_t size, size_t nmemb, void* userp) -> size_t`

**Brief:** libcurl write callback that writes received HTTP response data to an output stream. Called by libcurl when data is received from the server.

**Parameters:**
- `buf` - Pointer to buffer containing received data
- `size` - Size of each element in bytes
- `nmemb` - Number of elements received
- `userp` - User-defined pointer (cast to `std::ostream*`)

**Returns:** Number of bytes handled (size * nmemb on success, 0 on failure)

**Called by:** libcurl during HTTP response receipt

**Thread Safety:** Safe (operates on caller-provided stream)

**Implementation Details:**
- Casts `userp` to `std::ostream*`
- Writes `size * nmemb` bytes to the stream
- Returns bytes written on success
- Logs operation at TLOG level 20

---

### `data_read(void* buf, size_t size, size_t nmemb, void* userp) -> size_t`

**Brief:** libcurl read callback that provides data from an input stream for HTTP upload operations. Called by libcurl when it needs more data to send.

**Parameters:**
- `buf` - Buffer to fill with data to upload
- `size` - Size of each element
- `nmemb` - Number of elements the buffer can hold
- `userp` - User-defined pointer (cast to `std::istream*`)

**Returns:** Number of bytes read and placed in buffer

**Called by:** libcurl during HTTP PUT/POST upload

**Thread Safety:** Safe (operates on caller-provided stream)

**Implementation Details:**
- Casts `userp` to `std::istream*`
- Uses `readsome()` to read up to `size * nmemb` bytes
- Returns actual bytes read
- Logs operation at TLOG level 21

## Function Implementations

### `folders(const UconDBSPtr_t& provider) -> std::list<std::string>`

**Brief:** Retrieves list of all folders from UconDB server by making an HTTP GET request.

**REST Endpoint:** `GET /app/folders`

**Implementation Details:**
1. Builds URL: `{connection}/app/folders`
2. Configures curl with standard GET options
3. Parses JSON array response: `["folder1", "folder2", ...]`
4. Uses `dequote()` and `trim()` from SharedCommon for string cleanup

**Error Handling:**
- Logs CURLcode error via TLOG at level 12 if request fails
- Returns empty list on failure (does not throw)

**TLOG Levels Used:**
- Level 12: Begin/end logging, URL, results, errors

---

### `tags(const UconDBSPtr_t& provider, std::string const& folder) -> std::list<std::string>`

**Brief:** Retrieves tags for a specific folder via HTTP GET request.

**REST Endpoint:** `GET /app/tags?folder={folder}`

**Implementation Details:**
- Validates folder is not empty via `confirm()`
- Appends `?folder={folder}` query parameter to URL
- Same response parsing pattern as `folders()`

**TLOG Levels Used:**
- Level 13: Begin/end logging, folder parameter, URL, results, errors

---

### `objects(const UconDBSPtr_t& provider, std::string const& folder) -> std::list<std::string>`

**Brief:** Retrieves object names within a folder via HTTP GET request.

**REST Endpoint:** `GET /app/objects?folder={folder}`

**Implementation Details:**
- Validates folder is not empty via `confirm()`
- Filters out `"null"` values from response (UconDB may return null for empty slots)
- Logs individual objects at TLOG level 14

**TLOG Levels Used:**
- Level 14: Begin/end logging, folder parameter, individual objects, results, errors

---

### `get_object(const UconDBSPtr_t& provider, std::string const& folder, std::string const& object) -> result_t`

**Brief:** Retrieves an object's content from UconDB via HTTP GET request.

**REST Endpoint:** `GET /app/get?folder={folder}&object={object}`

**Implementation Details:**
- Validates both folder and object are not empty
- Returns raw response content (not parsed as JSON array)
- Uses `Success(retValue)` / `Failure(msg)` result constructors

**Return Values:**
- Success: `{true, object_content}` - raw string content of the object
- Failure: `{false, curl_error_message}` - human-readable error from curl

**TLOG Levels Used:**
- Level 15: Begin/end logging, folder/object parameters, URL, content received, errors

---

### `put_object(const UconDBSPtr_t& provider, ...) -> result_t`

**Brief:** Stores an object in UconDB via HTTP PUT request with optional versioning metadata.

**REST Endpoint:** `PUT /app/put?folder={folder}&object={object}[&tv={tv}][&tag={tags}][&key={key}]`

**Implementation Details:**
1. Builds URL using stringstream with all query parameters
2. Calculates content size for `CURLOPT_INFILESIZE_LARGE`
3. Enables HTTP Digest authentication via `CURLAUTH_DIGEST`
4. Enables HTTP PUT mode via `CURLOPT_UPLOAD`
5. Checks response body for "Authorization required" error

**Query Parameter Construction:**
| Parameter | Condition | Format |
|-----------|-----------|--------|
| `folder` | Always | Appended as-is |
| `object` | Always | Appended as-is |
| `tv` | If `tv != 0` | Converted to string with `std::to_string()` |
| `tag` | If `tags` not empty | Comma-separated, trailing comma removed via `seekp(-1)` |
| `key` | If `key` not empty | Appended as-is |

**Authentication:**
- Uses HTTP Digest auth (`CURLAUTH_DIGEST`)
- Credentials from `provider->authentication()` in `user:pass` format
- Set via `CURLOPT_USERPWD`

**TLOG Levels Used:**
- Level 16: Begin/end logging, parameters, URL, stream size, errors

---

### `create_folder(const UconDBSPtr_t& provider, std::string const& folder) -> result_t`

**Brief:** Creates a new folder with default permissions, checking if it already exists first.

**REST Endpoint:** `GET /app/create_folder?folder={folder}&read=read_only_user&write=db_writer&drop=no`

**Implementation Details:**
1. First calls `folders()` to check if folder already exists
2. If found (case-insensitive check using `to_lower()`), returns `Success(msg_AlreadyExist)`
3. If not found, makes create request with default permissions
4. After request, calls `folders()` again to verify creation
5. Returns appropriate success/failure based on verification

**Default Permissions:**
| Permission | Value | Meaning |
|------------|-------|---------|
| `read` | `read_only_user` | Default read role |
| `write` | `db_writer` | Default write role |
| `drop` | `no` | Cannot be dropped |

**TLOG Levels Used:**
- Level 15: Folder parameter logging
- Level 17: URL, creation status, errors

---

### `debug::UconDBAPI() -> void`

**Brief:** Enables TRACE debugging for all API operations by configuring maximum verbosity.

**Implementation Details:**
- Sets TRACE name to `"ucondb_api.cpp"`
- Enables all trace levels (0xFFFFFFFFFFFFFFFF for all 64 levels)
- Configures trace modes via `trace_mode::modeM` and `trace_mode::modeS`

**TLOG Level Used:**
- Level 10: Confirmation message that tracing is enabled

## libcurl Options Reference

| Option | Value | Purpose |
|--------|-------|---------|
| `CURLOPT_WRITEFUNCTION` | `data_write` | Response data handler callback |
| `CURLOPT_READFUNCTION` | `data_read` | Upload data source callback (PUT only) |
| `CURLOPT_NOPROGRESS` | `1L` | Disable progress meter output |
| `CURLOPT_FOLLOWLOCATION` | `1L` | Follow HTTP redirects automatically |
| `CURLOPT_FILE` | `&stringstream` | Output stream for response data |
| `CURLOPT_INFILE` | `&stringstream` | Input stream for upload data (PUT only) |
| `CURLOPT_TIMEOUT` | `provider->timeout()` | Request timeout in seconds (default: 5) |
| `CURLOPT_MAXREDIRS` | `10L` | Maximum number of redirects to follow |
| `CURLOPT_HTTPAUTH` | `CURLAUTH_DIGEST` | Authentication method (PUT/create only) |
| `CURLOPT_USERPWD` | `"user:pass"` | Authentication credentials |
| `CURLOPT_UPLOAD` | `1L` | Enable HTTP PUT mode |
| `CURLOPT_INFILESIZE_LARGE` | size | Upload content length |
| `CURLOPT_URL` | URL string | Target URL for request |

**Commented-out options (not currently used):**
- `CURLOPT_TCP_KEEPALIVE` - TCP keep-alive
- `CURLOPT_HTTPAUTH` for GET requests - authentication for read operations

## Dependencies

| Include | Purpose |
|---------|---------|
| `artdaq-database/SharedCommon/sharedcommon_common.h` | `trim()`, `dequote()`, `to_lower()`, `to_csv()` helpers |
| `artdaq-database/StorageProviders/UconDB/provider_ucondb.h` | `UconDBSPtr_t`, `UconDB` class |
| `artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h` | Additional UconDB types |
| `<curl/curl.h>` | libcurl HTTP client library |

## Performance Considerations

- **No connection reuse:** Each API call creates and destroys a curl handle; no connection pooling
- **Timeout:** Default 5 seconds per request; may be insufficient for large objects or slow networks
- **Memory buffering:** Large responses are fully buffered in memory before parsing
- **Folder existence check:** `create_folder()` makes O(n) call to `folders()` for existence check
- **Response parsing:** Simple string splitting is O(n) where n is response size

## Error Handling Strategy

1. **Preconditions:** `confirm()` validates required parameters (folder, object names not empty)
2. **CURL initialization:** Check `curl_easy_init()` returns non-null
3. **Option setting:** Chain of `CURLE_OK` checks for each `curl_easy_setopt()`
4. **Request execution:** Check return code from `curl_easy_perform()`
5. **HTTP-level errors:** Check response content for application errors (e.g., "Authorization required")
6. **Logging:** All errors logged via TLOG with descriptive messages
7. **Results:** Return `Failure(message)` with curl error string or application error

## Testing Notes

- **Unit tests:** Require running UconDB server or HTTP mock server
- **Key test cases:**
  - Successful GET operations for folders, tags, objects
  - Successful PUT with valid authentication
  - Network timeout handling (server unresponsive)
  - Invalid folder/object name handling
  - Authentication failure handling ("Authorization required")
  - Empty response handling
  - Redirect following (up to 10 hops)
  - Large object upload/download

## Maintenance Notes

### Connection Reuse

Currently, each API call creates a new curl handle. For better performance:
1. Use curl multi interface for concurrent requests
2. Implement connection pooling in UconDB class
3. Keep persistent curl handles with CURLOPT_TCP_KEEPALIVE
4. Consider libcurl's connection cache (`CURLOPT_MAXCONNECTS`)

### SSL/TLS Support

The code supports HTTPS through libcurl but does not explicitly configure:
- Certificate verification (`CURLOPT_SSL_VERIFYPEER`)
- SSL version requirements (`CURLOPT_SSLVERSION`)
- CA certificate bundle path (`CURLOPT_CAINFO`)

For production HTTPS deployments, add explicit SSL configuration options.

### Response Parsing Robustness

The JSON array parsing is simple and may fail on:
- Nested arrays or objects in response
- Escaped quotes in strings
- Unicode characters
- Whitespace variations

Consider using a proper JSON parser (e.g., from DataFormats/Json module) for more robust parsing.

### URL Encoding

Parameters are not URL-encoded before appending to URLs. This may cause issues with:
- Folder/object names containing special characters (`&`, `?`, `#`, spaces)
- Non-ASCII characters in names

Consider adding URL encoding for parameter values.

## See Also

- [ucondb_api.h](./ucondb_api.h.md) - Header file with function declarations
- [provider_ucondb.h](./provider_ucondb.h.md) - UconDB provider class definition
- [provider_ucondb_readwrite.cpp](./provider_ucondb_readwrite.cpp.md) - StorageProvider implementation using this API
- [provider_connection.cpp](./provider_connection.cpp.md) - Connection configuration implementation
- [SharedCommon/helper_functions.cpp](../../SharedCommon/helper_functions.cpp.md) - String helper functions used
- [External: libcurl Easy Interface](https://curl.se/libcurl/c/libcurl-easy.html) - libcurl documentation
- [External: UconDB Wiki](https://cdcvs.fnal.gov/redmine/projects/ucondb/wiki/Proposal) - UconDB REST protocol specification
