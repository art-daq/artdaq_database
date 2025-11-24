# ovlKeyValueWithDefault.h

## File Overview

Template class extending `ovlKeyValue` with automatic default value initialization and maskable comparison. Ensures that a JSON field exists with at least an empty object if not already present.

**Location**: `/home/user/artdaq-database/artdaq-database/Overlay/ovlKeyValueWithDefault.h`

## Purpose

Provides default initialization for optional JSON fields that should be objects, preventing access errors while supporting selective comparison through masking.

## Class Definition

```cpp
template <std::uint32_t mask>
class ovlKeyValueWithDefault : public ovlKeyValue {
 public:
  ovlKeyValueWithDefault(object_t::key_type const& key, value_t& object);

  // defaults
  ovlKeyValueWithDefault(ovlKeyValueWithDefault&&) = default;
  ~ovlKeyValueWithDefault() = default;

  // ops
  result_t operator==(ovlKeyValueWithDefault const&) const;

 private:
  bool init(value_t& parent);
  bool _initOK;
};
```

## Implementation

### Constructor
```cpp
template <std::uint32_t mask>
ovlKeyValueWithDefault<mask>::ovlKeyValueWithDefault(object_t::key_type const& key,
                                                      value_t& value)
    : ovlKeyValue(key, value), _initOK(init(value)) {}
```

### Initialization
```cpp
template <std::uint32_t mask>
bool ovlKeyValueWithDefault<mask>::init(value_t& parent) try {
  confirm(type(parent) == type_t::OBJECT);

  auto& obj = unwrap(parent).value_as<object_t>();

  if (obj.count(key()) == 0) obj[key()] = object_t{};
  confirm(obj.count(key()) == 1);

  return true;
} catch (...) {
  confirm(false);
  throw;
}
```

**Behavior**:
1. Confirms parent is an object
2. Checks if key exists in parent
3. If missing, creates empty object at key
4. Confirms field now exists

### Comparison
```cpp
template <std::uint32_t mask>
result_t ovlKeyValueWithDefault<mask>::operator==(ovlKeyValueWithDefault const& other) const {
  return ((useCompareMask() & mask) == mask) ? Success() : self() == other.self();
}
```

## Usage

### In ovlBookkeeping.h
The template is not directly used as a standalone type but its pattern is referenced.

### General Pattern
```cpp
// Hypothetical usage
template <std::uint32_t mask>
class MyOptionalField : public ovlKeyValueWithDefault<mask> {
 public:
  using ovlKeyValueWithDefault<mask>::ovlKeyValueWithDefault;
  // Additional methods...
};
```

## Design Rationale

### Default Empty Object

Creating an empty object prevents:
- Null pointer exceptions
- Missing key errors
- Complex existence checking code

### Template Pattern Benefits

1. **Type Safety** - Different masks create different types
2. **Compile-Time Configuration** - Mask is part of type signature
3. **Zero Runtime Cost** - Compiler can optimize mask checks

## Comparison with Similar Templates

| Template | Initialization | Use Case |
|----------|---------------|----------|
| `ovlKeyValueWithDefault` | Creates empty object | Optional object fields |
| `ovlKeyValueWithMask` | No initialization | Required existing fields |
| `ovlStringKeyValue` | Sets "not-provided" string | String fields |

## Related Files

- **ovlKeyValue.h** - Base class
- **ovlKeyValueWithMask.h** - Similar without initialization
- **ovlBookkeeping.h** - Pattern user
- **common.h** - Mask constants

## Best Practices

1. Use for optional metadata or bookkeeping fields
2. Let template handle initialization automatically
3. Choose appropriate mask for semantic meaning
4. Check _initOK in debug builds

## Notes

- Header-only template implementation
- Assumes parent is an object, not an array
- Creates nested object structure automatically
- Exception-safe initialization
- The empty object can be populated after creation
