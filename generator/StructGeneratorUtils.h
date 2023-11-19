//
// Created by snownf on 23-11-9.
//

#ifndef JAVABINDGEN_STRUCTGENERATORUTILS_H
#define JAVABINDGEN_STRUCTGENERATORUTILS_H

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <cassert>
#include "Value.h"
#include "../analyser/Analyser.h"
#include "GenUtils.h"
#include "StructGenerator.h"

#define NEXT_LINE  << std::endl
#define END_LINE std::endl

namespace jbindgen {
    CXType toPointeeType(CXType type);

    CXType toDeepPointeeType(CXType type);

    int32_t getPointeeDepth(CXType type);

    std::string toPointerName(const VarDeclare &declare);

    std::string toDeepPointerName(const VarDeclare &declare);

    int64_t getArrayLength(CXType type);

    std::string toArrayName(const VarDeclare &declare);

    std::string toVarDeclareString(const VarDeclare &varDeclare);

    class StructGeneratorUtils {
    public:
        static std::vector<Setter>
        defaultStructDecodeSetter(const jbindgen::StructMember &structMember,
                                  const std::string &ptrName, void *pUserdata);

        static std::vector<Getter>
        defaultStructDecodeGetter(const jbindgen::StructMember &structMember,
                                  const std::string &ptrName, void *pUserdata);

        static std::string
        defaultStructMemberName(const StructDeclaration &declaration, const StructMember &member, void *pUserdata) {
            return member.var.name;
        };

        static std::string
        makeCore(const std::string &imported, const std::string &packageName, const std::string &structName,
                 long byteSize,
                 const std::string &toString,
                 const std::string &getter_setter);
    };
}


#endif //JAVABINDGEN_STRUCTGENERATORUTILS_H
