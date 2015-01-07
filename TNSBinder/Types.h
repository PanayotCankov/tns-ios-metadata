//
//  Types.h
//  TNSBinder
//
//  Created by Panayot Cankov on 12/30/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#ifndef TNSBinder_Types_h
#define TNSBinder_Types_h

#include "clang-c/Index.h"

#include "Values.h"

namespace tns {
    
    enum ObjCBuiltInType : unsigned char {
        Void = 0,
        Bool = 1,
        Char_U = 2,
        UChar = 3,
        Char16 = 4,
        Char32 = 5,
        UShort = 6,
        UInt = 7,
        ULong = 8,
        ULongLong = 9,
        UInt128 = 10,
        Char_S = 11,
        SChar = 12,
        WChar = 13,
        Short = 14,
        Int = 15,
        Long = 16,
        LongLong = 17,
        Int128 = 18,
        Float = 19,
        Double = 20,
        LongDouble = 21,
        NullPtr = 22,
        Overload = 23,
        Dependent = 24,
        ObjCId = 25,
        ObjCClass = 26,
        ObjCSel = 27
    };

    class Type {
    protected:
        CXType type;
        Type(const CXType& type) : type(type) { }

        friend class Argument;
        friend class ArgDecl;
        friend class EnumDecl;
        friend class EnumConstantDecl;
        friend class VarDecl;
        friend class FunctionDecl;
        friend class MethodDecl;
        friend class CallSignatureType;
        friend class ArgsTypeIterator;
        friend class TypeDefType;
        friend class Field;
        friend class FieldDecl;
        friend class PropertyDecl;
        friend class FunctionProtoType;
        friend class PointerType;
        friend class ObjCObjectPointer;
        friend class ConstantArrayType;
        friend class VectorType;
        friend class IncompleteArrayType;
        friend class UnexposedType;

    public:
        const tns::String spelling() const { return String(clang_getTypeSpelling(this->type)); }

        template <class T>
        void visit(T& visitor) const;

        bool operator==(const Type& other) { return clang_equalTypes(this->type, other.type); }
        bool operator!=(const Type& other) { return !operator==(other); }
    };

    class ArgsTypeIterator {
        CXType cxType;
        int index;
        Type type;
        ArgsTypeIterator(const CXType& cxType, int index) : cxType(cxType), index(index), type(clang_getArgType(cxType, index)) { }

        friend class ArgsType;

    public:
        ArgsTypeIterator& operator++() {
            ++this->index;
            this->type = Type(clang_getArgType(this->cxType, this->index));
            return *this;
        }

        ArgsTypeIterator operator++(int) {
            ArgsTypeIterator tmp(*this);
            operator++();
            return tmp;
        }

        friend bool operator==(const ArgsTypeIterator& lhs, const ArgsTypeIterator& rhs) {
            return lhs.index == rhs.index;
        }

        friend bool operator!=(const ArgsTypeIterator& lhs, const ArgsTypeIterator& rhs) {
            return lhs.index != rhs.index;
        }

        const Type* operator->() const {
            return &this->type;
        }

        const Type& operator*() const {
            return this->type;
        }
    };

    class ArgsType {
        CXType type;
        ArgsType(const CXType& type) : type(type) { }

        friend class CallSignatureType;
        friend class FunctionProtoType;
    public:
        const int length() const { return clang_getNumArgTypes(this->type); }
        ArgsTypeIterator begin() const { return ArgsTypeIterator(this->type, 0); }
        ArgsTypeIterator end() const { return ArgsTypeIterator(this->type, clang_getNumArgTypes(this->type)); }
    };

    class CallSignatureType : Type {
    protected:
        CallSignatureType(CXType type) : Type(type) { }
        friend class tns::Type;
        friend class BlockPointerType;
    public:
        const Type returnType() const { return Type(clang_getResultType(this->type)); }
        const ArgsType arguments() const { return ArgsType(this->type); }
    };

    // 1
    class UnexposedType : public Type {
        UnexposedType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        const Type canonicalType() const { return Type(clang_getCanonicalType(this->type)); }
    };

    // 101
    class PointerType : public Type {
        PointerType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        const Type pointee() const { return Type(clang_getPointeeType(this->type)); }
    };

    // 102
    class BlockPointerType : public Type {
        BlockPointerType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        const CallSignatureType pointee() const { return CallSignatureType(clang_getPointeeType(this->type)); }
    };

