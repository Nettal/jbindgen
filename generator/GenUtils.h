//
// Created by snownf on 23-11-9.
//

#ifndef JBINDGEN_GENUTILS_H
#define JBINDGEN_GENUTILS_H

#include <string>
#include <cassert>
#include "../analyser/StructDeclaration.h"
#include "Value.h"
#include "../analyser/FunctionSymbolDeclaration.h"
#include "../analyser/FunctionTypeDefDeclaration.h"

namespace jbindgen {
    void overwriteFile(const std::string &file, const std::string &content);

    std::string generateFakeValueLayout(int64_t byteSize);
} // jbindgen

#endif //JBINDGEN_GENUTILS_H
