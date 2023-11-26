//
// Created by snownf on 23-11-8.
//

#ifndef JBINDGEN_FUNCTIONDECLARATION_H
#define JBINDGEN_FUNCTIONDECLARATION_H


#include <string>
#include <vector>
#include "AnalyserUtils.h"

namespace jbindgen {
    class FunctionDeclaration {
    public:
        FunctionDeclaration(VarDeclare function, jbindgen::VarDeclare ret, std::string canonicalName);

        const VarDeclare function;
        std::string canonicalName;
        VarDeclare ret;
        std::vector<VarDeclare> paras;

        void addPara(VarDeclare typed);

        friend std::ostream &operator<<(std::ostream &stream, const FunctionDeclaration &function);

        static FunctionDeclaration visit(CXCursor c);
    };

}
#endif //JBINDGEN_FUNCTIONDECLARATION_H
