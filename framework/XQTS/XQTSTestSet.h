#ifndef XQTSTESTSET_H
#define XQTSTESTSET_H

#include "sedna/ISednaTestSet.h"
#include "sedna/SednaInterface.h"
#include "XQTSTestCase.h"

#include <vector>

using std::string;

typedef std::vector<XQTSTestCase *> XQTSTestCaseList;
typedef std::vector< std::pair<string, string> > XQTSSourceList;

class XQTSTestSet : public ISednaTestSet
{
    XQTSTestCaseList testCases;
    XQTSSourceList sources;
    int currentCaseId;

    std::string baseDir;
        
    void freeTestCases();
    
    void doBulkloads(sedna::Session* session);
    
public:
    XQTSTestSet(const std::string& baseDir);
    ~XQTSTestSet();
    
    /**
     * Initializes TestCase iterator and performs bulkloads before testing started
     *   
     */
    void initTestSet(sedna::Session *session);

    /**
     * @returns NULL when no more tests available, or enumeration
     *   is not initialized by initTestSet
     */
    ISednaTestCase *nextCase();

    void generateReport() {};
};

#endif // XQTSTESTSET_H
