//
// Created by nettal on 23-11-7.
//

#include "StructDeclaration.h"
#include "Analyser.h"

#include <utility>
#include <iostream>
#include "Member.h"

using std::cout;

namespace jbindgen {


    StructDeclaration::StructDeclaration(Typed structType) : structType(std::move(structType)) {
    }

    StructDeclaration StructDeclaration::visit(CXCursor c) {
        auto name = toString(clang_getCursorSpelling(c));
        auto type = clang_getCursorType(c);
        StructDeclaration declaration(Typed(name, type, clang_Type_getSizeOf(type)));
        if (declaration.structType.size < 0) {
            return declaration;
        }
        intptr_t pUser[] = {reinterpret_cast<intptr_t>(&declaration), (intptr_t) ""};
        clang_visitChildren(c, StructDeclaration::visitChildren, pUser);
        if (DEBUG_LOG) {
            cout << declaration;
        }
        return declaration;
    }

    CXChildVisitResult StructDeclaration::visitChildren(CXCursor cursor, CXCursor parent, CXClientData client_data) {
        auto *this_ptr = (StructDeclaration *) (reinterpret_cast<intptr_t *>(client_data)[0]);
        auto prefix = reinterpret_cast<const char *>((reinterpret_cast<intptr_t *>(client_data)[1]));
        if (clang_getCursorKind(cursor) == CXCursor_FieldDecl) {
            int64_t offset = clang_Cursor_getOffsetOfField(cursor);
            const CXType &cursorType = clang_getCursorType(cursor);
            auto name = prefix + toString(clang_getCursorSpelling(cursor));
            if (cursorType.kind == CXType_Elaborated &&
                clang_getCursorType(clang_getTypeDeclaration(cursorType)).kind ==
                CXType_Record) {//unnamed struct or union
                std::string prefixed = name + "$";
                intptr_t pUserData[] = {((intptr_t) this_ptr), (intptr_t) (prefixed.c_str())};
                clang_visitChildren(
                        clang_getTypeDeclaration(cursorType),
                        StructDeclaration::visitChildren,
                        pUserData);
                return CXChildVisit_Continue;
            }
            if (offset < 0) {
                throw std::runtime_error(std::to_string(static_cast<int64_t>(offset)));
            }
            auto member = jbendgen::Member(Typed(name, cursorType, clang_Type_getSizeOf(cursorType)), offset);
            this_ptr->members.emplace_back(member);
        }
        return CXChildVisit_Continue;
    }
}
