#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/StorageProviders/FileSystemDB/provider_filedb.h"

#include <wordexp.h>
#include <boost/filesystem.hpp>
#include <boost/range/adaptors.hpp>

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "PRVDR:FileDB_C"

using namespace artdaq::database;

namespace db = artdaq::database;
namespace dbfs = db::filesystem;
namespace dbfsl = dbfs::literal;

using dbfs::file_paths_t;

std::string dbfs::mkdir(std::string const& d) {
  confirm(!d.empty());

  auto dir = d;

  if (dir.find(dbfsl::FILEURI) == 0) {
    dir = dir.substr(strlen(dbfsl::FILEURI));
  }

  TLOG(21) << "StorageProvider::FileSystemDB mkdir dir=<" << dir << ">";

  auto path = boost::filesystem::path(dir.c_str());

  if (boost::filesystem::exists(boost::filesystem::status(path))) {
    if (boost::filesystem::is_directory(path)) {
      TLOG(21) << "StorageProvider::FileSystemDB Directory exists, checking "
                  "permissions; path=<"
               << dir << ">";

      auto mask = boost::filesystem::perms::owner_write | boost::filesystem::perms::owner_read;

      if ((boost::filesystem::status(path).permissions() & mask) != mask) {
        TLOG(21) << "StorageProvider::FileSystemDB Directory  <" << dir
                 << "> has wrong permissions; needs to be owner readable and writable";
        throw runtime_error("FileSystemDB")
            << "Directory  <" << dir << "> has wrong permissions; needs to be owner readable and writable";
      }
    } else {
      TLOG(21) << "StorageProvider::FileSystemDB Failed creating a directory, sometging in the way path=<" << dir
               << ">";
      throw runtime_error("FileSystemDB") << "Failed creating a directory, sometging in the way path=<" << dir << ">";
    }
  }

  boost::system::error_code ec;

  if (!boost::filesystem::create_directories(path, ec)) {
    if (ec != boost::system::errc::success) {
      TLOG(21) << "StorageProvider::FileSystemDB Failed creating a directory path=<" << dir
               << "> error code=" << ec.message();
      throw runtime_error("FileSystemDB")
          << "Failed creating a directory path=<" << dir << "> error code=" << ec.message();
    }
  }

  ec.clear();

  /*
   auto perms = boost::filesystem::perms::add_perms | boost::filesystem::perms::owner_write |
   boost::filesystem::perms::owner_read;
   boost::filesystem::permissions(path, perms, ec);

    if (ec != boost::system::errc::success) {
      TLOG(21)<< "StorageProvider::FileSystemDB Failed enforcing directory permissions for path=<" << dir << "> error
   code=" << ec.message());
      throw runtime_error("FileSystemDB") << "Failed enforcing directory permissions for path=<" << dir << "> error
   code=" << ec.message();
    }
  */
  return dir;
}

std::list<std::string> dbfs::find_subdirs(std::string const& d) {
  confirm(!d.empty());

  auto returnValue = std::list<std::string>{};

  auto dir = d;

  if (dir.find(dbfsl::FILEURI) == 0) {
    dir = dir.substr(strlen(dbfsl::FILEURI));
  }

  TLOG(21) << "StorageProvider::FileSystemDB find_subdirs dir=<" << dir << ">";

  auto path = boost::filesystem::path(dir.c_str());

  if (!boost::filesystem::exists(boost::filesystem::status(path))) {
    TLOG(21) << "StorageProvider::FileSystemDB Failed searching for subdirectories, directory does not exist path=<"
             << dir << ">";
    throw runtime_error("FileSystemDB") << "Failed searching for subdirectories, directory does not exist path=<" << dir
                                        << ">";
  }

  if (!boost::filesystem::is_directory(path)) {
    TLOG(21) << "StorageProvider::FileSystemDB Failed searching for subdirectories, not a directory path=<" << dir
             << ">";
    throw runtime_error("FileSystemDB") << "Failed searching for subdirectories, not a directory path=<" << dir << ">";
  }

  boost::filesystem::directory_iterator end_iter;

  for (boost::filesystem::directory_iterator dir_iter(path); dir_iter != end_iter; ++dir_iter) {
    if (!boost::filesystem::is_directory(dir_iter->status())) {
      continue;
    }
    TLOG(21) << "StorageProvider::FileSystemDB Found subdirectory, path=<" << dir_iter->path().filename().string()
             << ">";

    returnValue.emplace_back(dir_iter->path().filename().string());
  }

  return returnValue;
}

