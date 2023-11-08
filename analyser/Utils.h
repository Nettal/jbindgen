//
// Created by nettal on 23-11-7.
//

#ifndef JAVABINDGEN_UTILS_H
#define JAVABINDGEN_UTILS_H

#include <clang-c/Index.h>
#include <string>
#include <ostream>

namespace jbindgen {
    std::string toString(const CXString &s);

    std::string toString(const CXType &t);

    class Typed {
    public:
        const std::string name;
        const CXType type;
        const int64_t size;

        Typed(std::string name, CXType type, int64_t size);

        friend std::ostream &operator<<(std::ostream &stream, const Typed &typed);
    };
}

#endif //JAVABINDGEN_UTILS_H
