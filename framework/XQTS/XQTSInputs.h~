#ifndef XQTSINPUTS_H
#define XQTSINPUTS_H

#include <list>
#include <string>

#include <sedna/SednaInterface.h>

class IXQTSInput
{
public:
  
    enum input_roles_t
    {
        principal, supplemental
    };
    
protected:
  
    input_roles_t role;
    
public:
  
    virtual ~IXQTSInput() {};
    IXQTSInput(input_roles_t _role) : role(_role) {};

    /** @brief Applies input data to a query
     */
    virtual void apply(sedna::Query &query) = 0;
};

class FileSource : public IXQTSInput
{
    std::string variableName;
    std::string docName;
    
public:
    FileSource(const std::string &_docName, const std::string &_variableName);
    void apply(sedna::Query &query);
};

#endif // XQTSINPUTS_H
