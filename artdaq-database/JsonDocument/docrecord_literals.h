#ifndef _ARTDAQ_DATABASE_DOCRECORD_DOCUMENT_LITERALS_H_
#define _ARTDAQ_DATABASE_DOCRECORD_DOCUMENT_LITERALS_H_

namespace artdaq {
namespace database {
namespace docrecord {

#if 0
namespace literal {
constexpr auto document = "document";
constexpr auto data = "document.data";
constexpr auto search = "document.search";
constexpr auto metadata = "document.metadata";
constexpr auto comments = "document.metadata.comments";

constexpr auto changelog = "changelog";

constexpr auto origin = "origin";
constexpr auto source_rawdata = "origin.rawdata";

constexpr auto version = "version";
constexpr auto id = "document._id";

constexpr auto alias = "alias";

constexpr auto aliases = "aliases";
constexpr auto aliases_active = "aliases.active";
constexpr auto aliases_history = "aliases.history";

constexpr auto bookkeeping = "bookkeeping";
constexpr auto bookkeeping_updates = "bookkeeping.updates";
constexpr auto bookkeeping_isdeleted = "bookkeeping.isdeleted";
constexpr auto bookkeeping_isreadonly = "bookkeeping.isreadonly";

constexpr auto configuration = "configuration";

constexpr auto configurations = "configurations";
constexpr auto configurations_name = "configurations.name";

constexpr auto name = "name";
constexpr auto entities = "entities";
constexpr auto entities_name = "entities.name";

constexpr auto document_root = "root";
}

#endif

namespace actions {
constexpr auto addAlias = "addAlias";
constexpr auto setentity = "setentity";
constexpr auto removeAlias = "removeAlias";
constexpr auto addToGlobalConfig = "addToGlobalConfig";
constexpr auto setVersion = "setVersion";
constexpr auto markDeleted = "markDeleted";
constexpr auto markReadonly = "markReadonly";
}

constexpr auto template__empty_document =
    "{\n"
    "}";

}  // namespace docrecord
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_DOCRECORD_DOCUMENT_LITERALS_H_ */
