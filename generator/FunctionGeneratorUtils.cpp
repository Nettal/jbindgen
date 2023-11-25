//
// Created by snownf on 23-11-15.
//

#include <iostream>
#include <cassert>
#include "FunctionGeneratorUtils.h"
#include "StructGeneratorUtils.h"
#include "Value.h"

namespace jbindgen::functiongenerator {
    //  j_type , fd , need allocator
    std::tuple<std::string, std::string, bool> processDirectCallType(const VarDeclare &varDeclare) {
        auto copyMethod = value::method::typeCopy(varDeclare.type, varDeclare.cursor);
        auto ffm = value::method::copy_method_2_ffm_type(copyMethod);
        if (ffm.type != value::jbasic::type_other && !value::method::copy_method_is_value(copyMethod)) {
            assert(copyMethod != value::method::copy_error);
            assert(copyMethod != value::method::copy_void);
            assert(copyMethod != value::method::copy_internal_function_proto);
            return {ffm.primitive(), ffm.value_layout(), false};
        }
        if (value::method::copy_method_is_value(copyMethod)) {
            return {value::jext::MemorySegment.primitive(), value::jext::MemorySegment.value_layout(), false};
        }
        if (value::method::copy_by_set_memory_segment_call == copyMethod ||
            value::method::copy_by_ext_int128_call == copyMethod ||
            value::method::copy_by_ext_long_double_call == copyMethod ||
            value::method::copy_by_ptr_copy_call == copyMethod) {
            return {value::jext::MemorySegment.primitive(), value::jext::MemorySegment.value_layout(), false};
        }
        if (value::method::copy_by_ptr_dest_copy_call == copyMethod) {
            return {value::jext::MemorySegment.primitive(), varDeclare.name + "." + VALUE_LAYOUT, true};
        }
        if (value::method::copy_by_array_call == copyMethod) {
            auto len = getArrayLength(varDeclare.type);
            if (len == -1) {
                return {value::jext::MemorySegment.primitive(), value::jext::MemorySegment.value_layout(), false};
            }
            auto depth = getPointeeOrArrayDepth(varDeclare.type);
            if (depth < 2) {
                auto name = toArrayName(varDeclare);
                return {value::jext::MemorySegment.primitive(),
                        "MemoryLayout.sequenceLayout(" + std::to_string(len) + ","
                        + name + "." + VALUE_LAYOUT + ")", true};
            } else {
                return {value::jext::MemorySegment.primitive(), generateFakeValueLayout(varDeclare.byteSize), true};
            }
        }
        assert(0);
    }

    //  j parameters , fds , invokes
    std::tuple<std::vector<std::string>, std::vector<std::string>, std::vector<std::string>>
    makeParameter(const jbindgen::FunctionDeclaration &declare) {
        std::vector<std::string> jParameters;
        std::vector<std::string> fds;
        std::vector<std::string> invokes;
        int i = 0;
        for (const auto &item: declare.paras) {
            //j_type fd
            auto pre = processDirectCallType(item);
            std::string paraName = item.name;
            if (std::equal(item.name.begin(), item.name.end(), NO_NAME)) {
                paraName = "para" + std::to_string(i);
            }
            jParameters.emplace_back(get<0>(pre) + " " + paraName);
            fds.emplace_back(get<1>(pre));
            invokes.emplace_back(paraName);
            i++;
        }
        return {jParameters, fds, invokes};
    }

    std::vector<FunctionWrapperInfo> makeWrappers(const FunctionDeclaration &declaration);
    //wrapper type,decode way,encode way
    struct wrapper {
        std::string type;
        std::string decode;
        std::string encode;
    };

