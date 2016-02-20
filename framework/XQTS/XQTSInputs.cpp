#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <fstream>
#include <sstream>

#include <libsedna.h>
#include "diff/xmldiff.h"

#include "XQTSInputs.h"

using std::string;


FileSource::FileSource(const string &_docName, const string &_variableName) 
    : IXQTSInput(principal), docName(_docName), variableName(_variableName)
{
}

void FileSource::apply(sedna::Query& query)
{
    /* Here we should analyze query body and substitute input document 
     * instead of variable occurences */
    
    /* Constructing regex to remove variable declarations */
    const boost::regex removeDeclExp("(declare)[[:space:]]*(variable)[[:space:]]*\\$[[:print:]]*[[:space:]]*(external);");
    const string replaceStr1 = "";
    
    /* Constructing regex to replace occurances */
    const boost::regex renameVarExp("\\$" + variableName);
    const string replaceStr2 = docName;    
    
    /* Applying expressions */
    
    query.body = boost::regex_replace(query.body, removeDeclExp, replaceStr1);
    query.body = boost::regex_replace(query.body, renameVarExp, replaceStr2);
}
