#ifndef _ots_ConfigurationInterface_h_
#define _ots_ConfigurationInterface_h_

#include <sstream>
#include <string>

namespace ots {
struct ConfigurationInterface {};

struct ConfigurationView {
  void printJSON(std::stringstream& ss) const { ss << _json; }
  int fillFromJSON(std::string const& newjson) {
    _json = newjson;
    return 0;
  }
  std::string _json = "{ \"testJSON\" : 123 }";
  int version = 1;
};

struct ConfigurationBase {
  virtual std::string getConfigurationName() const { return "ConfigurationBase"; }
  virtual ConfigurationView& getView() = 0;
  virtual ConfigurationView const& getView() const = 0;

  virtual ConfigurationView* getViewP() = 0;
  virtual int getViewVersion() const = 0;
};

struct TestConfiguration001 final : public ConfigurationBase {
  std::string getConfigurationName() const { return "TestConfiguration001"; }
  ConfigurationView& getView() { return view; }
  ConfigurationView const& getView() const { return view; }

  ConfigurationView* getViewP() { return &view; }
  int getViewVersion() const { return view.version; }
  ConfigurationView view;
};

struct TestConfiguration002 final : public ConfigurationBase {
  std::string getConfigurationName() const { return "TestConfiguration002"; }
  ConfigurationView& getView() { return view; }
  ConfigurationView const& getView() const { return view; }

  ConfigurationView* getViewP() { return &view; }
  int getViewVersion() const { return view.version; }
  ConfigurationView view;
};
}

#endif  //_ots_DatabaseConfigurationInterface_h_