    FunctionInfo defaultMakeFunctionInfo(const jbindgen::FunctionDeclaration *declaration,
                                         void *pUserdata) {
        std::cout << declaration->function << declaration->ret << declaration->canonicalName
                  << std::endl;
        for (const auto &para: declaration->paras) {
            std::cout << para << std::endl;
        }
        FunctionInfo info;
        info.functionName = declaration->function.name;
        //parameter
        auto parameters = makeParameter(*declaration);
        info.jParameters = get<0>(parameters);
        info.functionDescriptors = get<1>(parameters);
        info.invokeParameters = get<2>(parameters);
        //result
        info.hasResult = declaration->ret.type.kind != CXType_Void;
        if (info.hasResult) {
            auto ret = processDirectCallType(declaration->ret);
            info.jResult = get<0>(ret);
            info.resultDescriptor = get<1>(ret);
            info.needAllocator = get<2>(ret);
        }
        info.wrappers = makeWrappers(*declaration);
        return info;
    }

    std::string callNew(const VarDeclare &declare, const std::string &clazz) {
        return "new " + clazz + "(" + declare.name + ")";
    }

    std::string callList(const VarDeclare &declare, const std::string &clazz) {
        return clazz + ".list(" + declare.name + ")";
    }

    std::string callLambda(const VarDeclare &declare) {
        return "() ->{" + declare.name + "}";
    }

