#ifndef XQTSTESTCASE_H
#define XQTSTESTCASE_H

#include "sedna/ISednaTestCase.h"
#include "XQTSInputs.h"
#include "XQTSOutputs.h"

#include <list>
#include <vector>
#include <string>
#include <boost/scoped_ptr.hpp>

struct XqtsTestCaseData
{
};

struct XqtsTestCaseResult
{
    std::string outputFile;
};

class XQTSTestCase : public ISednaTestCase
{
    typedef std::vector<IXQTSInput *> InputList;
    typedef std::vector<IXQTSOutput *> OutputList;

    InputList inputs;
    OutputList outputs;

    std::string name;
    std::string queryFile;

    XqtsTestCaseData * data;

    boost::scoped_ptr<XqtsTestCaseResult> result;

    void freeInputsOutputs();

public:
    XQTSTestCase(const std::string &caseName);
    ~XQTSTestCase();

    void execute(sedna::Session *session);

    void addInput(IXQTSInput *input);
    void addOutput(IXQTSOutput *output);
    void setQueryFile(std::string queryFile);

    
    std::string getName();
};

#endif // XQTSTESTCASE_H
