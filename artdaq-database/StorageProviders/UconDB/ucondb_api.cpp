#include "artdaq-database/SharedCommon/sharedcommon_common.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb.h"
#include "artdaq-database/StorageProviders/UconDB/provider_ucondb_headers.h"

#include <curl/curl.h>

// https://cdcvs.fnal.gov/redmine/projects/ucondb/wiki/Proposal

#ifdef TRACE_NAME
#undef TRACE_NAME
#endif

#define TRACE_NAME "ucondb_api.cpp"

using namespace artdaq::database;

static size_t data_write(void* buf, size_t size, size_t nmemb, void* userp) {
  if (userp != nullptr) {
    std::ostream& os = *static_cast<std::ostream*>(userp);
    std::streamsize len = size * nmemb;
    if (os.write(static_cast<char*>(buf), len)) {
      TLOG(20) << "UconDBAPI::data_write() len=" << len;
      return len;
    }
  }

  return 0;
}

static size_t data_read(void* buf, size_t size, size_t nmemb, void* userp) {
  if (userp != nullptr) {
    std::istream& is = *static_cast<std::istream*>(userp);
    std::streamsize need = size * nmemb;
    std::streamsize len = is.readsome(static_cast<char*>(buf), need);
    TLOG(21) << "UconDBAPI::data_read() len=" << len;
    return len;
  }

  return 0;
}

std::list<std::string> dbuc::folders(const UconDBSPtr_t& provider) {
  auto retValue = std::list<std::string>{};

  TLOG(12) << "UconDBAPI::folders() begin";

  auto url = provider->connection();
  url.append("/app/folders");
  TLOG(12) << "UconDBAPI::folders() url=" << url;

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl != nullptr) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
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
  while (std::getline(ss, s, ',')) {
    s = db::dequote(trim(s));
    if (!s.empty()) {
      retValue.push_back(s);
    }
  }

  if (CURLE_OK != code) {
    TLOG(12) << "UconDBAPI::folders() CURLcode=" << curl_easy_strerror(code);
  }

  TLOG(12) << "UconDBAPI::folders() received=<" << db::to_csv(retValue) << ">";

  return retValue;
}

std::list<std::string> dbuc::tags(const UconDBSPtr_t& provider, std::string const& folder) {
  auto retValue = std::list<std::string>{};

  confirm(!folder.empty());

  TLOG(13) << "UconDBAPI::tags() begin";
  TLOG(13) << "UconDBAPI::tags() folder=<" << folder << ">";

  auto url = provider->connection();
  url.append("/app/tags");
  url.append("?folder=");
  url.append(folder);

  TLOG(13) << "UconDBAPI::tags() url=" << url;

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl != nullptr) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
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
  while (std::getline(ss, s, ',')) {
    s = db::dequote(trim(s));
    if (!s.empty()) {
      retValue.push_back(s);
    }
  }

  if (CURLE_OK != code) {
    TLOG(13) << "UconDBAPI::tags() CURLcode=" << curl_easy_strerror(code);
  }

  TLOG(13) << "UconDBAPI::tags() received=<" << db::to_csv(retValue) << ">";

  return retValue;
}

std::list<std::string> dbuc::objects(const UconDBSPtr_t& provider, std::string const& folder) {
  auto retValue = std::list<std::string>{};

  confirm(!folder.empty());

  TLOG(14) << "UconDBAPI::objects() begin";
  TLOG(14) << "UconDBAPI::objects() folder=<" << folder << ">";

  auto url = provider->connection();
  url.append("/app/objects");
  url.append("?folder=");
  url.append(folder);

  TLOG(14) << "UconDBAPI::objects() url=" << url;

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl != nullptr) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
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
  while (std::getline(ss, s, ',')) {
    s = db::dequote(trim(s));
    if (!s.empty() && s != "null") {
      TLOG(14) << "UconDBAPI::objects() object=<" << s << ">";
      retValue.push_back(s);
    }
  }

  if (CURLE_OK != code) {
    TLOG(14) << "UconDBAPI::objects() CURLcode=" << curl_easy_strerror(code);
  }

  TLOG(14) << "UconDBAPI::objects() received=<" << db::to_csv(retValue) << ">";

  return retValue;
}