    std::vector<wrapper> processWrapperCallType(const VarDeclare &declare) {
        std::vector<wrapper> optional;
        auto typeName = toVarDeclareString(declare);
        auto copyMethod = value::method::typeCopy(declare.type, declare.cursor);
        switch (copyMethod) {
            case value::method::copy_by_set_memory_segment_call:
                optional.emplace_back((wrapper) {"Pointer<?>", ".pointer()", callLambda(declare)});
                break;
            case value::method::copy_by_value_memory_segment_call:
                optional.emplace_back((wrapper) {"Value<MemorySegment>", ".value()", callLambda(declare)});
                break;
            case value::method::copy_by_ptr_dest_copy_call:
                optional.emplace_back((wrapper) {typeName, ".pointer()", callNew(declare, typeName)});
                break;
            case value::method::copy_by_ptr_copy_call: {
                auto pointee = toPointeeType(declare.type, declare.cursor);
                assert(pointee.kind != CXType_Invalid);
                auto pointeeCopy = value::method::typeCopy(pointee, clang_getTypeDeclaration(pointee));
                if (pointeeCopy == value::method::copy_by_set_j_byte_call) {//maybe a String
                    optional.emplace_back(
                            (wrapper) {JString, ".pointer()", callNew(declare, JString)});
                    optional.emplace_back((wrapper) {value::jbasic::Byte.native_wrapper(), ".pointer()",
                                                     callNew(declare, value::jbasic::Byte.native_wrapper())});
                    optional.emplace_back(
                            (wrapper) {NativeValue + "<" + value::jbasic::Byte.native_wrapper() + ">", ".pointer()",
                                       callList(declare, value::jbasic::Byte.native_wrapper())});
                    break;
                }
                const std::string &pointerName = toPointerName(declare);
                const value::jbasic::FFMType &pointeeType = copy_method_2_ffm_type(pointeeCopy);
                if (pointeeType.type != value::jbasic::type_other) {
                    if (copy_method_is_value(pointeeCopy)) {
                        optional.emplace_back((wrapper) {NativeValue + "<" + pointerName + ">", ".pointer()",
                                                         callList(declare, pointerName)});
                        break;
                    } else {
                        optional.emplace_back((wrapper) {
                                NativeArray + "<" + pointeeType.native_wrapper() + ">",
                                ".pointer()", callList(declare, pointeeType.native_wrapper())});
                        optional.emplace_back(
                                (wrapper) {pointeeType.native_wrapper(), ".pointer()",
                                           callNew(declare, pointeeType.native_wrapper())});
                        break;
                    }
                }
                auto depth = getPointeeOrArrayDepth(declare.type);
                if (depth < 2) {
                    optional.emplace_back((wrapper) {NativeArray + "<" + pointerName + ">", ".pointer()",
                                                     callList(declare, pointerName)});
                    optional.emplace_back((wrapper) {pointerName, ".pointer()", callNew(declare, pointerName)});
                    break;
                }
                auto deepType = toDeepPointeeType(declare.type, declare.cursor);
                assert(deepType.kind != CXType_Invalid);
                std::string jType;
                std::string end;
                for (int i = 0; i < depth; ++i) {
                    jType += "Pointer<";
                    end += ">";
                }
                auto deepCopy = value::method::typeCopy(deepType, clang_getTypeDeclaration(deepType));
                const value::jbasic::FFMType &elementFFM = copy_method_2_ffm_type(deepCopy);
                if (elementFFM.type != value::jbasic::type_other &&
                    !value::method::copy_method_is_value(deepCopy)) {
                    optional.emplace_back((wrapper) {jType + elementFFM.native_wrapper() + end, ".pointer()",
                                                     callLambda(declare)});
                    break;
                }
                optional.emplace_back((wrapper) {jType + toStringWithoutConst(deepType) + end, ".pointer()",
                                                 callLambda(declare)});
                break;
            }
            case value::method::copy_by_array_call: {
                auto depth = getPointeeOrArrayDepth(declare.type);
                if (depth < 2)
                    optional.emplace_back((wrapper) {NativeArray + "<" + toArrayName(declare) + ">", ".pointer()"});
                else {
                    std::string jType;
                    std::string end;
                    for (int i = 0; i < getPointeeOrArrayDepth(declare.type); ++i) {
                        jType += "Pointer<";
                        end += ">";
                    }
                    auto type = toDeepPointeeType(declare.type, declare.cursor);
                    assert(type.kind != CXType_Invalid);
                    auto copy = value::method::typeCopy(type, clang_getTypeDeclaration(type));
                    const value::jbasic::FFMType &elementFFM = copy_method_2_ffm_type(copy);
                    if (elementFFM.type != value::jbasic::type_other &&
                        !value::method::copy_method_is_value(copy)) {
                        optional.emplace_back((wrapper) {jType + elementFFM.native_wrapper() + end, ".pointer()",
                                                         callLambda(declare)});
                    } else
                        optional.emplace_back((wrapper) {jType + toStringWithoutConst(type) + end, ".pointer()",
                                                         callLambda(declare)});
                }
                break;
            }
            case value::method::copy_by_set_j_int_call:
            case value::method::copy_by_set_j_long_call:
            case value::method::copy_by_set_j_float_call:
            case value::method::copy_by_set_j_double_call:
            case value::method::copy_by_set_j_char_call:
            case value::method::copy_by_set_j_short_call:
            case value::method::copy_by_set_j_byte_call:
            case value::method::copy_by_set_j_bool_call:
                optional.emplace_back((wrapper) {copy_method_2_ffm_type(copyMethod).primitive(), "", declare.name});
                break;
            case value::method::copy_by_value_j_int_call:
            case value::method::copy_by_value_j_long_call:
            case value::method::copy_by_value_j_float_call:
            case value::method::copy_by_value_j_double_call:
            case value::method::copy_by_value_j_char_call:
            case value::method::copy_by_value_j_short_call:
            case value::method::copy_by_value_j_byte_call:
            case value::method::copy_by_value_j_bool_call:
                optional.emplace_back((wrapper) {typeName, ".value()", callNew(declare, typeName)});
                break;
            case value::method::copy_by_ext_int128_call:
            case value::method::copy_by_ext_long_double_call:
                optional.emplace_back((wrapper) {
                        value::method::copy_method_2_ext_type(copyMethod).native_wrapper, ".pointer()",
                        callNew(declare, value::method::copy_method_2_ext_type(copyMethod).native_wrapper)});
                break;
            case value::method::copy_error:
            case value::method::copy_void:
            case value::method::copy_internal_function_proto:
                assert(0);
        }
        assert(!optional.empty());
        return optional;
    }

