#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h"

#include <curl/curl.h>

// https://cdcvs.fnal.gov/redmine/projects/ucondb/wiki/Proposal

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "UCAPI:UconDB_C"

inline std::string trim(const std::string& s) {
  auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
  return std::string(wsfront, std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {
                                return std::isspace(c);
                              }).base());
}

static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp) {
  if (userp) {
    std::ostream& os = *static_cast<std::ostream*>(userp);
    std::streamsize len = size * nmemb;
    if (os.write(static_cast<char*>(buf), len)) return len;
  }

  return 0;
}

std::list<std::string> dbuc::folders(UconDBSPtr_t provider) {
  auto retValue = std::list<std::string>{};

  TRACE_(4, "UconDBAPI::folders() begin");

  auto url = provider->connection();
  url.append("/app/folders");
  TRACE_(4, "UconDBAPI::folders() url=" << url);

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  ss.seekp(-1, ss.cur);
  ss << " ";  // skip last ]
  char ch;
  ss >> ch;  // skip first [

  std::string s;
  while (std::getline(ss, s, ',')) retValue.push_back(db::dequote(trim(s)));

  if (CURLE_OK != code) {
    TRACE_(4, "UconDBAPI::folders() CURLcode=" << curl_easy_strerror(code));
  }

  TRACE_(4, "UconDBAPI::folders() received=<" << db::to_csv(retValue) << ">");

  return retValue;
}

std::list<std::string> dbuc::tags(UconDBSPtr_t provider, std::string const& folder) {
  auto retValue = std::list<std::string>{};

  confirm(!folder.empty());

  TRACE_(4, "UconDBAPI::tags() begin");
  TRACE_(4, "UconDBAPI::tags() folder=<" << folder << ">");

  auto url = provider->connection();
  url.append("/app/tags");
  url.append("?folder=");
  url.append(folder);

  TRACE_(4, "UconDBAPI::tags() url=" << url);

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  ss.seekp(-1, ss.cur);
  ss << " ";  // skip last ]
  char ch;
  ss >> ch;  // skip first [

  std::string s;
  while (std::getline(ss, s, ',')) retValue.push_back(db::dequote(trim(s)));

  if (CURLE_OK != code) {
    TRACE_(4, "UconDBAPI::tags() CURLcode=" << curl_easy_strerror(code));
  }

  TRACE_(4, "UconDBAPI::tags() received=<" << db::to_csv(retValue) << ">");

  return retValue;
}

std::list<std::string> dbuc::objects(UconDBSPtr_t provider, std::string const& folder) {
  auto retValue = std::list<std::string>{};

  confirm(!folder.empty());

  TRACE_(4, "UconDBAPI::objects() begin");
  TRACE_(4, "UconDBAPI::objects() folder=<" << folder << ">");

  auto url = provider->connection();
  url.append("/app/objects");
  url.append("?folder=");
  url.append(folder);

  TRACE_(4, "UconDBAPI::objects() url=" << url);

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  ss.seekp(-1, ss.cur);
  ss << " ";  // skip last ]
  char ch;
  ss >> ch;  // skip first [

  std::string s;
  while (std::getline(ss, s, ',')) retValue.push_back(db::dequote(trim(s)));

  if (CURLE_OK != code) {
    TRACE_(4, "UconDBAPI::objects() CURLcode=" << curl_easy_strerror(code));
  }

  TRACE_(4, "UconDBAPI::objects() received=<" << db::to_csv(retValue) << ">");

  return retValue;
}

std::string dbuc::get_object(UconDBSPtr_t provider, std::string const& folder, std::string const& object) {
  auto retValue = std::string{};

  confirm(!folder.empty());
  confirm(!object.empty());

  TRACE_(4, "UconDBAPI::get_object() begin");
  TRACE_(4, "UconDBAPI::get_object() folder=<" << folder << ">, object=<" << object <<">");

  auto url = provider->connection();
  url.append("/app/get");
  url.append("?folder=");
  url.append(folder);
  url.append("&object=");
  url.append(object);

  
  TRACE_(4, "UconDBAPI::get_object() url=" << url);

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  if (CURLE_OK != code) {
    TRACE_(4, "UconDBAPI::get() CURLcode=" << curl_easy_strerror(code));
  }

  retValue = ss.str();
  
  TRACE_(4, "UconDBAPI::get_object() received=<" << retValue << ">");

  return retValue;
}

void dbuc::debug::enableUconDBAPI() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TRACE_(0, "artdaq::database::ucon::enableUconDBAPI trace_enable");
}
