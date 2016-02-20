#include <iostream>
#include <string>

#include "diff/xmldiff.h"
#include "Tester/Tester.h"

int main(int argc, char **argv)
{
    #ifdef SETEST_DEBUG
    std::cout << argc << std::endl;
    for (int i = 0; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
    }
    #endif
    std::string baseDir(argv[1]);
    std::string config("XQTSCatalog.xml");
    std::string seData(argv[2]);

    Tester tester(baseDir, config, seData);
    tester.runXQTS();

    return 0;
}