    static void
    generateWrap(const std::vector<std::vector<std::string>> &elem, std::vector<std::vector<std::string> *> &paras) {
        if (elem.empty()) {
            return;
        }
        auto iter = elem[0].begin();
        auto s = *iter;
        paras[0]->emplace_back(s);
        auto batch = paras.size() / elem[0].size();
        decltype(batch) lastBatch = 0;
        for (int i = 1; i <= paras.size(); i++) {
            if (i % batch == 0) {
                auto subElem = std::vector(elem.begin() + 1, elem.end());
                auto subParas = std::vector(paras.begin() + lastBatch, paras.begin() + i);
                generateWrap(subElem, subParas);
                iter++;
                if (iter == elem[0].end())
                    break;
                s = *iter;
                lastBatch = i;
            }
            paras[i]->emplace_back(s);
        }
    }

    std::vector<FunctionWrapperInfo> makeWrappers(const FunctionDeclaration &declaration) {
        std::vector<std::vector<std::string>> jOptions;
        std::vector<std::vector<std::string>> decodeOptions;
        std::vector<std::vector<std::string>> encodeOptions;
        std::vector<std::vector<std::string> *> jParameters;
        std::vector<std::vector<std::string> *> decodeParameters;
        std::vector<std::vector<std::string> *> encodeParameters;
        size_t i = 1;
        for (const auto &item: declaration.paras) {
            auto para = processWrapperCallType(item);
            assert(!para.empty());
            i *= para.size();
            std::vector<std::string> jOption;
            std::vector<std::string> decodeOption;
            std::vector<std::string> encodeOption;
            int paraCount = 1;
            for (auto &p: para) {
                paraCount++;
                //todo fix it
                if (!std::equal(item.name.begin(), item.name.end(), NO_NAME)) {
                    jOption.emplace_back(p.type + " " + "para" + std::to_string(paraCount));
                    decodeOption.emplace_back("para" + std::to_string(paraCount) + " " + p.decode);
                    encodeOption.emplace_back("para" + std::to_string(paraCount) + " " + p.encode);
                } else {
                    jOption.emplace_back(p.type + " " + item.name);
                    decodeOption.emplace_back(item.name + " " + p.decode);
                    encodeOption.emplace_back(item.name + " " + p.encode);
                }
            }
            jOptions.emplace_back(jOption);
            decodeOptions.emplace_back(decodeOption);
            encodeOptions.emplace_back(decodeOption);
        }
        for (int j = 0; j < i; ++j) {
            jParameters.emplace_back(new std::vector<std::string>());
            decodeParameters.emplace_back(new std::vector<std::string>());
            encodeParameters.emplace_back(new std::vector<std::string>());
        }
        generateWrap(jOptions, jParameters);
        generateWrap(decodeOptions, decodeParameters);
        generateWrap(encodeOptions, encodeParameters);

        std::vector<FunctionWrapperInfo> wrappers;
        if (declaration.ret.type.kind == CXType_Void) {
            FunctionWrapperInfo info;
            info.wrapperName = declaration.function.name;
            for (auto j = 0; j < jParameters.size(); ++j) {
                info.jParameters = *jParameters[j];
                info.decodeParameters = *decodeParameters[j];
                info.encodeParameters = *encodeParameters[j];
                //result is void
                wrappers.emplace_back(info);
            }
        } else {
            auto ret = processWrapperCallType(declaration.ret);
            for (auto &item: ret) {
                FunctionWrapperInfo info;
                info.wrapperName = declaration.function.name + "$" + item.type;
                for (auto j = 0; j < jParameters.size(); ++j) {
                    info.jParameters = *jParameters[j];
                    info.decodeParameters = *decodeParameters[j];
                    info.encodeParameters = *encodeParameters[j];
                    info.wrappedResult = item.type;
                    wrappers.emplace_back(info);
                }
            }
        }
        for (const auto &item: jParameters) {
            delete item;
        }
        for (const auto &item: decodeParameters) {
            delete item;
        }
        for (const auto &item: encodeParameters) {
            delete item;
        }
        return wrappers;
    }
} // jbindgen