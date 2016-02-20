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


#ifndef TESTCASE_H
#define TESTCASE_H

#include <vector>

#include "sedna/isednatest.h"
#include "sedna/sednainterface.h"

using std::vector;
using std::string;

class TestCase : public ISednaTest
{
    string caseName;
    vector<Query *> queries;
    vector<string> answerFiles;

public:
    TestCase(const string &caseName);
    ~TestCase();
    
    void freeQueries();
    
    void addQuery(Query *query);
    void addQuery(Query *query, string answerFile);

    virtual void execute(Session *session);
    virtual void generateAnswer(Session *session);
};

#endif // TESTCASE_H
