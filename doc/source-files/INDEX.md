# artdaq-database Documentation Index

**Version:** 3.00.00
**Last Updated:** 2026-01-14

This is the master documentation index for the artdaq-database project, a configuration database implementation for the ARTDAQ data acquisition toolkit used in high-energy physics experiments.

---

## Quick Links

| Resource | Description |
|----------|-------------|
| [GLOSSARY.md](./GLOSSARY.md) | Standard terminology definitions |
| [ARCHITECTURE.md](./ARCHITECTURE.md) | System architecture overview |

---

## Quick Start by Role

| Goal | Start Here |
|------|------------|
| **Understand the architecture** | [ARCHITECTURE.md](./ARCHITECTURE.md) |
| **Use the C++ API** | [ConfigurationDB/configurationdbifc.h.md](./ConfigurationDB/configurationdbifc.h.md) |
| **Use command-line tools** | [Utilities/conftool.cc.md](./Utilities/conftool.cc.md) |
| **Use Python bindings** | [SWIGBindings/README.md](./SWIGBindings/README.md) |
| **Look up terminology** | [GLOSSARY.md](./GLOSSARY.md) |

---

## Architecture Overview

```
┌─────────────────────────────────────────────────────────────────┐
│                         Utilities                               │
│              (CLI tools: conftool, bulkloader, etc.)            │
└─────────────────────────────┬───────────────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────────┐
│                      ConfigurationDB                             │
│                   (Main API Entry Point)                         │
│         ConfigurationInterface, Database Operations              │
└─────────────────────────────┬───────────────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────────┐
│                    StorageProviders                              │
│     ┌─────────────┬─────────────────┬─────────────────┐         │
│     │ FileSystemDB │    MongoDB      │     UconDB      │         │
│     │ (JSON files) │ (Document DB)   │   (REST API)    │         │
│     └─────────────┴─────────────────┴─────────────────┘         │
└─────────────────────────────┬───────────────────────────────────┘
                              │
┌─────────────────────────────▼──────────┬────────────────────────┐
│              JsonDocument               │        Overlay         │
│         (Core document model)           │  (Type-safe wrappers)  │
└─────────────────────────────┬──────────┴────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────────┐
│                       DataFormats                                │
│     ┌─────────┬─────────┬─────────┬─────────┐                   │
│     │  JSON   │  FHiCL  │   XML   │  Conf   │                   │
│     └─────────┴─────────┴─────────┴─────────┘                   │
└─────────────────────────────┬───────────────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────────┐
│                       BasicTypes                                 │
│            (Data format wrappers: JsonData, FhiclData)          │
└─────────────────────────────┬───────────────────────────────────┘
                              │
┌─────────────────────────────▼───────────────────────────────────┐
│                      SharedCommon                                │
│         (Foundation: exceptions, helpers, result types)          │
└─────────────────────────────────────────────────────────────────┘
```

See [ARCHITECTURE.md](./ARCHITECTURE.md) for detailed architecture documentation.

---

## Module Summary

| Layer | Module | Purpose | Files |
|-------|--------|---------|-------|
| **Application** | [Utilities](./Utilities/README.md) | CLI tools (conftool, bulkloader) | 16 |
| **API** | [ConfigurationDB](./ConfigurationDB/README.md) | Main C++ API and operations | 56 |
| **Storage** | [StorageProviders](./StorageProviders/README.md) | Backend implementations | 27 |
| **Format** | [DataFormats](./DataFormats/README.md) | JSON, XML, FHiCL, Conf parsers | 54 |
| **Document** | [JsonDocument](./JsonDocument/README.md) | Core JSON document model | 10 |
| **Overlay** | [Overlay](./Overlay/README.md) | Type-safe document wrappers | 28 |
| **Types** | [BasicTypes](./BasicTypes/README.md) | Format wrapper types | 13 |
| **Foundation** | [SharedCommon](./SharedCommon/README.md) | Exceptions, helpers, utilities | 14 |
| **Build** | [BuildInfo](./BuildInfo/README.md) | Version information | 2 |
| **Bindings** | [SWIGBindings](./SWIGBindings/README.md) | Python API bindings | 3 |

