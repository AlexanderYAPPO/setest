#ifndef XQTSCATALOGPARSER_H
#define XQTSCATALOGPARSER_H

#include <pugixml.hpp>
#include "XQTSTestSet.h"
#include "XQTSTestCase.h"
#include <string>

//FIXME Refactor - do the initialization in constructor (catalog opening)

class XQTSCatalogParser {
private:
    std::string baseDir;
    bool parseTestCase(pugi::xml_node& caseNode, XQTSTestCase* testCase, string filePath);
    
public:
    XQTSCatalogParser(std::string &baseDir) {
        this->baseDir = baseDir;
    }
    
    XQTSTestCaseList parseTestCases();
    XQTSSourceList parseSources();

};

#endif // XQTSCATALOGPARSER_H