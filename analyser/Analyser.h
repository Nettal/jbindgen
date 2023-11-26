//
// Created by nettal on 23-11-7.
//

#ifndef JBINDGEN_ANALYSER_H
#define JBINDGEN_ANALYSER_H

#include <string>
#include <clang-c/Index.h>
#include <vector>
#include "StructDeclaration.h"
#include "UnionDeclaration.h"
#include "EnumDeclaration.h"
#include "NormalTypedefDeclaration.h"
#include "NormalMacroDeclaration.h"
#include "FunctionLikeMacroDeclaration.h"
#include "FunctionSymbolDeclaration.h"
#include "FunctionTypeDefDeclaration.h"
#include "VarDeclaration.h"

namespace jbindgen {
    class Analyser {
        CXIndex index4declaration{};
        CXTranslationUnit unit4declaration{};
        CXIndex index4macro{};
        CXTranslationUnit unit4macro{};

    public:
        std::vector<StructDeclaration> structs{};
        std::vector<UnionDeclaration> unions{};
        std::vector<VarDeclaration> vars{};
        std::vector<EnumDeclaration> enums{};
        std::vector<NormalMacroDeclaration> normalMacro{};
        std::vector<FunctionLikeMacroDeclaration> functionLikeMacro{};
        std::vector<FunctionDeclaration> functions{};
        std::vector<FunctionTypedefDeclaration> typedefFunctions{};
        std::vector<NormalTypedefDeclaration> typedefs{};

        Analyser(const std::string&path, const char* const * command_line_args,
                 int num_command_line_args);

        ~Analyser();

        Analyser(const Analyser&that) = delete;

        Analyser& operator=(const Analyser&) = delete;

        void visitStruct(const CXCursor&param);

        void visitUnion(const CXCursor&param);

        void visitEnum(const CXCursor&param);
        void visitVar(const CXCursor&param);

        void visitTypedef(const CXCursor&param);

        void visitNormalMacro(const CXCursor&param);

        void visitFunctionLikeMacro(const CXCursor&param);

        void visitFunction(const CXCursor&param);

        void visitTypeDefFunction(const CXCursor&param);

        void visitStructUnnamedFunctionPointer(const CXCursor&param, const std::string&functionName);

        void visitStructUnnamedStruct(const CXCursor&param, const std::string &structName);

        void visitStructUnnamedUnion(const CXCursor&param, const std::string &structName);
    };
}


#endif //JBINDGEN_ANALYSER_H
