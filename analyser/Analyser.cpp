//
// Created by nettal on 23-11-7.
//

#include "Analyser.h"
#include <iostream>
#include <cstdint>

using std::ostream;
using std::cout;
using std::cerr;
using std::endl;
using std::flush;

namespace jbindgen {
    Analyser::Analyser(const std::string &path, const char *const *command_line_args, int num_command_line_args) {
        CXIndex index = clang_createIndex(0, 0);
        CXTranslationUnit unit;
        {
            clang_parseTranslationUnit2(
                    index,
                    path.c_str(), command_line_args, num_command_line_args,
                    nullptr, 0,
                    CXTranslationUnit_SkipFunctionBodies, &unit);
            if (unit == nullptr) {
                cerr << "Unable to parse translation unit. Quitting." << endl;
                exit(-1);
            }
            CXCursor cursor = clang_getTranslationUnitCursor(unit);
            intptr_t ptrs[] = {reinterpret_cast<intptr_t>(this),
                               reinterpret_cast<intptr_t>(&unit)};
            clang_visitChildren(
                    cursor,
                    [](CXCursor c, CXCursor parent, CXClientData ptrs) {
                        if (DEBUG_LOG) {
                            CXFile file;
                            unsigned line;
                            unsigned column;
                            unsigned offset;
                            clang_getSpellingLocation(clang_getCursorLocation(c), &file, &line, &column, &offset);
                            cout << " line: " << line << " column: "
                                 << column << " offest: " << offset << endl << std::flush;
                        }
                        CXCursorKind cursorKind = clang_getCursorKind(c);
                        if (cursorKind == CXCursor_UnexposedDecl) {
                            throw std::runtime_error("CXCursor_UnexposedDecl");
                        }
                        if (cursorKind == CXCursor_StructDecl) {
                            reinterpret_cast<Analyser *>((reinterpret_cast<intptr_t *>(ptrs))[0])->visitStruct(c);
                        }
                        if (cursorKind == CXCursor_UnionDecl) {
                            reinterpret_cast<Analyser *>((reinterpret_cast<intptr_t *>(ptrs))[0])->visitUnion(c);
                        }
                        if (cursorKind == CXCursor_TypedefDecl) {
                            reinterpret_cast<Analyser *>((reinterpret_cast<intptr_t *>(ptrs))[0])->visitTypedef(c);
                        }
                        if (cursorKind == CXCursor_FunctionDecl) {
                            reinterpret_cast<Analyser *>((reinterpret_cast<intptr_t *>(ptrs))[0])->visitFunction(c);
                        }
                        if (cursorKind == CXCursor_ClassDecl || cursorKind == CXCursor_CXXMethod) {
                            throw std::runtime_error("CXCursor_ClassDecl");
                        }
                        if (cursorKind == CXCursor_VarDecl || cursorKind == CXCursor_FieldDecl) {
                            cerr << "VarDecl || FieldDecl: " << cxstring2string(clang_getCursorSpelling(c)) << endl;
                        }
                        if (cursorKind == CXCursor_EnumConstantDecl || cursorKind == CXCursor_EnumDecl) {
                            reinterpret_cast<Analyser *>((reinterpret_cast<intptr_t *>(ptrs))[0])->visitEnum(c);
                        }
                        if (cursorKind == CXCursor_ParmDecl) {
                            throw std::runtime_error("CXCursor_ParmDecl");
                        }
                        return CXChildVisit_Continue;
                    },
                    ptrs);
            clang_disposeTranslationUnit(unit);
        }
    }

    void Analyser::visitStruct(CXCursor param) {

    }
}
