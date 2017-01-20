#ifndef _ARTDAQ_DATABASE_OVERLAY_OVLRECORD_H_
#define _ARTDAQ_DATABASE_OVERLAY_OVLRECORD_H_

#include "artdaq-database/Overlay/common.h"
#include "artdaq-database/Overlay/ovlBookkeeping.h"
#include "artdaq-database/Overlay/ovlChangeLog.h"
#include "artdaq-database/Overlay/ovlComment.h"
#include "artdaq-database/Overlay/ovlDocument.h"
#include "artdaq-database/Overlay/ovlId.h"
#include "artdaq-database/Overlay/ovlKeyValue.h"
#include "artdaq-database/Overlay/ovlOrigin.h"

#include "artdaq-database/Overlay/ovlKeyValueTimeStamp.h"
#include "artdaq-database/Overlay/ovlKeyValueWithDefault.h"

#include "artdaq-database/Overlay/ovlFixedList.h"
#include "artdaq-database/Overlay/ovlMovableList.h"

namespace artdaq {
namespace database {
namespace overlay {
namespace jsonliteral = artdaq::database::dataformats::literal;
using namespace artdaq::database::result;

using ovlVersion = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_VERSION>;
using ovlVersionUPtr_t = std::unique_ptr<ovlVersion>;

using ovlCollection = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_COLLECTION>;
using ovlCollectionUPtr_t = std::unique_ptr<ovlCollection>;

using ovlAlias = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_ALIAS, true, true>;
using ovlAliasUPtr_t = std::unique_ptr<ovlAlias>;

using ovlConfiguration = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationUPtr_t = std::unique_ptr<ovlConfiguration>;

using ovlConfigurationType = ovlStringKeyValue<DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationTypeUPtr_t = std::unique_ptr<ovlConfigurationType>;

using ovlRun = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_RUN>;
using ovlRunUPtr_t = std::unique_ptr<ovlRun>;

using ovlAttachment = ovlKeyValue;
using ovlAttachmentUPtr_t = std::unique_ptr<ovlAttachment>;

using ovlentity = ovlKeyValueTimeStamp<DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlentityUPtr_t = std::unique_ptr<ovlentity>;

using ovlConfigurations = ovlFixedList<ovlConfiguration, DOCUMENT_COMPARE_MUTE_CONFIGURATION>;
using ovlConfigurationsUPtr_t = std::unique_ptr<ovlConfigurations>;

using ovlConfigurableEntities = ovlFixedList<ovlentity, DOCUMENT_COMPARE_MUTE_CONFIGENTITY>;
using ovlConfigurableEntitiesUPtr_t = std::unique_ptr<ovlConfigurableEntities>;

using ovlAliases = ovlMovableList<ovlAlias, DOCUMENT_COMPARE_MUTE_ALIAS>;
using ovlAliasesUPtr_t = std::unique_ptr<ovlAliases>;

using ovlAttachments = ovlFixedList<ovlAttachment, DOCUMENT_COMPARE_MUTE_ATTACHMENT>;
using ovlAttachmentsUPtr_t = std::unique_ptr<ovlAttachments>;

using ovlRuns = ovlFixedList<ovlRun, DOCUMENT_COMPARE_MUTE_RUN>;
using ovlRunsUPtr_t = std::unique_ptr<ovlRuns>;

using ovlComments = ovlFixedList<ovlComment, DOCUMENT_COMPARE_MUTE_COMMENTS>;
using ovlCommentsUPtr_t = std::unique_ptr<ovlComments>;

class ovlDatabaseRecord final : public ovlKeyValue {
 public:
  ovlDatabaseRecord(value_t& record);

  // defaults
  ovlDatabaseRecord(ovlDatabaseRecord&&) = default;
  ~ovlDatabaseRecord() = default;

  // accessors
  ovlDocument& document();
  ovlComments& comments();
  ovlOrigin& origin();
  ovlVersion& version();
  ovlentity& entity();
  ovlConfigurations& configurations();
  ovlBookkeeping& bookkeeping();
  ovlId& id();

  // utils
  result_t swap(ovlDocumentUPtr_t& /*document*/);
  result_t swap(ovlCommentsUPtr_t& /*comments*/);
  result_t swap(ovlOriginUPtr_t& /*origin*/);
  result_t swap(ovlVersionUPtr_t& /*version*/);
  result_t swap(ovlConfigurationTypeUPtr_t& /*configtype*/);
  result_t swap(ovlCollectionUPtr_t& /*collection*/);

  result_t swap(ovlIdUPtr_t& /*id*/);

  // overrides
  std::string to_string() const noexcept override;

  // ops
  result_t operator==(ovlDatabaseRecord const&) const;

  // delegates
  bool& isReadonly();
  bool const& isReadonly() const;
  result_t markReadonly();
  result_t markDeleted();

  result_t addConfiguration(ovlConfigurationUPtr_t& /*configuration*/);
  result_t removeConfiguration(ovlConfigurationUPtr_t& /*configuration*/);

  result_t addAlias(ovlAliasUPtr_t& /*alias*/);
  result_t removeAlias(ovlAliasUPtr_t& /*alias*/);

  result_t addEntity(ovlentityUPtr_t& /*entity*/);
  result_t removeEntity(ovlentityUPtr_t& /*entity*/);

  result_t setVersion(ovlVersionUPtr_t& /*version*/);
  result_t setCollection(ovlCollectionUPtr_t& /*version*/);
  result_t setConfigurationType(ovlConfigurationTypeUPtr_t& /*configtype*/);

 private:
  ovlDocumentUPtr_t _document;
  ovlCommentsUPtr_t _comments;
  ovlOriginUPtr_t _origin;
  ovlVersionUPtr_t _version;
  ovlConfigurableEntitiesUPtr_t _entities;
  ovlConfigurationsUPtr_t _configurations;
  ovlChangeLogUPtr_t _changelog;
  ovlBookkeepingUPtr_t _bookkeeping;
  ovlIdUPtr_t _id;
  ovlCollectionUPtr_t _collection;
  ovlAliasesUPtr_t _aliases;
  ovlRunsUPtr_t _runs;
  ovlAttachmentsUPtr_t _attachments;
  ovlConfigurationTypeUPtr_t _configurationtype;
};

using ovlDatabaseRecordUPtr_t = std::unique_ptr<ovlDatabaseRecord>;

}  // namespace overlay
}  // namespace database
}  // namespace artdaq

#endif /* _ARTDAQ_DATABASE_OVERLAY_OVLRECORD_H_ */