**Total: 223 documented source files**

---

## Key Entry Points

### For C++ Developers

| Task | Header File | Documentation |
|------|-------------|---------------|
| Store/retrieve configurations | `ConfigurationDB/configurationdbifc.h` | [Link](./ConfigurationDB/configurationdbifc.h.md) |
| Build JSON documents | `JsonDocument/JSONDocumentBuilder.h` | [Link](./JsonDocument/JSONDocumentBuilder.h.md) |
| Access document metadata | `Overlay/ovlDocument.h` | [Link](./Overlay/ovlDocument.h.md) |
| Convert data formats | `BasicTypes/basictypes.h` | [Link](./BasicTypes/basictypes.h.md) |
| Parse FHiCL files | `DataFormats/Fhicl/fhicl_reader.h` | [Link](./DataFormats/Fhicl/fhicl_reader.h.md) |
| Use thread pool | `ConfigurationDB/Multitasker.h` | [Link](./ConfigurationDB/Multitasker.h.md) |

### For System Administrators

| Task | Tool | Documentation |
|------|------|---------------|
| Manage configurations | `conftool` | [Link](./Utilities/conftool.cc.md) |
| Bulk import | `bulkloader` | [Link](./Utilities/bulkloader.cc.md) |
| Bulk export | `bulkdownloader` | [Link](./Utilities/bulkdownloader.cc.md) |
| Migrate databases | `migrate_database` | [Link](./Utilities/migrate_database.cc.md) |
| Convert FHiCL to JSON | `fhicl2json` | [Link](./Utilities/fhicl2json.cc.md) |
| Rebuild index | `rebuild_database_index` | [Link](./Utilities/rebuild_database_index.cc.md) |

### For Python Developers

| Task | Module | Documentation |
|------|--------|---------------|
| Python API | `conftoolp` | [Link](./SWIGBindings/conftool.h.md) |
| Python wrapper script | `conftool.py` | [Link](./Utilities/conftool_py.md) |

---

## Module Documentation

### [SharedCommon](./SharedCommon/README.md) - Foundation Utilities

Foundation module providing utilities used by all other modules. No dependencies on other artdaq-database modules.

| File | Description |
|------|-------------|
| [common.h.md](./SharedCommon/common.h.md) | Common includes and namespace definitions |
| [sharedcommon_common.h.md](./SharedCommon/sharedcommon_common.h.md) | Module-level common header |
| [shared_exceptions.h.md](./SharedCommon/shared_exceptions.h.md) | Exception hierarchy |
| [shared_datatypes.h.md](./SharedCommon/shared_datatypes.h.md) | Common data type definitions |
| [returned_result.h.md](./SharedCommon/returned_result.h.md) | Result wrapper type for error handling |
| [returned_result.cpp.md](./SharedCommon/returned_result.cpp.md) | Result wrapper implementation |
| [helper_functions.h.md](./SharedCommon/helper_functions.h.md) | General utility functions |
| [helper_functions.cpp.md](./SharedCommon/helper_functions.cpp.md) | Helper functions implementation |
| [fileststem_functions.h.md](./SharedCommon/fileststem_functions.h.md) | Filesystem utility functions |
| [fileststem_functions.cpp.md](./SharedCommon/fileststem_functions.cpp.md) | Filesystem functions implementation |
| [printStackTrace.h.md](./SharedCommon/printStackTrace.h.md) | Stack trace utilities |
| [printStackTrace.cpp.md](./SharedCommon/printStackTrace.cpp.md) | Stack trace implementation |
| [process_exit_codes.h.md](./SharedCommon/process_exit_codes.h.md) | Standard process exit codes |
| [configuraion_api_literals.h.md](./SharedCommon/configuraion_api_literals.h.md) | API string literals |

---

### [BasicTypes](./BasicTypes/README.md) - Data Format Wrappers

Wrapper classes for different data formats (JSON, FHiCL, XML) providing type-safe handling.

