#include "Tester.h"

#include <string>
#include <libxml++/libxml++.h>
#include <xercesc/dom/DOM.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <grp.h>

#include "sedna/SednaInterface.h"

using std::string;
using xmlpp::Node;
using xmlpp::Element;
using xmlpp::ContentNode;
using SeTest::Query;
using SeTest::QueryType;

Tester::Tester(string &baseDir, string &configFile, string &sednaDir): 
    configParser((baseDir + "/" + configFile).c_str()), sednaInstance(sednaDir)
{
    this->configFile = baseDir + "/" + configFile;
    this->baseDir = baseDir;
    this->sednaDir = sednaDir;
    
    this->queryDir = baseDir + "/Tests/";
    this->answerDir = baseDir + "/ExpectedResults/";
    this->dataDir = baseDir + "/TestData/";

    sednaInstance.startSedna();

    if (!configParser) {
        throw std::runtime_error("Cannot parse config file");
    }
    
    configRoot = configParser.get_document()->get_root_node();
    
    parseGroups();
}

Tester::~Tester()
{
    freeGroups();
    //Sedna instance will be stopped in destructor
}

void Tester::freeGroups()
{
    for (int i = 0; i < groups.size(); i++) {
        delete groups[i];
    }
    groups.clear();
}

void Tester::parseTests(xmlpp::Element *groupNode, TestGroup *testGroup)
{
    Node::NodeList testNodes = groupNode->get_children("TestCase");
    
    TestCase *testCase;
    
    for (Node::NodeList::iterator node = testNodes.begin(); node != testNodes.end(); node++) {
        Element *testNode = dynamic_cast<Element *>(*node);
        if (!testNode) {
            throw std::runtime_error("TestCase node is not an element");
        }
        
        string name = string(testNode->get_attribute_value("name"));
        if (name.length() == 0) {
            throw std::runtime_error("Case doesn't have a name");
        }
        
        string filepath = string(testNode->get_attribute_value("filepath"));
        if (filepath.length() == 0) {
            throw std::runtime_error("Case " + name + " doesn't have a filepath");
        }

        testCase = new TestCase(name);
        
        xmlpp::Node::NodeList inputFilesNodeList = testNode->get_children("InputFiles");
        Element *inputFilesNode = dynamic_cast<Element *>(*inputFilesNodeList.begin());
        if (!inputFilesNode) {
            throw std::runtime_error("InputFiles node is not correct");
        }
        
        Node::NodeList inFileNodes = inputFilesNode->get_children("InputFile");
        for (Node::NodeList::iterator inFile = inFileNodes.begin(); inFile != inFileNodes.end(); inFile++) {
            Element *inputFileNode = dynamic_cast<Element *>(*inFile);
            if (!inputFileNode) {
                throw std::runtime_error("InputFile node is not correct");
            }
            
            string queryTypeStr = string(inputFileNode->get_attribute_value("QueryType"));
            if (queryTypeStr == "USUAL") {
                Node::NodeList inFileNames = inputFileNode->get_children();
                ContentNode *fileNameNode = dynamic_cast<ContentNode *>(*inFileNames.begin());
                if (!fileNameNode) {
                    throw std::runtime_error("InputFile node is not correct in case " + name);
                }
                
                string filename = filepath + "/" + string(fileNameNode->get_content());
                
                Query *query = new Query(this->queryDir + filename);
                testCase->addQuery(query, this->answerDir + filename);
            } 
            else if (queryTypeStr == "LOAD") {
                string sourceFile = string(inputFileNode->get_attribute_value("SourceFile"));
                string docname = string(inputFileNode->get_attribute_value("Document"));
                string colname = string(inputFileNode->get_attribute_value("Collection"));
                
                Query *query = new Query(this->dataDir + sourceFile, docname, colname);
                testCase->addQuery(query);
            } else {
                throw std::runtime_error("QueryType attribute is not correct in case " + name + ": " + queryTypeStr);
                
            }
        }
        testGroup->addCase(testCase);
    }
}

void Tester::parseGroups()
{
    xmlpp::Node::NodeList groupNodes = configRoot->get_children("TestGroup");
    
    TestGroup *testGroup;
    
    for (Node::NodeList::iterator node = groupNodes.begin(); node != groupNodes.end(); node++) {
        Element *groupNode = dynamic_cast<Element *>(*node);
        
        if (!groupNode) {
            throw std::runtime_error("TestGroup node is not an element");
        }
        
        string name = string(groupNode->get_attribute_value("name"));
        if (name.length() == 0) {
            throw std::runtime_error("Group doesn't have a name");
        }
        testGroup = new TestGroup(name);
        
        groups.push_back(testGroup);
        parseTests(groupNode, testGroup);
    }
}

void Tester::executeAll(string dbName)
{
    Database *dbInstance = sednaInstance.getDatabase("dbName");
    dbInstance->create();
    dbInstance->start();
    
    Session *session = dbInstance->createSession();
    
    for (int i = 0; i < groups.size(); i++) {
        groups[i]->execute(session);
    }
    
    dbInstance->stop();
    dbInstance->drop();
}

void Tester::generateAllAnswers(string dbName)
{
    std::cout << "\nStarting generating answers for all presented test cases \n";
    std::cout << "\nInitializing Sedna \n";
    
    Database *dbInstance = sednaInstance.getDatabase("dbName");
    dbInstance->create();
    dbInstance->start();
    
    Session *session = dbInstance->createSession();
    
    std::cout << "\nSedna succesfully initialized\n";
    
    for (int i = 0; i < groups.size(); i++) {
        groups[i]->generateAnswers(session);
    }
    
    std::cout << "\nAnswer generation complete\n";
    dbInstance->stop();
    dbInstance->drop();
}
