// artdaq-database C++ API Indexes Script
// ======================================
//
// Usage:
//   mongosh mongodb://host:port/database < create_api_indexes.js
//   mongosh mongodb://127.0.0.1:27017/teststand_db < create_api_indexes.js
//
// Description:
//   Creates the 4 indexes required by the C++ artdaq-database API.
//
// Indexes created:
//   1. idx_version_entities_name - CRITICAL: duplicate detection, findCompositionsContaining
//   2. idx_configurations_name - HIGH: configuration lookups (shared with GUI)
//   3. idx_entities_name - HIGH: entity lookups
//   4. idx_version_asc - MEDIUM: ascending version queries
//

print("");
print("=".repeat(80));
print("  ARTDAQ-DATABASE C++ API INDEXES SCRIPT");
print("=".repeat(80));
print("");
print("  This script will:");
print("    1. Drop existing API indexes (if present)");
print("    2. Create all 4 C++ API indexes");
print("");

// ============================================================================
// INDEX DEFINITIONS
// ============================================================================

const API_INDEX_DEFINITIONS = [
  {
    name: "idx_version_entities_name",
    keys: { "version": 1, "entities.name": 1 },
    options: { background: true },
    description: "CRITICAL: Duplicate detection, findCompositionsContaining"
  },
  {
    name: "idx_configurations_name",
    keys: { "configurations.name": 1 },
    options: { background: true },
    description: "Configuration lookups (shared with GUI)"
  },
  {
    name: "idx_entities_name",
    keys: { "entities.name": 1 },
    options: { background: true },
    description: "Entity lookups (findVersions, findEntities)"
  },
  {
    name: "idx_version_asc",
    keys: { "version": 1 },
    options: { background: true },
    description: "Ascending version for API queries"
  }
];

// ============================================================================
// MAIN
// ============================================================================

const collections = db.getCollectionNames().filter(name =>
  !name.startsWith("system.") && name !== "SystemMetadata"
);

if (collections.length === 0) {
  print("ERROR: No collections found in database.");
  print("Make sure you are connected to the correct database.");
  print("");
  print("Usage: mongosh mongodb://host:port/database < create_api_indexes.js");
  quit(1);
}

print(`Database: ${db.getName()}`);
print(`Collections found: ${collections.length}`);
print(`API indexes per collection: ${API_INDEX_DEFINITIONS.length}`);
print(`Excluded: system.*, SystemMetadata`);
print("");

let stats = {
  collectionsProcessed: 0,
  indexesDropped: 0,
  indexesCreated: 0,
  indexesFailed: 0,
  errors: []
};

const apiIndexNames = API_INDEX_DEFINITIONS.map(idx => idx.name);

collections.forEach(collName => {
  print("-".repeat(80));
  print(`Processing: ${collName}`);
  print("-".repeat(80));

  const coll = db[collName];

  print("  Dropping existing API indexes...");
  try {
    const existingIndexes = coll.getIndexes();
    let droppedCount = 0;

    existingIndexes.forEach(idx => {
      if (apiIndexNames.includes(idx.name)) {
        try {
          coll.dropIndex(idx.name);
          print(`    [DROP] ${idx.name}`);
          droppedCount++;
          stats.indexesDropped++;
        } catch (e) {
          print(`    [FAIL] Could not drop ${idx.name}: ${e.message}`);
          stats.errors.push(`${collName}: Drop ${idx.name} - ${e.message}`);
        }
      }
    });

    if (droppedCount === 0) {
      print("    (no API indexes to drop)");
    }
  } catch (e) {
    print(`    [ERROR] Failed to list indexes: ${e.message}`);
    stats.errors.push(`${collName}: List indexes - ${e.message}`);
  }

  print("  Creating API indexes...");
  API_INDEX_DEFINITIONS.forEach(indexDef => {
    try {
      coll.createIndex(indexDef.keys, {
        name: indexDef.name,
        ...indexDef.options
      });
      print(`    [OK]   ${indexDef.name}`);
      stats.indexesCreated++;
    } catch (e) {
      print(`    [FAIL] ${indexDef.name}: ${e.message}`);
      stats.indexesFailed++;
      stats.errors.push(`${collName}: Create ${indexDef.name} - ${e.message}`);
    }
  });

  stats.collectionsProcessed++;
  print("");
});

print("=".repeat(80));
print("  SUMMARY");
print("=".repeat(80));
print("");
print(`  Collections processed:  ${stats.collectionsProcessed}`);
print(`  API indexes dropped:    ${stats.indexesDropped}`);
print(`  API indexes created:    ${stats.indexesCreated}`);
print(`  API indexes failed:     ${stats.indexesFailed}`);
print("");

if (stats.errors.length > 0) {
  print("  ERRORS:");
  stats.errors.forEach(err => print(`    - ${err}`));
  print("");
}

const expectedIndexes = collections.length * API_INDEX_DEFINITIONS.length;
if (stats.indexesCreated === expectedIndexes) {
  print(`  Status: SUCCESS - All ${expectedIndexes} API indexes created.`);
} else {
  print(`  Status: PARTIAL - Created ${stats.indexesCreated}/${expectedIndexes} API indexes.`);
}

print("");
print("  API Index Definitions:");
API_INDEX_DEFINITIONS.forEach((idx, i) => {
  print(`    ${i + 1}. ${idx.name}`);
  print(`       ${idx.description}`);
});

print("");
print("=".repeat(80));
print("  C++ API INDEX CREATION COMPLETE");
print("=".repeat(80));
print("");
