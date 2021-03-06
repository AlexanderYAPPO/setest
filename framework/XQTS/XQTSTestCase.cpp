/*
    Copyright 2012 Ilya Taranov <ilya@taranoff.net>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include <string>
#include <iostream>

#include "sedna/SednaInterface.h"
#include "XQTSTestCase.h"
#include "XQTSConfig.h"

#ifdef SETEST_DEBUG
#include <csignal>
#endif

using std::string;
using sedna::Query;

string XQTSTestCase::getName()
{
    return name;
}

void XQTSTestCase::addInput(IXQTSInput *input)
{
    inputs.push_back(input);
}

void XQTSTestCase::addOutput(IXQTSOutput *output)
{
    outputs.push_back(output);
}

void XQTSTestCase::setQueryFile(std::string queryFile) {
    this->queryFile = queryFile;
}

void XQTSTestCase::execute(sedna::Session *session)
{
    std::cout << "Executing case " + name + ": \n";
    //Preparing query for execution
    Query query = Query(Query::Filename(queryFile));
    
    for (InputList::iterator input = inputs.begin(); input != inputs.end(); input++) {
        (*input)->apply(query);
    }

    sedna::Transaction tr(session);
    tr.execute(query);

    std::cout << query.resultCode << "\n";
    #ifdef SETEST_DEBUG
        std::cout << "TEST======0======TEST" << std::endl;
    #endif
    std::cout << query.resultString << "\n";
    
//     if (query.resultCode == -7 || query.resultCode == 6) {
//         std::raise(SIGINT);
//     }

    /*  Here we are checking actual result for correctness against every presented output
    *   If any of them matches, result is correct */
    OutputList::iterator output;
    for (output = outputs.begin(); output != outputs.end(); output++) {
        if ((*output)->check(query)) {
            std::cout << "Correct!\n";
        } else {
	    break;
	}
    }
    if (output != outputs.end()) {
        std::cout << "Incorrect!\n";
    }
}

XQTSTestCase::XQTSTestCase(const string& caseName): name(caseName)
{
}

void XQTSTestCase::freeInputsOutputs()
{
    for (unsigned int i = 0; i < inputs.size(); i++) {
        delete inputs[i];
    }
    inputs.clear();
    
    for (unsigned int i = 0; i < outputs.size(); i++) {
        delete outputs[i];
    }
    outputs.clear();
}

XQTSTestCase::~XQTSTestCase()
{
    freeInputsOutputs();
}
