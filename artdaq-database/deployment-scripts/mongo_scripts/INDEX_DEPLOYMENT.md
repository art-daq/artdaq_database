# MongoDB Index Deployment Guide

This directory contains scripts for managing MongoDB indexes that optimize query performance for the artdaq-database C++ API and DBBrowser GUI.

## Quick Start

```bash
# Create indexes for C++ API (required for all deployments)
mongosh mongodb://host:port/database < create_api_indexes.js

# Create indexes for DBBrowser GUI (optional, if using GUI)
mongosh mongodb://host:port/database < create_gui_indexes.js

# Drop all indexes (cleanup/reset)
mongosh mongodb://host:port/database < drop_all_indexes.js

# Verify all index builds are complete
mongosh mongodb://host:port/database --quiet < verify_indexes.js
```

## Scripts Overview

| Script | Purpose | Indexes |
|--------|---------|---------|
| `create_api_indexes.js` | C++ API performance | 4 indexes |
| `create_gui_indexes.js` | DBBrowser GUI performance | 8 indexes |
| `drop_all_indexes.js` | Remove all indexes (except `_id`) | - |
| `verify_indexes.js` | Verify index builds complete | - |

All scripts:
- Are safe to be (re)run
- Exclude `system.*` and `SystemMetadata` collections
- Provide detailed progress output and summary statistics

---

## C++ API Indexes

These 4 indexes are required by the artdaq-database C++ API.

| # | Index Name | Keys | Purpose |
|---|------------|------|---------|
| 1 | `idx_version_entities_name` | `{version: 1, entities.name: 1}` | Duplicate detection, findCompositionsContaining |
| 2 | `idx_configurations_name` | `{configurations.name: 1}` | Configuration lookups |
| 3 | `idx_entities_name` | `{entities.name: 1}` | Entity lookups (findVersions, findEntities) |
| 4 | `idx_version_asc` | `{version: 1}` | Version-only queries |

### API Function to Index Mapping

```
findCompositionsContaining ──► idx_version_entities_name
                           └─► idx_version_asc (fallback)

writeDocument (dup check) ───► idx_version_entities_name

findConfigurations ──────────► idx_configurations_name

configurationComposition ────► idx_configurations_name

findVersions ────────────────► idx_entities_name
                           └─► idx_version_asc

findEntities ────────────────► idx_entities_name

readDocument ────────────────► idx_configurations_name
                           └─► idx_entities_name
                           └─► _id (automatic)
```

---

## GUI Indexes

These 8 indexes are used exclusively by the DBBrowser GUI application.

| # | Index Name | Keys | Purpose |
|---|------------|------|---------|
| 1 | `idx_version_desc` | `{version: -1}` | GUI version sorting (most recent first) |
| 2 | `idx_bookkeeping_created` | `{bookkeeping.created: -1}` | Timestamp sorting |
| 3 | `idx_bookkeeping_status` | `{bookkeeping.isdeleted: 1, bookkeeping.isreadonly: 1}` | Protection status filtering |
| 4 | `idx_config_version_compound` | `{configurations.name: 1, version: -1}` | Config + version queries |
| 5 | `idx_collection_version` | `{collection: 1, version: -1}` | Duplicate version detection |
| 6 | `idx_config_summary_covering` | `{configurations.name, collection, version, bookkeeping.created}` | Summary queries (4 fields) |
| 7 | `idx_config_assignment_covering` | `{configurations.name, configurations.assigned, collection, version}` | Assignment queries (4 fields) |
| 8 | `idx_full_summary_covering` | `{configurations.name, collection, version, bookkeeping.created, bookkeeping.isdeleted, bookkeeping.isreadonly}` | Full summary queries (6 fields) |

### GUI Method to Index Mapping

```
get_documents_by_configuration_summary ──► idx_configurations_name (API)
                                       └─► idx_config_summary_covering
                                       └─► idx_full_summary_covering

get_documents_by_configurations_summary_batch ──► idx_configurations_name (API)
                                              └─► idx_config_summary_covering
                                              └─► idx_full_summary_covering

get_document_versions_summary ───────────► idx_bookkeeping_status (projection)

get_configuration_info_optimized ────────► idx_configurations_name (API)
                                       └─► idx_config_assignment_covering

get_collection_info_optimized ───────────► idx_version_desc
                                       └─► idx_bookkeeping_created

find_duplicate_versions ─────────────────► idx_version_asc (API)
                                       └─► idx_version_desc

find_duplicate_collections_in_configs ───► idx_configurations_name (API)
                                       └─► idx_collection_version
                                       └─► idx_config_assignment_covering
```

---

## Deployment Instructions

### New Database Setup

```bash
# 1. Connect and verify database exists
mongosh mongodb://127.0.0.1:27017/mydb --eval "db.getCollectionNames()"

# 2. Create API indexes (always required)
mongosh mongodb://127.0.0.1:27017/mydb < create_api_indexes.js

# 3. Create GUI indexes (if using DBBrowser)
mongosh mongodb://127.0.0.1:27017/mydb < create_gui_indexes.js

# 4. Verify all index builds are complete
mongosh mongodb://127.0.0.1:27017/mydb --quiet < verify_indexes.js
```

### Existing Database (Index Refresh)

```bash
# Option A: Run scripts directly (they drop and recreate their own indexes)
mongosh mongodb://127.0.0.1:27017/mydb < create_api_indexes.js
mongosh mongodb://127.0.0.1:27017/mydb < create_gui_indexes.js
mongosh mongodb://127.0.0.1:27017/mydb --quiet < verify_indexes.js

# Option B: Full reset (drops ALL indexes first)
mongosh mongodb://127.0.0.1:27017/mydb < drop_all_indexes.js
mongosh mongodb://127.0.0.1:27017/mydb < create_api_indexes.js
mongosh mongodb://127.0.0.1:27017/mydb < create_gui_indexes.js
mongosh mongodb://127.0.0.1:27017/mydb --quiet < verify_indexes.js
```

### Verify Indexes

```bash
# Verify all index builds are complete and report status
mongosh mongodb://127.0.0.1:27017/mydb --quiet < verify_indexes.js

# The script will:
# - Check for in-progress index builds
# - Check for pending builds per collection
# - List all indexes on all collections
# - Report presence of expected API and GUI indexes
# - Exit with code 1 if builds still in progress, 0 if complete
```

#### Manual Verification (alternative)

```bash
# List all indexes on a collection
mongosh mongodb://127.0.0.1:27017/mydb --eval "db.SomeCollection.getIndexes()"

# Check index usage stats
mongosh mongodb://127.0.0.1:27017/mydb --eval "db.SomeCollection.aggregate([{\$indexStats:{}}])"
```

---

## Document Structure Reference

These indexes assume the artdaq-database document structure:

```javascript
{
  "_id": ObjectId("..."),
  "version": "v1_0",
  "collection": "ComponentA",
  "entities": [
    { "name": "entity_name" }
  ],
  "configurations": [
    { "name": "config_name", "assigned": ISODate("...") }
  ],
  "bookkeeping": {
    "created": ISODate("..."),
    "isdeleted": false,
    "isreadonly": false
  },
  // ... document data
}
```