    // 105
    class RecordType : public Type {
        RecordType(CXType type) : Type(type) { }
        friend class Type;
    public:
        const String usr() const { return String(clang_getCursorUSR(clang_getTypeDeclaration(this->type))); }
    };

    // 106
    class EnumType : public Type {
        EnumType(CXType type) : Type(type) { }
        friend class tns::Type;
    };

    // 108
    class ObjCInterfaceType : public Type {
        ObjCInterfaceType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        const String usr() const { return String(clang_getCursorUSR(clang_getTypeDeclaration(this->type))); }
    };

    // 107
    class TypeDefType : public Type {
        TypeDefType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        const Type canonicalType() const { return Type(clang_getCanonicalType(this->type)); }
    };

    // 109
    class ObjCObjectPointer : public Type {
        ObjCObjectPointer(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        Type pointee() const { return Type(clang_getPointeeType(this->type)); }
    };

    // 111
    class FunctionProtoType : public CallSignatureType {
        FunctionProtoType(CXType type) : CallSignatureType(type) { }
        friend class tns::Type;
    };

    // 112
    class ConstantArrayType : public Type {
        ConstantArrayType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        long long length() const { return clang_getNumElements(this->type); }
        const Type elementType() const { return Type(clang_getElementType(this->type)); }
    };

    // 113
    class VectorType : public Type {
        VectorType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        long long length() const { return clang_getNumElements(this->type); }
        const Type elementType() const { return Type(clang_getElementType(this->type)); }
    };

    // 114
    class IncompleteArrayType : public Type {
        IncompleteArrayType(CXType type) : Type(type) { }
        friend class tns::Type;
    public:
        const Type elementType() const { return Type(clang_getElementType(this->type)); }
    };
}

#include "clang-c/Index.h"
template <class T>
void tns::Type::visit(T& visitor) const {
    if (this->type.kind >= CXType_FirstBuiltin && this->type.kind <= CXType_LastBuiltin) {
        visitor.visitBuiltIn((ObjCBuiltInType)(type.kind - 2));
        return;
    }

    switch(this->type.kind) {
        case CXType_Invalid: // 0
            visitor.visitInvalid();
            return;
        case CXType_Unexposed: // 1
            visitor.visitUnexposed(UnexposedType(this->type));
            return;
        case CXType_Complex: // 100
            std::cout << " [TODO " << this->type.kind << "] ";
            return;
        case CXType_Pointer: // 101
            visitor.visitPointer(PointerType(type));
            return;
        case CXType_BlockPointer: // 102
            visitor.visitBlockPointer(BlockPointerType(type));
            return;
        case CXType_LValueReference: // 103
        case CXType_RValueReference: // 104
            std::cout << " [TODO " << this->type.kind << "] ";
            return;
        case CXType_Record: // 105
            visitor.visitRecord(RecordType(this->type));
            return;
        case CXType_Enum: // 106
            visitor.visitEnum(EnumType(this->type));
            return;
        case CXType_Typedef: // 107
            visitor.visitTypeDef(TypeDefType(this->type));
            return;
        case CXType_ObjCInterface: // 108
            visitor.visitObjCInterface(ObjCInterfaceType(this->type));
            return;
        case CXType_ObjCObjectPointer: // 109
            visitor.visitObjCObjectPointer(ObjCObjectPointer(this->type));
            return;
        case CXType_FunctionNoProto: // 110
            std::cout << " [TODO " << this->type.kind << "] ";
            return;
        case CXType_FunctionProto: // 111
            visitor.visitFunctionProto(FunctionProtoType(this->type));
            break;
        case CXType_ConstantArray: // 112
            visitor.visitConstantArray(ConstantArrayType(this->type));
            return;
        case CXType_Vector: // 113
            visitor.visitVector(VectorType(this->type));
            return;
        case CXType_IncompleteArray: // 114
            visitor.visitIncompleteArray(IncompleteArrayType(this->type));
            return;
        case CXType_VariableArray: // 115
        case CXType_DependentSizedArray: // 116
        case CXType_MemberPointer: // 117
            std::cout << " [TODO " << this->type.kind << "] ";
            return;
        default:
            std::cout << " [TODO " << this->type.kind << "] ";
            return;
    }
}

#endif