result_t dbuc::get_object(const UconDBSPtr_t& provider, std::string const& folder, std::string const& object) {
  auto retValue = std::string{};

  confirm(!folder.empty());
  confirm(!object.empty());

  TLOG(15) << "UconDBAPI::get_object() begin";
  TLOG(15) << "UconDBAPI::get_object() folder=<" << folder << ">, object=<" << object << ">";

  auto url = provider->connection();
  url.append("/app/get");
  url.append("?folder=");
  url.append(folder);
  url.append("&object=");
  url.append(object);

  TLOG(15) << "UconDBAPI::get_object() url=" << url;

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl != nullptr) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  if (CURLE_OK != code) {
    auto msg = std::string(curl_easy_strerror(code));
    TLOG(15) << "UconDBAPI::get_object() CURLcode=" << msg;
    return Failure(msg);
  }

  retValue = ss.str();

  TLOG(15) << "UconDBAPI::get_object() received=<" << retValue << ">";

  return Success(retValue);
}

result_t dbuc::put_object(const UconDBSPtr_t& provider, std::string const& folder, std::string const& buffer, std::string const& object,
                          long const tv, std::list<std::string> const& tags, std::string const& key) {
  confirm(!folder.empty());
  confirm(!buffer.empty());
  confirm(!object.empty());

  TLOG(16) << "UconDBAPI::put_object() begin";
  TLOG(16) << "UconDBAPI::put_object() folder=<" << folder << ">, object=<" << object << ">";

  std::stringstream urlss;
  urlss << provider->connection();
  urlss << "/app/put";
  urlss << "?folder=";
  urlss << folder;
  urlss << "&object=";
  urlss << object;

  if (tv != 0) {
    urlss << "&tv=";
    urlss << std::to_string(tv);
  }

  if (!tags.empty()) {
    urlss << "&tag=";
    for (auto const& tag : tags) {
      urlss << tag << ",";
    }
    urlss.seekp(-1, urlss.cur);
  }

  if (!key.empty()) {
    urlss << "&key=";
    urlss << key;
  }

  urlss << " ";

  auto url = urlss.str();
  TLOG(16) << "UconDBAPI::put_object() url=" << url;

  std::stringstream oss;
  std::stringstream iss;
  iss << buffer;

  iss.seekg(0, std::ios::end);
  auto size = iss.tellg();
  iss.seekg(0, std::ios::beg);

  TLOG(16) << "UconDBAPI::put_object() streamsize=" << size;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl != nullptr) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_READFUNCTION, &data_read)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &oss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_INFILE, &iss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)size)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  if (CURLE_OK != code) {
    auto msg = std::string(curl_easy_strerror(code));
    TLOG(16) << "UconDBAPI::put_object() CURLcode=" << msg;
    return Failure(msg);
  }

  auto msg = oss.str();

  if (msg == "Authorization required") {
    return Failure(msg);
  }

  return Success();
}

result_t dbuc::create_folder(const UconDBSPtr_t& provider, std::string const& folder) {
  confirm(!folder.empty());

  TLOG(15) << "UconDBAPI::create_folder() begin";
  TLOG(15) << "UconDBAPI::create_folder() folder=<" << folder << ">";

  auto fl = folders(provider);

  bool found = (std::find(fl.begin(), fl.end(), to_lower(folder)) != fl.end());

  if (found) {
    TLOG(15) << "UconDBAPI::create_folder() Folder exists.";
    return Success(db::result::msg_AlreadyExist);
  }

  auto url = provider->connection();
  url.append("/app/create_folder");
  url.append("?folder=");
  url.append(folder);
  url.append("&read=read_only_user&write=db_writer&drop=no");

  TLOG(17) << "UconDBAPI::create_folder() url=" << url;

  std::stringstream ss;

  CURLcode code(CURLE_FAILED_INIT);
  CURL* curl = curl_easy_init();
  if (curl != nullptr) {
    if (CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &data_write)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, (long)CURLAUTH_DIGEST)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L)) && CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_FILE, &ss)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, provider->timeout())) &&
        // CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 10L)) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_USERPWD, provider->authentication().c_str())) &&
        CURLE_OK == (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str()))) {
      code = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
  }

  if (CURLE_OK != code) {
    auto msg = std::string(curl_easy_strerror(code));
    TLOG(17) << "UconDBAPI::create_folder() CURLcode=" << msg;
    return Failure(msg);
  }

  fl = folders(provider);
  found = (std::find(fl.begin(), fl.end(), to_lower(folder)) != fl.end());

  if (found) {
    return Success(db::result::msg_Added);
  }
  { return Failure(db::result::msg_Failure); }
}

void dbuc::debug::UconDBAPI() {
  TRACE_CNTL("name", TRACE_NAME);
  TRACE_CNTL("lvlset", 0xFFFFFFFFFFFFFFFFLL, 0xFFFFFFFFFFFFFFFFLL, 0LL);
  TRACE_CNTL("modeM", trace_mode::modeM);
  TRACE_CNTL("modeS", trace_mode::modeS);

  TLOG(10) << "artdaq::database::ucon::UconDBAPI trace_enable";
}
