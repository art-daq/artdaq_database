// verify_indexes.js
// Verifies that all index builds have completed and reports index status
// Usage: mongosh mongodb://host:port/database < verify_indexes.js

const excludedCollections = ["system.", "SystemMetadata"];

function isExcluded(collName) {
    return excludedCollections.some(excluded => collName.startsWith(excluded));
}

print("=".repeat(70));
print("INDEX VERIFICATION REPORT");
print("Database: " + db.getName());
print("Timestamp: " + new Date().toISOString());
print("=".repeat(70));

print("\n[1/3] Checking for in-progress index builds...\n");

const currentOps = db.adminCommand({
    currentOp: true,
    $or: [
        { op: "command", "command.createIndexes": { $exists: true } },
        { op: "none", "msg": /^Index Build/ }
    ]
});

const indexBuilds = currentOps.inprog.filter(op =>
    op.command && op.command.createIndexes ||
    (op.msg && op.msg.includes("Index Build"))
);

if (indexBuilds.length > 0) {
    print("WARNING: " + indexBuilds.length + " index build(s) in progress:");
    print("-".repeat(70));
    indexBuilds.forEach(op => {
        print("  Collection: " + (op.command ? op.command.createIndexes : op.ns));
        if (op.msg) print("  Status: " + op.msg);
        if (op.progress) {
            const pct = ((op.progress.done / op.progress.total) * 100).toFixed(1);
            print("  Progress: " + pct + "% (" + op.progress.done + "/" + op.progress.total + ")");
        }
        print("");
    });
    print("\nINDEX BUILDS NOT COMPLETE - please wait and re-run verification.\n");
} else {
    print("OK: No index builds currently in progress.\n");
}

try {
    const buildInfo = db.adminCommand({ listIndexBuilds: 1 });
} catch (e) {
}

print("[2/3] Checking index build status per collection...\n");

const collections = db.getCollectionNames().filter(name => !isExcluded(name));
let pendingBuilds = [];

collections.forEach(collName => {
    try {
        const builds = db.runCommand({ listIndexBuilds: collName });
        if (builds.ok && builds.indexes && builds.indexes.length > 0) {
            pendingBuilds.push({
                collection: collName,
                builds: builds.indexes
            });
        }
    } catch (e) {
    }
});

if (pendingBuilds.length > 0) {
    print("WARNING: Collections with pending index builds:");
    print("-".repeat(70));
    pendingBuilds.forEach(item => {
        print("  " + item.collection + ":");
        item.builds.forEach(build => {
            print("    - " + build.name + " (" + JSON.stringify(build.spec.key) + ")");
        });
    });
    print("");
} else {
    print("OK: No pending index builds on any collection.\n");
}

print("[3/3] Index inventory by collection...\n");
print("-".repeat(70));

let totalCollections = 0;
let totalIndexes = 0;
let indexSummary = {};

collections.sort().forEach(collName => {
    const indexes = db.getCollection(collName).getIndexes();
    totalCollections++;
    totalIndexes += indexes.length;

    print("\nCollection: " + collName + " (" + indexes.length + " indexes)");

    indexes.forEach(idx => {
        const keyStr = JSON.stringify(idx.key);
        const flags = [];
        if (idx.unique) flags.push("unique");
        if (idx.sparse) flags.push("sparse");
        if (idx.background) flags.push("background");

        const flagStr = flags.length > 0 ? " [" + flags.join(", ") + "]" : "";
        print("  - " + idx.name + ": " + keyStr + flagStr);

        if (!indexSummary[idx.name]) {
            indexSummary[idx.name] = 0;
        }
        indexSummary[idx.name]++;
    });
});

print("\n" + "=".repeat(70));
print("SUMMARY");
print("=".repeat(70));
print("Total collections scanned: " + totalCollections);
print("Total indexes found: " + totalIndexes);

const expectedApiIndexes = [
    "idx_version_entities_name",
    "idx_configurations_name",
    "idx_entities_name",
    "idx_version_asc"
];

const expectedGuiIndexes = [
    "idx_version_desc",
    "idx_bookkeeping_created",
    "idx_bookkeeping_status",
    "idx_config_version_compound",
    "idx_collection_version",
    "idx_config_summary_covering",
    "idx_config_assignment_covering",
    "idx_full_summary_covering"
];

print("\nAPI Index Status:");
expectedApiIndexes.forEach(idx => {
    const count = indexSummary[idx] || 0;
    const status = count > 0 ? "PRESENT (" + count + " collections)" : "MISSING";
    print("  " + idx + ": " + status);
});

print("\nGUI Index Status:");
expectedGuiIndexes.forEach(idx => {
    const count = indexSummary[idx] || 0;
    const status = count > 0 ? "PRESENT (" + count + " collections)" : "MISSING";
    print("  " + idx + ": " + status);
});

print("\n" + "=".repeat(70));
if (indexBuilds.length === 0 && pendingBuilds.length === 0) {
    print("RESULT: ALL INDEX BUILDS COMPLETE");
    print("=".repeat(70));
} else {
    print("RESULT: INDEX BUILDS IN PROGRESS - VERIFICATION FAILED");
    print("=".repeat(70));
    quit(1);
}
