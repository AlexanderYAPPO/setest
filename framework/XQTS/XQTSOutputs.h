#ifndef XQTSOUTPUTS_H
#define XQTSOUTPUTS_H

#include <list>
#include <string>

#include <sedna/SednaInterface.h>

class IXQTSOutput
{
public:
    enum output_roles_t
    {
        principal, console
    };
protected:
    output_roles_t role;
public:
    virtual ~IXQTSOutput() {};

    IXQTSOutput(output_roles_t _role) : role(_role) {};

    /** @brief Compares query result
     * @return 0 if compares as equal
     */
    virtual bool check(sedna::Query &query) = 0;
};

/**
 * The XML InfoSets should be canonicalized and compared.
 */
class XmlOutput : public IXQTSOutput
{
    std::string fileName;
    
public:
    XmlOutput(const std::string &_fileName);
    
    bool check(sedna::Query &query);
};

/**
 * Each line of non-whitespace text should match.
 * New-line sequences may vary and should be neutralized.
 * Due to issues with the XML serialization of certain characters (e.g. '<'),
 * it is not possible to simply compare the actual and expected results.
 * Rather (as is the case with the 'Fragment' comparator) the test harness should
 * convert the results into valid XML (by adding a container element) and
 * perform comparison on the XML canonicalized versions of the results.
 */
class TextOutput : public IXQTSOutput
{
    std::string fileName;
public:
    explicit TextOutput(const std::string &_fileName);

    bool check(sedna::Query &query);
};

class ErrorCheck : public IXQTSOutput
{
    std::string errorCode;
public:
    explicit ErrorCheck();
    explicit ErrorCheck(const std::string &errorCode);
    
    bool check(sedna::Query &query);
};

#endif // XQTSOUTPUTS_H