| File | Description |
|------|-------------|
| [common.h.md](./BasicTypes/common.h.md) | Common includes for BasicTypes |
| [basictypes.h.md](./BasicTypes/basictypes.h.md) | Main header including all basic types |
| [data_json.h.md](./BasicTypes/data_json.h.md) | JsonData wrapper class |
| [data_json.cpp.md](./BasicTypes/data_json.cpp.md) | JsonData implementation |
| [data_json_fusion.h.md](./BasicTypes/data_json_fusion.h.md) | Boost.Fusion adaptation for JsonData |
| [data_fhicl.h.md](./BasicTypes/data_fhicl.h.md) | FhiclData wrapper class |
| [data_fhicl.cpp.md](./BasicTypes/data_fhicl.cpp.md) | FhiclData implementation |
| [data_fhicl_fusion.h.md](./BasicTypes/data_fhicl_fusion.h.md) | Boost.Fusion adaptation for FhiclData |
| [data_xml.h.md](./BasicTypes/data_xml.h.md) | XmlData wrapper class |
| [data_xml.cpp.md](./BasicTypes/data_xml.cpp.md) | XmlData implementation |
| [data_xml_fusion.h.md](./BasicTypes/data_xml_fusion.h.md) | Boost.Fusion adaptation for XmlData |
| [base64.h.md](./BasicTypes/base64.h.md) | Base64 encoding/decoding |
| [base64.cpp.md](./BasicTypes/base64.cpp.md) | Base64 implementation |

---

### [JsonDocument](./JsonDocument/README.md) - Core Document Model

Core JSON document model with builder pattern for constructing and manipulating documents.

| File | Description |
|------|-------------|
| [common.h.md](./JsonDocument/common.h.md) | Common includes for JsonDocument |
| [docrecord_literals.h.md](./JsonDocument/docrecord_literals.h.md) | Document field name literals |
| [docrecord_exceptions.h.md](./JsonDocument/docrecord_exceptions.h.md) | Document-specific exceptions |
| [JSONDocument.h.md](./JsonDocument/JSONDocument.h.md) | Main JSONDocument class |
| [JSONDocument.cpp.md](./JsonDocument/JSONDocument.cpp.md) | JSONDocument implementation |
| [JSONDocument_utils.cpp.md](./JsonDocument/JSONDocument_utils.cpp.md) | JSONDocument utility functions |
| [JSONDocumentBuilder.h.md](./JsonDocument/JSONDocumentBuilder.h.md) | Builder pattern for documents |
| [JSONDocumentBuilder.cpp.md](./JsonDocument/JSONDocumentBuilder.cpp.md) | Builder implementation |
| [JSONDocumentMigrator.h.md](./JsonDocument/JSONDocumentMigrator.h.md) | Document version migration |
| [JSONDocumentMigrator.cpp.md](./JsonDocument/JSONDocumentMigrator.cpp.md) | Migrator implementation |

---

### [Overlay](./Overlay/README.md) - Type-Safe Document Wrappers

Type-safe overlay classes for structured access to JSON document fields.

