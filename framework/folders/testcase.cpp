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

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "testcase.h"
#include "sedna/sednainterface.h"
#include "diff/xmldiff.h"

using std::string;

TestCase::TestCase(const string& caseName)
{
    this->caseName = caseName;
}

void TestCase::freeQueries()
{
    for (int i = 0; i < queries.size(); i++) {
        delete queries[i];
    }
    queries.clear();
    answerFiles.clear();
}

TestCase::~TestCase()
{
    freeQueries();
}

void TestCase::addQuery(Query *query)
{
    //For LOAD queries
    queries.push_back(query);
    answerFiles.push_back("");
}

void TestCase::addQuery(Query* query, string answerFile)
{
    //For USUAL queries
    queries.push_back(query);
    answerFiles.push_back(answerFile);
}

void TestCase::execute(Session *session)
{
    Transaction tr(session);
    for (int i = 0; i < queries.size(); i++) {
        tr.execute(*queries[i]);
        
        string result;
        if (!tr.obtainResult(result)) {
            throw std::runtime_error("Execution failed!\n");
        }
        
        std::ifstream correctAnswer(answerFiles[i].c_str());
        std::istringstream resultStream(result);
        
        XmlDiff xmlDiff(correctAnswer, resultStream);
        if (xmlDiff.diff()) {
            throw std::runtime_error("Incorrect answer!\n");
        }
    }
}

void TestCase::generateAnswer(Session *session)
{
    std::cout << "\tGenerating answer for test case " << caseName << std::endl;
    Transaction tr(session);
    for (int i = 0; i < queries.size(); i++) {
        std::cout << "\tQuery file: " << queries[i]->getFilename() << std::endl;
        std::cout << "\tAnswer file: " << answerFiles[i] << std::endl;
        
        tr.execute(*queries[i]);
        
        string result;
        if (!tr.obtainResult(result)) {
            throw std::runtime_error("Execution failed on generation phase!\n");
        }
        
        std::ofstream ans(answerFiles[i].c_str());
        ans << result;
        ans.close();
    }
}
