#ifndef _util_xmlhelpers_h
#define _utul_xmlhelpers_h

#include <string>
#include <tinyxml2.h>



inline std::string nullAwareAttr(const char* str) {
    if (str) {
        return std::string(str);
    }

    return std::string();
}

#endif // _util_xmlhelpers_h