| File | Description |
|------|-------------|
| [common.h.md](./Overlay/common.h.md) | Common includes for Overlay |
| [JSONDocumentOverlay.h.md](./Overlay/JSONDocumentOverlay.h.md) | Main overlay include header |
| [ovlDocument.h.md](./Overlay/ovlDocument.h.md) | Document overlay class |
| [ovlDocument.cpp.md](./Overlay/ovlDocument.cpp.md) | Document overlay implementation |
| [ovlDatabaseRecord.h.md](./Overlay/ovlDatabaseRecord.h.md) | Database record overlay |
| [ovlDatabaseRecord.cpp.md](./Overlay/ovlDatabaseRecord.cpp.md) | Database record implementation |
| [ovlId.h.md](./Overlay/ovlId.h.md) | ID field overlay |
| [ovlId.cpp.md](./Overlay/ovlId.cpp.md) | ID overlay implementation |
| [ovlTimeStamp.h.md](./Overlay/ovlTimeStamp.h.md) | Timestamp overlay |
| [ovlTimeStamp.cpp.md](./Overlay/ovlTimeStamp.cpp.md) | Timestamp implementation |
| [ovlOrigin.h.md](./Overlay/ovlOrigin.h.md) | Origin metadata overlay |
| [ovlOrigin.cpp.md](./Overlay/ovlOrigin.cpp.md) | Origin implementation |
| [ovlBookkeeping.h.md](./Overlay/ovlBookkeeping.h.md) | Bookkeeping metadata overlay |
| [ovlBookkeeping.cpp.md](./Overlay/ovlBookkeeping.cpp.md) | Bookkeeping implementation |
| [ovlChangeLog.h.md](./Overlay/ovlChangeLog.h.md) | Changelog overlay |
| [ovlChangeLog.cpp.md](./Overlay/ovlChangeLog.cpp.md) | Changelog implementation |
| [ovlComment.h.md](./Overlay/ovlComment.h.md) | Comment overlay |
| [ovlComment.cpp.md](./Overlay/ovlComment.cpp.md) | Comment implementation |
| [ovlUpdate.h.md](./Overlay/ovlUpdate.h.md) | Update record overlay |
| [ovlUpdate.cpp.md](./Overlay/ovlUpdate.cpp.md) | Update implementation |
| [ovlKeyValue.h.md](./Overlay/ovlKeyValue.h.md) | Key-value pair overlay |
| [ovlKeyValue.cpp.md](./Overlay/ovlKeyValue.cpp.md) | Key-value implementation |
| [ovlStringKeyValue.h.md](./Overlay/ovlStringKeyValue.h.md) | String key-value overlay |
| [ovlKeyValueTimeStamp.h.md](./Overlay/ovlKeyValueTimeStamp.h.md) | Key-value with timestamp |
| [ovlKeyValueWithDefault.h.md](./Overlay/ovlKeyValueWithDefault.h.md) | Key-value with default |
| [ovlKeyValueWithMask.h.md](./Overlay/ovlKeyValueWithMask.h.md) | Key-value with mask |
| [ovlFixedList.h.md](./Overlay/ovlFixedList.h.md) | Fixed-size list overlay |
| [ovlMovableList.h.md](./Overlay/ovlMovableList.h.md) | Movable list overlay |

---

### [DataFormats](./DataFormats/README.md) - Format Converters

Format conversion between JSON, FHiCL, XML, and Conf formats.

| File | Description |
|------|-------------|
| [common.h.md](./DataFormats/common.h.md) | Common includes for DataFormats |
| [shared_literals.h.md](./DataFormats/shared_literals.h.md) | Shared string literals |
| [shared_types.h.md](./DataFormats/shared_types.h.md) | Shared type definitions |

#### [DataFormats/Json](./DataFormats/Json/README.md) - JSON Format

| File | Description |
|------|-------------|
| [json_common.h.md](./DataFormats/Json/json_common.h.md) | JSON common includes |
| [json_types.h.md](./DataFormats/Json/json_types.h.md) | JSON type definitions |
| [json_types.cpp.md](./DataFormats/Json/json_types.cpp.md) | JSON types implementation |
| [json_types_impl.h.md](./DataFormats/Json/json_types_impl.h.md) | JSON types implementation details |
| [json_reader.h.md](./DataFormats/Json/json_reader.h.md) | JSON parser/reader |
| [json_reader.cpp.md](./DataFormats/Json/json_reader.cpp.md) | JSON reader implementation |
| [json_writer.h.md](./DataFormats/Json/json_writer.h.md) | JSON writer/serializer |
| [json_writer.cpp.md](./DataFormats/Json/json_writer.cpp.md) | JSON writer implementation |
| [convertjson2guijson.h.md](./DataFormats/Json/convertjson2guijson.h.md) | JSON to GUI JSON converter |
| [convertjson2guijson.cpp.md](./DataFormats/Json/convertjson2guijson.cpp.md) | GUI JSON converter implementation |
| [presort_json.h.md](./DataFormats/Json/presort_json.h.md) | JSON pre-sorting utilities |
| [presort_json.cpp.md](./DataFormats/Json/presort_json.cpp.md) | Pre-sort implementation |

