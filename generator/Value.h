//
// Created by snownf on 23-11-9.
//

#ifndef JAVABINDGEN_VALUE_H
#define JAVABINDGEN_VALUE_H

#include <string>
#include <sstream>
#include "../analyser/StructDeclaration.h"

#define NativeArray std::string("NativeArray")
#define NativePointer std::string("NativePointer")
#define NativeValue std::string("NativeValue")


namespace jbindgen::value {
    enum basic_j_type {
        j_int,
        j_long,
        j_float,
        j_double,
        j_char,
        j_bool,
        j_byte,
        j_void,
        memory_segment,
        type_other
    };

    struct FFMType {
        enum basic_j_type type;
        std::string primitive;
        std::string value_layout;
        std::string native_wrapper;
    };
    const FFMType Integer{j_int, "int", "JAVA_INT", "JInt"};
    const FFMType Long{j_long, "long", "JAVA_LONG", "JLong"};
    const FFMType Double{j_double, "double", "JAVA_DOUBLE", "JDouble"};
    const FFMType Float{j_float, "float", "JAVA_FLOAT", "JFloat"};
    const FFMType Char{j_char, "char", "JAVA_CHAR", "JChar"};
    const FFMType Byte{j_byte, "byte", "JAVA_BYTE", "JByte"};
    const FFMType Bool{j_bool, "boolean", "JAVA_BOOLEAN", "JBoolean"};
    const FFMType MemorySegment{memory_segment, "MemorySegment", "ADDRESS", "###"};
    const FFMType Void{j_void, "void", "###", "###"};
    const FFMType Not{type_other, "###", "###", "###"};

    enum decode_method {
        decode_by_value_call = 1,
        decode_by_primitive,
        decode_by_pointer_call,
        decode_error = -1
    };
    enum copy_method {
        copy_by_set_j_int_call = 1,
        copy_by_set_j_long_call,
        copy_by_set_j_float_call,
        copy_by_set_j_double_call,
        copy_by_set_j_char_call,
        copy_by_set_j_byte_call,
        copy_by_set_j_bool_call,
        copy_by_set_memory_segment_call,
        copy_by_value_j_int_call,
        copy_by_value_j_long_call,
        copy_by_value_j_float_call,
        copy_by_value_j_double_call,
        copy_by_value_j_char_call,
        copy_by_value_j_byte_call,
        copy_by_value_j_bool_call,
        copy_by_value_memory_segment_call,
        copy_by_ptr_dest_copy_call,
        copy_error = INT32_MIN,
        copy_void,
        copy_internal_function_proto,
    };

    FFMType copy_method_2_ffm_type(enum copy_method copyMethod);

    enum encode_method {
        encode_by_get_j_int_call = 1,
        encode_by_get_j_long_call,
        encode_by_get_j_float_call,
        encode_by_get_j_double_call,
        encode_by_get_j_char_call,
        encode_by_get_j_byte_call,
        encode_by_get_j_bool_call,
        encode_by_get_memory_segment_call,
        encode_by_object_slice,//memorysegment.asSlice(,)
        encode_by_object_ptr,//memorysegment.get(ValueLayout.ADDRESS,)
        encode_by_array_ptr,
        encode_error = INT32_MIN,
        encode_by_void,//function return void
        encode_internal_function_proto,
    };

    FFMType encode_method_2_ffm_type(enum encode_method encodeMethod);

    enum decode_method typeDecode(const CXType &declare, const CXCursor &cursor);

    enum copy_method typeCopy(const CXType &declare, const CXCursor &cursor);

    enum encode_method typeEncode(const CXType &declare);
}
// jbindgen

#endif //JAVABINDGEN_VALUE_H
