#include "utils.h"

bool MyHomeNew::Utils::isInt(String str) {
    unsigned int strLen = str.length();
 
    if (strLen == 0) {
        return false;
    }

    for(unsigned int i = 0; i < strLen; i++) {
        if (isDigit(str.charAt(i))) {
            continue;
        }
        return false;
    }
    return true;
}