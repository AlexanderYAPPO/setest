#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <fstream>
#include <sstream>
#include <libxml++/libxml++.h>

#include "libsedna.h"
#include "diff/xmldiff.h"

#include "XQTSOutputs.h"

#ifdef SETEST_DEBUG
#include <csignal>
#endif

using std::string;

using xmlpp::Document;
using xmlpp::Element;
using xmlpp::Node;

using xmlpp::TextNode;

XmlOutput::XmlOutput(const string &_fileName)
    : IXQTSOutput(principal), fileName(_fileName)
{
}

bool XmlOutput::check(sedna::Query &query)
{
    /* Here, we concatenating all result sequence elements and 
     * comparing them with correct XML */
    
    #ifdef SETEST_DEBUG
        std::cout << "TEST======1======TEST" << std::endl;
    #endif
    if (query.resultCode != SEDNA_QUERY_SUCCEEDED) {
        //Query exited with error or it was bulk load or update (incorrect test case)
	#ifdef SETEST_DEBUG
	    std::cout << "TEST======2======TEST" << std::endl;
	#endif
        return false;
    }
    #ifdef SETEST_DEBUG
        std::cout << "TEST======3======TEST" << std::endl;
    #endif
    
    std::string fullResult;
    for (int i = 0; i < query.resultSequence.size(); i++) {
        fullResult += query.resultSequence[i];
    }
    
    std::ifstream correctAnswer(fileName.c_str(), std::ios_base::in);
    std::istringstream result(fullResult);
    
    #ifdef SETEST_DEBUG
        std::raise(SIGINT);
        std::cout << "TEST======4======TEST" << std::endl;
	std::cout << fileName.c_str() << std::endl;
	std::cout << "=====================" << std::endl;
        std::cout << fullResult << std::endl;
	std::cout << fullResult.length() << std::endl;
	std::cout << "=====================" << std::endl;
	std::string correctAnswerStr = "";
	std::string tmp = "";
	while (std::getline(correctAnswer, tmp)) {
	    correctAnswerStr += tmp;
	}
	correctAnswer.clear();
	correctAnswer.seekg(0, std::ios_base::beg); 
	std::cout << correctAnswerStr << std::endl;
	std::cout << correctAnswerStr.length() << std::endl;
	std::cout << "END======4=======END" << std::endl;
    #endif
    
    XmlDiff xmlDiff(correctAnswer, result);
    return !xmlDiff.diff();
}

TextOutput::TextOutput(const string &_fileName)
    : IXQTSOutput(principal), fileName(_fileName)
{
}

bool TextOutput::check(sedna::Query &query)
{
    #ifdef SETEST_DEBUG
        std::cout << "TEST======A======TEST" << std::endl;
    #endif
    /* Wrapping text and result in XML Containers and compare them as xml */
    
    if (query.resultCode != SEDNA_QUERY_SUCCEEDED) {
        //Query exited with error or it was bulk load or update (incorrect test case)
        return false;
    }
    #ifdef SETEST_DEBUG
        std::cout << "TEST======B======TEST" << std::endl;
    #endif

    string answerString;
    boost::filesystem::path answerFile(fileName);
    answerString.resize(boost::filesystem::file_size(fileName));
    boost::filesystem::ifstream answerFileStream(answerFile);
    answerFileStream.read(&answerString[0], answerString.size());
    
    Document answerDoc;
    Element *answerRoot = answerDoc.create_root_node("TextOutput");
    //answerRoot->add_child_text(answerString);
    TextNode* tmpA = answerRoot->add_child_text(answerString);
    
    Document resultDoc;
    Element *resultRoot = answerDoc.create_root_node("TextOutput");
    //resultRoot->add_child_text(query.resultString);
    TextNode* tmpB = resultRoot->add_child_text(query.resultString);

    #ifdef SETEST_DEBUG
        std::cout << "TEST======D======TEST" << std::endl;
    #endif
    std::istringstream answerStream(answerDoc.write_to_string());
    std::istringstream resultStream(answerDoc.write_to_string());
    
    #ifdef SETEST_DEBUG
        std::cout << "TEST======C======TEST" << std::endl;
        std::cout << query.resultString << std::endl;
	std::string tmpBB = string(tmpB->get_content());
	std::cout << tmpBB << std::endl;
	std::cout << "=====================" << std::endl;
	std::cout << answerString << std::endl;
	std::string tmpAA = string(tmpA->get_content());
	std::cout << tmpAA << std::endl;
	std::cout << "END======C=======END" << std::endl;
    #endif
    
    XmlDiff xmlDiff(answerStream, resultStream);
    return !xmlDiff.diff();
}

ErrorCheck::ErrorCheck() : IXQTSOutput(console), errorCode("")
{
}

ErrorCheck::ErrorCheck(const std::string &_errorCode)
    : IXQTSOutput(console), errorCode(_errorCode)
{
}

bool ErrorCheck::check(sedna::Query &query)
{
    switch (query.resultCode) {
        case SEDNA_QUERY_SUCCEEDED:
        case SEDNA_UPDATE_SUCCEEDED:
        case SEDNA_BULK_LOAD_SUCCEEDED:
        {
            //Correct execution
            return false;
        }
        case SEDNA_QUERY_FAILED:
        case SEDNA_UPDATE_FAILED:
        case SEDNA_BULK_LOAD_FAILED:
        case SEDNA_NEXT_ITEM_FAILED:
        case SEDNA_ERROR:
        {
            if (errorCode.length() == 0) {
                //Any error allowed
                return true;
            }
            //Let's check the error
            if (query.resultSequence.size() == 0) {
                //No error presented
                return false;
            }

            const string &error = query.resultSequence[0];

            const std::string errorPrefix = "SEDNA Message: ERROR ";
            const boost::regex errorFinder("^" + errorPrefix + "[[:alnum:]]*$");
            boost::match_results<string::const_iterator> matched;

            boost::regex_search(error.begin(), error.end(), matched, errorFinder);
            if (matched.size() != 1) {
                //Unexpected error message
                return false;
            }

            string match = matched.str(0);
            match.erase(0, errorPrefix.length());

	    #ifdef SETEST_DEBUG
		std::cout << "TEST=====ERR=====TEST" << std::endl;
		std::cout << match << std::endl;
		std::cout << errorCode << std::endl;
		std::cout << "END=====ERR======END" << std::endl;
	    #endif
            if (match == errorCode) {            
                return true;
            }
            
            return false;
        }
        default:
        {
            std::cerr << "Unexpected execution status in error comparator!\n";
            return false;
        }
    }
    
    if (query.resultCode > 0) {
        //Query exited with error or it was bulk load or update (incorrect test case)
        return false;
    }
}
