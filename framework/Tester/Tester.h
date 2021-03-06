#ifndef TESTER_H
#define TESTER_H

#include <string>
#include "sedna/SednaInterface.h"

using std::string;

class Tester {
    
    string baseDir;
    string configFile;
    string sednaDir;
    
    sedna::SednaInterface sednaInstance;
    
public:
    
    Tester(string &baseDir, string &configFile, string &sednaDir):
        sednaInstance(sednaDir)
    {
        this->baseDir = baseDir;
        this->configFile = baseDir + "/" + configFile;
        this->sednaDir = sednaDir;   
    }
    
    void runXQTS();
    
};

#endif /* TESTER_H */
