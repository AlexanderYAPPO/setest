#include <string>
#include <pugixml.hpp>
#include <stdexcept>

using std::string;

#include "XQTSCatalogParser.h"
#include "XQTSConfig.h"

XQTSSourceList XQTSCatalogParser::parseSources() {
    XQTSSourceList sourceFileList;

    pugi::xml_document doc;
    if (!doc.load_file((baseDir + "/" + XQTSConfig::configFile).c_str())) {
        throw std::runtime_error("Cannot parse xqts catalog file");
    }
    
    pugi::xpath_node_set sourceNodes = doc.select_nodes("//source");
    for (pugi::xpath_node_set::const_iterator sourceNodeIt = sourceNodes.begin(); sourceNodeIt != sourceNodes.end(); sourceNodeIt++) {
        pugi::xml_node sourceNode = sourceNodeIt->node();

        string sourceName = sourceNode.attribute("ID").value();
        string fileName = baseDir + sourceNode.attribute("FileName").value();

        sourceFileList.push_back(std::make_pair(sourceName, fileName));
    }
    return sourceFileList;
}

XQTSTestCaseList XQTSCatalogParser::parseTestCases() {
    pugi::xml_document doc;
    if (!doc.load_file((baseDir + "/" + XQTSConfig::configFile).c_str())) {
        throw std::runtime_error("Cannot parse xqts catalog file");
    }

    XQTSTestCaseList testCases;

    pugi::xpath_node_set testCaseNodes = doc.select_nodes("//test-case");
    for (pugi::xpath_node_set::const_iterator caseNodeIt = testCaseNodes.begin(); caseNodeIt != testCaseNodes.end(); caseNodeIt++) {
        pugi::xml_node caseNode = caseNodeIt->node();

        string filePath = caseNode.attribute("FilePath").value();
        string caseName = caseNode.attribute("name").value();

        XQTSTestCase *testCase = new XQTSTestCase(caseName);
        if (parseTestCase(caseNode, testCase, filePath)) {
            testCases.push_back(testCase);
        }
    }
    return testCases;
}

bool XQTSCatalogParser::parseTestCase(pugi::xml_node &caseNode, XQTSTestCase *testCase, string filePath) {
    //Setting correct query filename
    pugi::xml_node queryNode = caseNode.child("query");
    string queryFile = queryNode.attribute("name").value();
    if (queryFile.length() == 0) {
        return false;
    }
    
    testCase->setQueryFile(baseDir + "/" + XQTSConfig::queryBaseDir + filePath + "/" + queryFile + ".xq");

    //Here we should check inputs and outputs
    for (pugi::xml_node inputFileNode: caseNode.children("input-file")) {
        string role = inputFileNode.attribute("role").value();
        if (role != "principal-data") {
            throw std::runtime_error("Strange test case (contains supplemental-data input): " + testCase->getName());
        }
        
        string inputDoc = inputFileNode.child_value();
        string variable = inputFileNode.attribute("variable").value();
        
        FileSource *input = new FileSource(inputDoc, variable);
        testCase->addInput(input);
    } 

    for (pugi::xml_node outputFileNode: caseNode.children("output-file")) {
        string role = outputFileNode.attribute("role").value();
        if (role != "principal") {
            throw std::runtime_error("Strange test case (contains console-log output): " + testCase->getName());
        }

        string outputFile = outputFileNode.child_value();

        //Building correct path for file
        outputFile = baseDir + "/" + XQTSConfig::answerBaseDir + filePath + "/" + outputFile;
        
        IXQTSOutput *output;
        string comparator = outputFileNode.attribute("compare").value();
        if (comparator == "XML") {
            output = new XmlOutput(outputFile);
        }
        else if (comparator == "Fragment") {
            output = new XmlOutput(outputFile);
        }
        else if (comparator == "Text") {
            output = new TextOutput(outputFile);
        } else if (comparator == "Inspect") {
            continue;
        } else if (comparator == "Ignore") {
            std::cerr << ("Ignore compare method in case: " + testCase->getName());
            continue;
        } else {
            throw std::runtime_error("Unsupported compare method in case: " + testCase->getName());
        }
           
        testCase->addOutput(output);
    }
        
    for (pugi::xml_node expectedErrorCodeNode: caseNode.children("expected-error")) {
        string expectedCode = expectedErrorCodeNode.child_value();

        IXQTSOutput *output;
        if (expectedCode == "*") {
            output = new ErrorCheck();
        } else {
            output = new ErrorCheck(expectedCode);
        }
        testCase->addOutput(output);
    }

    return true;
}
