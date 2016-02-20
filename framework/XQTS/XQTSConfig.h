#ifndef XQTSCONFIG_H
#define XQTSCONFIG_H

#include <string>

struct XQTSConfig {
  static const std::string configFile;
  
  static const std::string queryBaseDir;
  static const std::string answerBaseDir;
  static const std::string dataBaseDir;
};


#endif // XQTSCONFIG_H