#### [DataFormats/Fhicl](./DataFormats/Fhicl/README.md) - FHiCL Format

| File | Description |
|------|-------------|
| [fhicl_common.h.md](./DataFormats/Fhicl/fhicl_common.h.md) | FHiCL common includes |
| [fhiclcpplib_includes.h.md](./DataFormats/Fhicl/fhiclcpplib_includes.h.md) | FHiCL library includes |
| [fhicl_types.h.md](./DataFormats/Fhicl/fhicl_types.h.md) | FHiCL type definitions |
| [fhicl_types.cpp.md](./DataFormats/Fhicl/fhicl_types.cpp.md) | FHiCL types implementation |
| [fhicl_reader.h.md](./DataFormats/Fhicl/fhicl_reader.h.md) | FHiCL parser/reader |
| [fhicl_reader.cpp.md](./DataFormats/Fhicl/fhicl_reader.cpp.md) | FHiCL reader implementation |
| [fhicl_writer.h.md](./DataFormats/Fhicl/fhicl_writer.h.md) | FHiCL writer/serializer |
| [fhicl_writer.cpp.md](./DataFormats/Fhicl/fhicl_writer.cpp.md) | FHiCL writer implementation |
| [fhicljsondb.h.md](./DataFormats/Fhicl/fhicljsondb.h.md) | FHiCL-JSON database interface |
| [fhicljsondb.cpp.md](./DataFormats/Fhicl/fhicljsondb.cpp.md) | FHiCL-JSON DB implementation |
| [convertfhicl2jsondb.h.md](./DataFormats/Fhicl/convertfhicl2jsondb.h.md) | FHiCL to JSON converter |
| [convertfhicl2jsondb.cpp.md](./DataFormats/Fhicl/convertfhicl2jsondb.cpp.md) | Converter implementation |
| [helper_functions.h.md](./DataFormats/Fhicl/helper_functions.h.md) | FHiCL helper functions |
| [helper_functions.cpp.md](./DataFormats/Fhicl/helper_functions.cpp.md) | Helper implementation |

#### [DataFormats/Xml](./DataFormats/Xml/README.md) - XML Format

| File | Description |
|------|-------------|
| [xml_common.h.md](./DataFormats/Xml/xml_common.h.md) | XML common includes |
| [xml_types.h.md](./DataFormats/Xml/xml_types.h.md) | XML type definitions |
| [xml_reader.h.md](./DataFormats/Xml/xml_reader.h.md) | XML parser/reader |
| [xml_reader.cpp.md](./DataFormats/Xml/xml_reader.cpp.md) | XML reader implementation |
| [xml_writer.h.md](./DataFormats/Xml/xml_writer.h.md) | XML writer/serializer |
| [xml_writer.cpp.md](./DataFormats/Xml/xml_writer.cpp.md) | XML writer implementation |
| [xmljsondb.h.md](./DataFormats/Xml/xmljsondb.h.md) | XML-JSON database interface |
| [xmljsondb.cpp.md](./DataFormats/Xml/xmljsondb.cpp.md) | XML-JSON DB implementation |
| [convertxml2json.h.md](./DataFormats/Xml/convertxml2json.h.md) | XML to JSON converter |
| [convertxml2json.cpp.md](./DataFormats/Xml/convertxml2json.cpp.md) | Converter implementation |

#### [DataFormats/Conf](./DataFormats/Conf/README.md) - Conf Format

| File | Description |
|------|-------------|
| [conf_common.h.md](./DataFormats/Conf/conf_common.h.md) | Conf common includes |
| [conf_types.h.md](./DataFormats/Conf/conf_types.h.md) | Conf type definitions |
| [conf_reader.h.md](./DataFormats/Conf/conf_reader.h.md) | Conf parser/reader |
| [conf_reader.cpp.md](./DataFormats/Conf/conf_reader.cpp.md) | Conf reader implementation |
| [conf_writer.h.md](./DataFormats/Conf/conf_writer.h.md) | Conf writer/serializer |
| [conf_writer.cpp.md](./DataFormats/Conf/conf_writer.cpp.md) | Conf writer implementation |
| [confjsondb.h.md](./DataFormats/Conf/confjsondb.h.md) | Conf-JSON database interface |
| [confjsondb.cpp.md](./DataFormats/Conf/confjsondb.cpp.md) | Conf-JSON DB implementation |
| [convertconf2json.h.md](./DataFormats/Conf/convertconf2json.h.md) | Conf to JSON converter |
| [convertconf2json.cpp.md](./DataFormats/Conf/convertconf2json.cpp.md) | Converter implementation |

