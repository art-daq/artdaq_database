// artdaq-database GUI Indexes Script
// ===================================
//
// Usage:
//   mongosh mongodb://host:port/database < create_gui_indexes.js
//   mongosh mongodb://127.0.0.1:27017/teststand_db < create_gui_indexes.js
//
// Description:
//   Creates the 8 indexes required by the DBBrowser GUI.
//
// Indexes created:
//   1. idx_version_desc - Descending version for GUI sorting
//   2. idx_bookkeeping_created - Timestamp sorting
//   3. idx_bookkeeping_status - Protection status filtering
//   4. idx_config_version_compound - Config + version queries
//   5. idx_collection_version - Duplicate detection
//   6. idx_config_summary_covering - Summary queries (4 fields)
//   7. idx_config_assignment_covering - Assignment queries (4 fields)
//   8. idx_full_summary_covering - Full summaries (6 fields)
//

print("");
print("=".repeat(80));
print("  ARTDAQ-DATABASE GUI INDEXES SCRIPT");
print("=".repeat(80));
print("");
print("  This script will:");
print("    1. Drop existing GUI indexes (if present)");
print("    2. Create all 8 GUI indexes");
print("");

// ============================================================================
// INDEX DEFINITIONS
// ============================================================================

const GUI_INDEX_DEFINITIONS = [
  {
    name: "idx_version_desc",
    keys: { "version": -1 },
    options: { background: true },
    description: "Descending version for GUI sorting (most recent first)"
  },
  {
    name: "idx_bookkeeping_created",
    keys: { "bookkeeping.created": -1 },
    options: { background: true },
    description: "Timestamp sorting for finding latest documents"
  },
  {
    name: "idx_bookkeeping_status",
    keys: { "bookkeeping.isdeleted": 1, "bookkeeping.isreadonly": 1 },
    options: { background: true },
    description: "Document protection status filtering"
  },
  {
    name: "idx_config_version_compound",
    keys: { "configurations.name": 1, "version": -1 },
    options: { background: true },
    description: "Configuration + version queries with descending sort"
  },
  {
    name: "idx_collection_version",
    keys: { "collection": 1, "version": -1 },
    options: { background: true },
    description: "Duplicate version detection queries"
  },
  {
    name: "idx_config_summary_covering",
    keys: {
      "configurations.name": 1,
      "collection": 1,
      "version": -1,
      "bookkeeping.created": -1
    },
    options: { background: true },
    description: "Covering index for get_documents_by_configuration_summary"
  },
  {
    name: "idx_config_assignment_covering",
    keys: {
      "configurations.name": 1,
      "configurations.assigned": -1,
      "collection": 1,
      "version": -1
    },
    options: { background: true },
    description: "Covering index for configuration assignment queries"
  },
  {
    name: "idx_full_summary_covering",
    keys: {
      "configurations.name": 1,
      "collection": 1,
      "version": -1,
      "bookkeeping.created": -1,
      "bookkeeping.isdeleted": 1,
      "bookkeeping.isreadonly": 1
    },
    options: { background: true },
    description: "Full covering index for summaries with protection status"
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
  print("Usage: mongosh mongodb://host:port/database < create_gui_indexes.js");
  quit(1);
}

print(`Database: ${db.getName()}`);
print(`Collections found: ${collections.length}`);
print(`GUI indexes per collection: ${GUI_INDEX_DEFINITIONS.length}`);
print(`Excluded: system.*, SystemMetadata`);
print("");

let stats = {
  collectionsProcessed: 0,
  indexesDropped: 0,
  indexesCreated: 0,
  indexesFailed: 0,
  errors: []
};

const guiIndexNames = GUI_INDEX_DEFINITIONS.map(idx => idx.name);

collections.forEach(collName => {
  print("-".repeat(80));
  print(`Processing: ${collName}`);
  print("-".repeat(80));

  const coll = db[collName];

  print("  Dropping existing GUI indexes...");
  try {
    const existingIndexes = coll.getIndexes();
    let droppedCount = 0;

    existingIndexes.forEach(idx => {
      if (guiIndexNames.includes(idx.name)) {
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
      print("    (no GUI indexes to drop)");
    }
  } catch (e) {
    print(`    [ERROR] Failed to list indexes: ${e.message}`);
    stats.errors.push(`${collName}: List indexes - ${e.message}`);
  }

  print("  Creating GUI indexes...");
  GUI_INDEX_DEFINITIONS.forEach(indexDef => {
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
print(`  GUI indexes dropped:    ${stats.indexesDropped}`);
print(`  GUI indexes created:    ${stats.indexesCreated}`);
print(`  GUI indexes failed:     ${stats.indexesFailed}`);
print("");

if (stats.errors.length > 0) {
  print("  ERRORS:");
  stats.errors.forEach(err => print(`    - ${err}`));
  print("");
}

const expectedIndexes = collections.length * GUI_INDEX_DEFINITIONS.length;
if (stats.indexesCreated === expectedIndexes) {
  print(`  Status: SUCCESS - All ${expectedIndexes} GUI indexes created.`);
} else {
  print(`  Status: PARTIAL - Created ${stats.indexesCreated}/${expectedIndexes} GUI indexes.`);
}

print("");
print("  GUI Index Definitions:");
GUI_INDEX_DEFINITIONS.forEach((idx, i) => {
  print(`    ${i + 1}. ${idx.name}`);
  print(`       ${idx.description}`);
});

print("");
print("=".repeat(80));
print("  GUI INDEX CREATION COMPLETE");
print("=".repeat(80));
print("");
