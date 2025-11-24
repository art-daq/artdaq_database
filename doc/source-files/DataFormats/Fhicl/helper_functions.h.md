# helper_functions.h

## File Overview

Declares utility functions for FHiCL parsing, formatting, and conversion operations.

**Location**: `/home/user/artdaq-database/artdaq-database/DataFormats/Fhicl/helper_functions.h`

## Helper Functions

Provides utilities for:
- String quoting/unquoting
- FHiCL syntax validation
- Type detection
- Format conversion helpers
- Comment/annotation processing

## Purpose

Supports FHiCL reader/writer with common operations like:
- Determining if strings need quoting
- Processing escape sequences
- Handling FHiCL-specific syntax

## Related Files

- **helper_functions.cpp** - Implementation
- **fhicl_reader.h** - Uses helpers for parsing
- **fhicl_writer.h** - Uses helpers for generation