std::list<object_id_t> dbfs::find_documents(std::string const& d) {
  confirm(!d.empty());

  auto returnValue = std::list<object_id_t>{};

  auto dir = d;

  if (dir.find(dbfsl::FILEURI) == 0) {
    dir = dir.substr(strlen(dbfsl::FILEURI));
  }

  TLOG(21) << "StorageProvider::FileSystemDB find_documents dir=<" << dir << ">";

  auto path = boost::filesystem::path(dir.c_str());

  if (!boost::filesystem::exists(boost::filesystem::status(path))) {
    TLOG(21) << "StorageProvider::FileSystemDB Failed searching for documents, directory does not exist path=<" << dir
             << ">";
    throw runtime_error("FileSystemDB") << "Failed searching fo documents, directory does not exist path=<" << dir
                                        << ">";
  }

  if (!boost::filesystem::is_directory(path)) {
    TLOG(21) << "StorageProvider::FileSystemDB Failed searching fo documents, not a directory path=<" << dir << ">";
    throw runtime_error("FileSystemDB") << "Failed searching fo documents, not a directory path=<" << dir << ">";
  }

  boost::filesystem::directory_iterator end_iter;

  for (boost::filesystem::directory_iterator dir_iter(path); dir_iter != end_iter; ++dir_iter) {
    if (boost::filesystem::is_directory(dir_iter->status())) {
      continue;
    }

    auto file = dir_iter->path().filename().replace_extension();

    TLOG(21) << "StorageProvider::FileSystemDB Found document, oid=<" << file.string() << ">";

    returnValue.emplace_back(file.string());
  }

  return returnValue;
}

std::list<std::string> dbfs::find_siblingdirs(std::string const& d) {
  confirm(!d.empty());

  auto dir = d;

  if (dir.find(dbfsl::FILEURI) == 0) {
    dir = dir.substr(strlen(dbfsl::FILEURI));
  }

  TLOG(21) << "StorageProvider::FileSystemDB find_siblingdirs dir=<" << dir << ">";

  auto path = std::string{boost::filesystem::path(dir.c_str()).parent_path().c_str()};

  return find_subdirs(path);
}

file_paths_t dbfs::list_files_in_directory(boost::filesystem::path const& path, std::string const& ext[[gnu::unused]]) {
  auto result = file_paths_t{};

  if (!boost::filesystem::exists(boost::filesystem::status(path))) {
    TLOG(21) << "StorageProvider::FileSystemDB Directory does not exist, path=<" << path.string() << ">";
    throw runtime_error("FileSystemDB") << "StorageProvider::FileSystemDB Directory does not exist, path=<"
                                        << path.string() << ">";
  }

  if (!boost::filesystem::is_directory(path)) {
    TLOG(22) << "StorageProvider::FileSystemDB Failed listing files in the "
                "directory; directory doesn't exist =<"
             << path.string() << ">";
  }

  result.reserve(100);

  auto end = boost::filesystem::directory_iterator{};
  auto iter = boost::filesystem::directory_iterator(path);

  for (; iter != end; ++iter) {
    if (boost::filesystem::is_regular_file(iter->status()) && !iter->path().filename().empty()) {
      result.push_back(iter->path());
    }
  }
  return result;
}

bool dbfs::check_if_file_exists(std::string const& f) {
  confirm(!f.empty());

  std::ifstream in(f.c_str());
  return in.good();
}