---

### [StorageProviders](./StorageProviders/README.md) - Database Backends

Storage provider implementations for different backends.

| File | Description |
|------|-------------|
| [common.h.md](./StorageProviders/common.h.md) | Common includes for StorageProviders |
| [storage_providers.h.md](./StorageProviders/storage_providers.h.md) | Storage provider interface |
| [storage_providers.cpp.md](./StorageProviders/storage_providers.cpp.md) | Provider factory implementation |

#### [StorageProviders/FileSystemDB](./StorageProviders/FileSystemDB/README.md) - File System Backend

| File | Description |
|------|-------------|
| [provider_filedb.h.md](./StorageProviders/FileSystemDB/provider_filedb.h.md) | FileSystemDB provider interface |
| [provider_filedb.cpp.md](./StorageProviders/FileSystemDB/provider_filedb.cpp.md) | FileSystemDB implementation |
| [provider_filedb_headers.h.md](./StorageProviders/FileSystemDB/provider_filedb_headers.h.md) | FileSystemDB header includes |
| [provider_filedb_index.h.md](./StorageProviders/FileSystemDB/provider_filedb_index.h.md) | Search index interface |
| [provider_filedb_index.cpp.md](./StorageProviders/FileSystemDB/provider_filedb_index.cpp.md) | Search index implementation |
| [provider_filedb_readwrite.cpp.md](./StorageProviders/FileSystemDB/provider_filedb_readwrite.cpp.md) | Read/write operations |
| [provider_connection.cpp.md](./StorageProviders/FileSystemDB/provider_connection.cpp.md) | Connection management |
| [filesystem_functions.cpp.md](./StorageProviders/FileSystemDB/filesystem_functions.cpp.md) | Filesystem utilities |

#### [StorageProviders/MongoDB](./StorageProviders/MongoDB/README.md) - MongoDB Backend

| File | Description |
|------|-------------|
| [provider_mongodb.h.md](./StorageProviders/MongoDB/provider_mongodb.h.md) | MongoDB provider interface |

#### [StorageProviders/UconDB](./StorageProviders/UconDB/README.md) - UconDB REST Backend

| File | Description |
|------|-------------|
| [provider_ucondb.h.md](./StorageProviders/UconDB/provider_ucondb.h.md) | UconDB provider interface |

---

### [ConfigurationDB](./ConfigurationDB/README.md) - Main API

Main configuration database API and operations. **Largest module with 56 files.**

#### Core Interface Files

| File | Description |
|------|-------------|
| [configurationdbifc.h.md](./ConfigurationDB/configurationdbifc.h.md) | **ConfigurationInterface** - Main API |
| [configurationdbifc_base.h.md](./ConfigurationDB/configurationdbifc_base.h.md) | Base interface class |
| [common.h.md](./ConfigurationDB/common.h.md) | Common includes |
| [configuration_common.h.md](./ConfigurationDB/configuration_common.h.md) | Configuration common definitions |
| [configurationdb.h.md](./ConfigurationDB/configurationdb.h.md) | Main include header |
| [conftoolifc.h.md](./ConfigurationDB/conftoolifc.h.md) | Configuration tool interface |

#### Database Operations

