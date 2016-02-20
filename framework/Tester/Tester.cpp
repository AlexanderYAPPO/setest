#include "Tester.h"

#include "sedna/ISednaTestCase.h"
#include "XQTS/XQTSTestSet.h"

#include <string>

const static std::string dbName = "xqts_test";

void Tester::runXQTS() {
    XQTSTestSet testset(baseDir);

    sednaInstance.startSedna();
    
    sedna::Database *dbInstance = sednaInstance.getDatabase(dbName);
    dbInstance->create();
    dbInstance->start();
    
    sedna::Session *session = dbInstance->createSession();
    
    testset.initTestSet(session);
    
    ISednaTestCase *testIt = testset.nextCase();
    while (testIt != 0) {
        testIt->execute(session);
        testIt = testset.nextCase();
    }
    
    dbInstance->stop();
    dbInstance->drop();
    
    testset.generateReport();
    
    sednaInstance.stopSedna();
}
