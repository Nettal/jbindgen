//
// Created by nettal on 23-11-8.
//

#include "FunctionTypeDefDeclaration.h"

#include <utility>
#include <cassert>

namespace jbindgen {

    FunctionTypedefDeclaration::FunctionTypedefDeclaration(VarDeclare function, VarDeclare ret,
                                                           std::string canonicalName, CXCursor cursor)
            : function(std::move(function)),
              ret(std::move(ret)), canonicalName(std::move(canonicalName)) {
    }

    std::ostream &operator<<(std::ostream &stream, const FunctionTypedefDeclaration &function) {
        stream << "#### TypedefFunction " << std::endl;
        stream << "  " << function.ret << " " << function.function.name << " ";
        for (const auto &item: function.paras) {
            stream << item << " ";
        }
        stream << std::endl;
        return stream;
    }

    FunctionTypedefDeclaration FunctionTypedefDeclaration::visit(CXCursor cursor) {
        assert(cursor.kind == CXCursor_TypedefDecl);
        auto functionName = toString(clang_getCursorSpelling(cursor));
        auto functionType = clang_getPointeeType(clang_getTypedefDeclUnderlyingType(cursor));
        assert(functionType.kind == CXType_FunctionProto || functionType.kind == CXType_FunctionNoProto);
        auto ret = clang_getResultType(functionType);
        VarDeclare function(functionName, functionType, clang_Type_getSizeOf(functionType), getCommit(cursor), cursor);
        FunctionTypedefDeclaration declaration(function,
                                               VarDeclare(NO_NAME, ret, clang_Type_getSizeOf(ret), NO_COMMIT, cursor),
                                               toStringWithoutConst(clang_getCanonicalType(functionType)), cursor);
        clang_visitChildren(cursor, FunctionTypedefDeclaration::visitChildren, &declaration);
        return declaration;
    }

    FunctionTypedefDeclaration FunctionTypedefDeclaration::visitFunctionUnnamedPointer(CXCursor cursor,
                                                                                       const std::string &functionName) {
        assert(cursor.kind == CXCursor_FieldDecl);
        const auto functionType = clang_getPointeeType(clang_getCursorType(cursor));
        assert(functionType.kind == CXType_FunctionProto || functionType.kind == CXType_FunctionNoProto);
        const auto ret = clang_getResultType(functionType);
        VarDeclare function(functionName, functionType, clang_Type_getSizeOf(functionType), getCommit(cursor), cursor);
        FunctionTypedefDeclaration declaration(function,
                                               VarDeclare(NO_NAME, ret, clang_Type_getSizeOf(ret), NO_COMMIT, cursor),
                                               toStringWithoutConst(clang_getCanonicalType(functionType)), cursor);
        clang_visitChildren(cursor, FunctionTypedefDeclaration::visitChildren, &declaration);
        return declaration;
    }

    enum CXChildVisitResult
    FunctionTypedefDeclaration::visitChildren(CXCursor cursor, CXCursor parent, CXClientData client_data) {
        if (cursor.kind == CXCursor_ParmDecl) {
            auto name = toString(clang_getCursorSpelling(cursor));
            auto type = clang_getCursorType(cursor);
            VarDeclare typed(name, type, clang_Type_getSizeOf(type), getCommit(cursor), cursor);
            reinterpret_cast<FunctionTypedefDeclaration *>(client_data)->paras
                    .emplace_back(typed);
        }
        return CXChildVisit_Continue;
    }
} // jbindgen