| File | Description |
|------|-------------|
| [dboperation_managedocument.h.md](./ConfigurationDB/dboperation_managedocument.h.md) | Document management operations |
| [dboperation_manageconfigs.h.md](./ConfigurationDB/dboperation_manageconfigs.h.md) | Configuration management operations |
| [dboperation_managealiases.h.md](./ConfigurationDB/dboperation_managealiases.h.md) | Alias management operations |
| [dboperation_exportimport.h.md](./ConfigurationDB/dboperation_exportimport.h.md) | Export/import operations |
| [dboperation_searchcollection.h.md](./ConfigurationDB/dboperation_searchcollection.h.md) | Search operations |
| [dboperation_metadata.h.md](./ConfigurationDB/dboperation_metadata.h.md) | Metadata operations |
| [dboperation_findcompositions.h.md](./ConfigurationDB/dboperation_findcompositions.h.md) | Find compositions operations |

#### Operation Options

| File | Description |
|------|-------------|
| [options_operations.h.md](./ConfigurationDB/options_operations.h.md) | All operation options |
| [options_operation_base.h.md](./ConfigurationDB/options_operation_base.h.md) | Base options class |
| [options_operation_managedocument.h.md](./ConfigurationDB/options_operation_managedocument.h.md) | Document options |
| [options_operation_manageconfigs.h.md](./ConfigurationDB/options_operation_manageconfigs.h.md) | Config options |
| [options_operation_managealiases.h.md](./ConfigurationDB/options_operation_managealiases.h.md) | Alias options |
| [options_operation_bulkoperations.h.md](./ConfigurationDB/options_operation_bulkoperations.h.md) | Bulk operation options |

#### Provider Dispatch

| File | Description |
|------|-------------|
| [operation_dispatch.h.md](./ConfigurationDB/operation_dispatch.h.md) | Operation dispatcher |
| [dispatch_common.h.md](./ConfigurationDB/dispatch_common.h.md) | Dispatch common definitions |
| [dispatch_signatures.h.md](./ConfigurationDB/dispatch_signatures.h.md) | Operation signatures |
| [dispatch_filedb.h.md](./ConfigurationDB/dispatch_filedb.h.md) | FileDB dispatch |
| [dispatch_mongodb.h.md](./ConfigurationDB/dispatch_mongodb.h.md) | MongoDB dispatch |
| [dispatch_ucondb.h.md](./ConfigurationDB/dispatch_ucondb.h.md) | UconDB dispatch |

#### Utilities

| File | Description |
|------|-------------|
| [Multitasker.h.md](./ConfigurationDB/Multitasker.h.md) | Thread pool for parallel operations |
| [search_filter.h.md](./ConfigurationDB/search_filter.h.md) | Search filter definitions |
| [shared_helper_functions.h.md](./ConfigurationDB/shared_helper_functions.h.md) | Shared helpers |
| [enable_trace.cpp.md](./ConfigurationDB/enable_trace.cpp.md) | TRACE logging enablement |

---

### [Utilities](./Utilities/README.md) - CLI Tools

Command-line utilities for configuration management.

| File | Description |
|------|-------------|
| [conftool.cc.md](./Utilities/conftool.cc.md) | Main configuration CLI tool |
| [bulkloader.cc.md](./Utilities/bulkloader.cc.md) | Bulk import utility |
| [bulkdownloader.cc.md](./Utilities/bulkdownloader.cc.md) | Bulk export utility |
| [migrate_database.cc.md](./Utilities/migrate_database.cc.md) | Database migration tool |
| [fhicl2json.cc.md](./Utilities/fhicl2json.cc.md) | FHiCL to JSON converter |
| [readfhicl.cc.md](./Utilities/readfhicl.cc.md) | FHiCL file reader |
| [readjson.cc.md](./Utilities/readjson.cc.md) | JSON file reader |
| [fixtestjson.cc.md](./Utilities/fixtestjson.cc.md) | Test JSON fixer |
| [rebuild_database_index.cc.md](./Utilities/rebuild_database_index.cc.md) | Index rebuilder |
| [refactorfhicl.cc.md](./Utilities/refactorfhicl.cc.md) | FHiCL refactoring tool |
| [conftool_py.md](./Utilities/conftool_py.md) | Python configuration tool |
| [database_bash_rc.md](./Utilities/database_bash_rc.md) | Bash environment setup |
| [mongod_conf.md](./Utilities/mongod_conf.md) | MongoDB configuration template |
| [readwrite_js.md](./Utilities/readwrite_js.md) | MongoDB read/write scripts |
| [schema_fcl.md](./Utilities/schema_fcl.md) | FHiCL schema definitions |
| [systemd_services.md](./Utilities/systemd_services.md) | Systemd service files |

