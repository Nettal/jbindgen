//
// Created by snownf on 23-11-8.
//

#include "FunctionDeclaration.h"
#include "Analyser.h"

#include <utility>

namespace jbindgen {
    FunctionDeclaration FunctionDeclaration::visit(CXCursor c) {
        auto type = clang_getCursorType(c);
        auto funcName = toString(clang_getCursorSpelling(c));
        auto retName = toString((clang_getTypeSpelling(clang_getResultType(type))));
        auto size = clang_Type_getSizeOf(type);
        Typed retType(NO_NAME, type, size, NO_COMMIT);
        Typed functionType(funcName, type, clang_Type_getSizeOf(type), getCommit(c));
        FunctionDeclaration def(functionType, retType, toString(clang_getTypeSpelling(clang_getCanonicalType(type))));

        for (int i = 0; i < clang_getNumArgTypes(type); ++i) {
            auto argType = clang_getArgType(type, i);
            auto name = toString(clang_getCursorSpelling(clang_Cursor_getArgument(c, i)));
            name = name.empty() ? NO_NAME : name;
            Typed par(name, argType, clang_Type_getSizeOf(argType), NO_COMMIT);
            def.addPara(par);
        }
        return def;
    }


    FunctionDeclaration::FunctionDeclaration(Typed function, jbindgen::Typed ret, std::string canonicalName)
            : function(std::move(function)),
              ret(std::move(ret)),
              canonicalName(std::move(canonicalName)) {

    }

    void FunctionDeclaration::addPara(Typed typed) {
        paras.push_back(std::move(typed));
    }

    std::ostream &operator<<(std::ostream &stream, const FunctionDeclaration &function) {
        stream << "#### Function " << std::endl;
        stream << "  " << function.ret << " " << function.function.name << " ";
        for (const auto &item: function.paras) {
            stream << item << " ";
        }
        stream << std::endl;
        return stream;
    }
}