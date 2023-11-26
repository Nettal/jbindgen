//
// Created by nettal on 23-11-7.
//

#ifndef JBINDGEN_STRUCTDECLARATION_H
#define JBINDGEN_STRUCTDECLARATION_H

#include <clang-c/Index.h>
#include <vector>
#include <cstdint>
#include "AnalyserUtils.h"

namespace jbindgen {
    class Analyser;

    class StructMember {
    public:
        const jbindgen::VarDeclare var;
        const int64_t offsetOfBit;

        explicit StructMember(jbindgen::VarDeclare type, int64_t offsetOfBit);

        friend std::ostream &operator<<(std::ostream &stream, const StructMember &member);
    };

    class StructDeclaration {
    protected:
        static CXChildVisitResult visitChildren(CXCursor cursor,
                                                CXCursor parent,
                                                CXClientData client_data);

    public:
        static StructDeclaration visit(CXCursor c, Analyser &analyser);

        static StructDeclaration visitStructUnnamed(CXCursor c, const std::string &basicString, Analyser &analyser);

        const VarDeclare structType;

        std::vector<jbindgen::StructMember> members{};

        explicit StructDeclaration(VarDeclare structType);

        friend std::ostream &operator<<(std::ostream &stream, const StructDeclaration &str);
    };
}

#endif //JBINDGEN_STRUCTDECLARATION_H