---

### [BuildInfo](./BuildInfo/README.md) - Version Information

Build and version information utilities.

| File | Description |
|------|-------------|
| [GetPackageBuildInfo.hh.md](./BuildInfo/GetPackageBuildInfo.hh.md) | Build info header |
| [GetPackageBuildInfo.cc.in.md](./BuildInfo/GetPackageBuildInfo.cc.in.md) | Build info template |

---

### [SWIGBindings](./SWIGBindings/README.md) - Python Bindings

SWIG-generated Python bindings for the configuration database.

| File | Description |
|------|-------------|
| [conftool.h.md](./SWIGBindings/conftool.h.md) | Python bindings header |
| [conftool.cpp.md](./SWIGBindings/conftool.cpp.md) | Python bindings implementation |
| [conftool.i.md](./SWIGBindings/conftool.i.md) | SWIG interface file |
| [conftool-python.md](./SWIGBindings/conftool-python.md) | Python usage documentation |

---

## Cross-Cutting Topics

### Thread Safety

| Component | Thread-Safe | Notes |
|-----------|-------------|-------|
| ConfigurationInterface | No | Create per-thread instance |
| MongoDB provider | Yes | Uses connection pooling |
| FileSystemDB provider | No | Single-threaded access |
| UconDB provider | Yes | Stateless HTTP calls |
| Multitasker | Internal | Thread pool management |
| JSONDocument | No | Not for concurrent modification |

See [ConfigurationDB README](./ConfigurationDB/README.md#thread-safety) for details.

### Error Handling

The project uses two error handling patterns:

1. **result_t**: `std::pair<bool, std::string>` for operations
2. **Exceptions**: `database_error`, `invalid_argument` for exceptional cases

See [SharedCommon/returned_result.h](./SharedCommon/returned_result.h.md) for details.

### Logging

Uses TRACE for high-performance logging:

```cpp
#include "artdaq-database/SharedCommon/common.h"

TLOG(TLVL_DEBUG) << "Debug message";
TLOG(TLVL_INFO) << "Info message";
TLOG(TLVL_WARNING) << "Warning message";
TLOG(TLVL_ERROR) << "Error message";
```

### Environment Variables

| Variable | Purpose | Example |
|----------|---------|---------|
| `ARTDAQ_DATABASE_URI` | Database connection | `filesystemdb:///path/to/db` |
| `TRACE_LVLS` | Logging level | `DEBUG` |
| `TRACE_FILE` | Log file path | `/tmp/artdaq.trace` |

---

## External Documentation

- [ARTDAQ Project](https://cdcvs.fnal.gov/redmine/projects/artdaq/wiki)
- [FHiCL Reference](https://cdcvs.fnal.gov/redmine/projects/fhicl-cpp/wiki)
- [Boost Libraries](https://www.boost.org/doc/libs/)
- [MongoDB C++ Driver](https://mongocxx.org/)

---

## Statistics

| Module | Header Files | Implementation Files | Total |
|--------|-------------|---------------------|-------|
| SharedCommon | 8 | 6 | 14 |
| BasicTypes | 8 | 5 | 13 |
| Overlay | 18 | 10 | 28 |
| JsonDocument | 5 | 5 | 10 |
| DataFormats | 26 | 28 | 54 |
| StorageProviders | 10 | 17 | 27 |
| ConfigurationDB | 26 | 30 | 56 |
| Utilities | 0 | 16 | 16 |
| BuildInfo | 1 | 1 | 2 |
| SWIGBindings | 2 | 1 | 3 |
| **Total** | **104** | **119** | **223** |

---

## Contributing to Documentation

- Documentation style guidelines
- Template formats
- Quality checks
- Glossary of terms

---

*Documentation generated for artdaq-database v3.00.00*
