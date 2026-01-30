// artdaq-database MongoDB Drop All Indexes Script
// ================================================
//
// Usage:
//   mongosh mongodb://host:port/database < drop_all_indexes.js
//   mongosh mongodb://127.0.0.1:27017/teststand_db < drop_all_indexes.js
//
// Description:
//   Drops ALL indexes (except _id) on all collections in the database.
//   Use this to clean up indexes before running the index creation script,
//   or to reset indexes to default state.
//
// Excluded collections:
//   - system.* (MongoDB system collections)
//   - SystemMetadata (artdaq-database metadata collection)

print("");
print("=".repeat(80));
print("  ARTDAQ-DATABASE DROP ALL INDEXES SCRIPT");
print("=".repeat(80));
print("");
print("  This script will:");
print("    1. Drop ALL existing indexes (except _id) on all collections");
print("");

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
  print("Usage: mongosh mongodb://host:port/database < drop_all_indexes.js");
  quit(1);
}

print(`Database: ${db.getName()}`);
print(`Collections found: ${collections.length}`);
print(`Excluded: system.*, SystemMetadata`);
print("");

let stats = {
  collectionsProcessed: 0,
  indexesDropped: 0,
  errors: []
};

collections.forEach(collName => {
  print("-".repeat(80));
  print(`Processing: ${collName}`);
  print("-".repeat(80));

  const coll = db[collName];

  print("  Dropping indexes...");
  try {
    const existingIndexes = coll.getIndexes();
    let droppedCount = 0;

    existingIndexes.forEach(idx => {
      if (idx.name !== "_id_") {
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
      print("    (no indexes to drop)");
    }
  } catch (e) {
    print(`    [ERROR] Failed to list/drop indexes: ${e.message}`);
    stats.errors.push(`${collName}: List indexes - ${e.message}`);
  }

  stats.collectionsProcessed++;
  print("");
});

print("=".repeat(80));
print("  SUMMARY");
print("=".repeat(80));
print("");
print(`  Collections processed:  ${stats.collectionsProcessed}`);
print(`  Indexes dropped:        ${stats.indexesDropped}`);
print("");

if (stats.errors.length > 0) {
  print("  ERRORS:");
  stats.errors.forEach(err => print(`    - ${err}`));
  print("");
}

if (stats.errors.length === 0) {
  print("  Status: SUCCESS - All indexes dropped.");
} else {
  print("  Status: PARTIAL - Some indexes could not be dropped.");
}

print("");
print("=".repeat(80));
print("  DROP INDEXES COMPLETE");
print("=".repeat(80));
print("");
