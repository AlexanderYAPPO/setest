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


#include "testgroup.h"

#include <map>
#include <iostream>

TestGroup::TestGroup(const std::string& groupName)
{
    this->groupName = groupName;
}

void TestGroup::freeCases()
{
    for (int i = 0; i < cases.size(); i++) {
        delete cases[i];
    }
    cases.clear();
}

TestGroup::~TestGroup()
{
    freeCases();
}

void TestGroup::addCase(TestCase* testCase)
{
    cases.push_back(testCase);
}

void TestGroup::execute(Session *session)
{
    for (int i = 0; i < cases.size(); i++) {
        cases[i]->execute(session);
    }
}

void TestGroup::generateAnswers(Session *session)
{
    std::cout << "Starting answer generation for test group " << groupName << std::endl;
    for (int i = 0; i < cases.size(); i++) {
        cases[i]->generateAnswer(session);
    }
    std::cout << "Answer generation for test group " << groupName << "complete\n";
}
