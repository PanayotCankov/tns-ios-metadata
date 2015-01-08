//
//  main.cpp
//  TNSBinder
//
//  Created by Panayot Cankov on 12/22/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#include <iostream>

#include <stdio.h>
#include "string.h"
#include "clang-c/Index.h"

#include "Stopwatch.h"
#include "Parser.h"
#include "VisitAction.h"

class ObjCDeclarationVisitor {

    void outputProtocols(const tns::Protocols protocols) {
        if (protocols.begin() != protocols.end()) {
            std::cout << " <";
            for(auto protocol : protocols) {
                std::cout << protocol.name() << ", ";
            }
            std::cout << ">";
        }
    }

public:

    // Types
    void visitInvalid() {
    }

    void visitUnexposed(tns::UnexposedType unexposed) {
        if (unexposed == unexposed.canonicalType()) {
            tns::String spelling = unexposed.spelling();
            if (!strncmp(spelling.cString(), "id", 2)) {
                // TODO: ...Interface...<...protocols list...> is here too...
                std::cout << "id";
            } else {
                std::cout << "?" << unexposed.spelling() << ";";
            }
        } else {
            unexposed.canonicalType().visit(*this);
        }
    }

    void visitBuiltIn(tns::ObjCBuiltInType type) {
        static const char * primitivemap[] = {
            "void",
            "Bool",
            "Char_U",
            "UChar",
            "Char16",
            "Char32",
            "UShort",
            "UInt",
            "ULong",
            "ULongLong",
            "UInt128",
            "Char_S",
            "SChar",
            "WChar",
            "Short",
            "Int",
            "Long",
            "LongLong",
            "Int128",
            "Float",
            "Double",
            "LongDouble",
            "NullPtr",
            "Overload",
            "Dependent",
            "id",
            "ObjCClass",
            "SEL"
        };

        std::cout << primitivemap[type];
    }

    void visitPointer(const tns::PointerType pointer) {
        pointer.pointee().visit(*this);
        std::cout << "*";
    }

    void visitBlockPointer(const tns::BlockPointerType type) {
        type.pointee().returnType().visit(*this);
        std::cout << " (^) ";
        for(auto argType : type.pointee().arguments()) {
            argType.visit(*this);
            std::cout << ", ";
        }
    }

    void visitRecord(const tns::RecordType record) {
        std::cout << "{" << record.usr() << "}";
    }

    void visitEnum(const tns::EnumType& e) {
        std::cout << "enum";
    }

    void visitTypeDef(const tns::TypeDefType typeDef) {
        typeDef.canonicalType().visit(*this);
    }

    void visitObjCObjectPointer(const tns::ObjCObjectPointer objCPointer) {
        objCPointer.pointee().visit(*this);
        std::cout << "*";
    }

    void visitObjCInterface(const tns::ObjCInterfaceType interface) {
        if (!strcmp(interface.spelling().cString(), "Protocol")) {
            std::cout << "Protocol";
        } else {
            std::cout << "@(" << interface.usr() << ")";
        }
    }

    void visitFunctionProto(const tns::FunctionProtoType proto) {
        proto.returnType().visit(*this);
        std::cout << " (";
        for(auto argType : proto.arguments()) {
            argType.visit(*this);
            std::cout << ", ";
        }
        std::cout << ")";
        // TODO: varargs ...
    }

    void visitConstantArray(const tns::ConstantArrayType array) {
        array.elementType().visit(*this);
        std::cout << "[" << array.length() << "]";
    }

    void visitVector(const tns::VectorType& vector) {
        vector.elementType().visit(*this);
        std::cout << "<" << vector.length() << ">";
    }

    void visitIncompleteArray(const tns::IncompleteArrayType incompleteArray) {
        incompleteArray.elementType().visit(*this);
        std::cout << "[]";
    }

    // Declarations
    void visitStruct(const tns::StructDecl s) {
        std::cout << "struct ";
        if (s.name()) {
            std::cout << s.name();
        } else {
            std::cout << "(anonymous)";
        }
        std::cout << std::endl;
    }

    void visitUnion(const tns::UnionDecl u) {
        std::cout << "union ";
        if (u.name()) {
            std::cout << u.name();
        } else {
            std::cout << "(anonymous)";
        }
        std::cout << std::endl;
    }

    void visitEnum(const tns::EnumDecl e) {
        std::cout << "enum ";
        if (e.name()) {
            std::cout << e.name();
        } else {
            std::cout << "(anonymous)";
        }
        std::cout << " : ";
        e.type().visit(*this);
        std::cout << std::endl;
    }

    void visitField(const tns::FieldDecl f) {
        std::cout << "\t ." << f.name();

        std::cout << " : ";
        f.type().visit(*this);

        std::cout << std::endl;
    }

    void visitEnumConstant(const tns::EnumConstantDecl declaration) {
        std::cout << "\t. ";
        declaration.type().visit(*this);
        std::cout << " " << declaration.name() << " = " << declaration.value() << ";" << std::endl;
    }

    void visitFunction(const tns::FunctionDecl function) {
        std::cout << "function ";
        function.returnType().visit(*this);
        std::cout << " " << function.name() << " (";
        for (auto a : function.arguments()) {
            a.type().visit(*this);
            std::cout << " " << a.name() << ", ";
        }
        std::cout << ")" << std::endl;
    }

    void visitVar(const tns::VarDecl var) {
        std::cout << "var ";
        var.type().visit(*this);
        std::cout << " " << var.name() << ";" << std::endl;
    }

    void visitInterface(const tns::InterfaceDecl interface) {
        std::cout << "interface " << interface.name();

        if (interface.hasSuper()) {
            std::cout << " : " << interface.superName();
        }
        outputProtocols(interface.protocols());
        std::cout << " module: " << interface.module();
        std::cout << std::endl;
    }

    void visitCategory(const tns::CategoryDecl category) {
        std::cout << "category " << category.name() << std::endl;
    }

    void visitProtocol(const tns::ProtocolDecl protocol) {
        std::cout << "protocol " << protocol.name() << std::endl;
    }

    void visitProperty(const tns::PropertyDecl property) {
        std::cout << "\t ." << property.name();
        std::cout << " : ";
        property.type().visit(*this);
        std::cout << std::endl;
    }

    void visitMethod(const tns::MethodDecl method) {
        std::cout << "\t" << (method.isInstance() ? "-" : "+") << " ";
        method.returnType().visit(*this);
        std::cout << " ";
        std::cout << method.name() << " ";

        for (auto a : method.arguments()) {
            a.type().visit(*this);
            std::cout << " " << a.name() << ", ";
        }
        
        std::cout << std::endl;
    }
};

const char * args[] = {
    "-v",
    "-x",
    "objective-c",
    "-arch", "armv7",
    //"-arch", "i386",
    "-target", "arm-apple-darwin",
    "-std=gnu99",
    "-miphoneos-version-min=7.0",

    // "-fmodules",

    "-fmodule-maps",
    "-fmodule-map-file=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk/usr/include/module.map",
    "-fmodule-map-file=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk/usr/dispatch/module.map",
    "-I", "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk/usr/include/objc",

    "-isysroot", "/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.1.sdk"
};

int main(int argc, const char * argv[]) {
    const char *sourceFile = "/Users/cankov/GitRoot/tns-ios/Source/Headers/iOS8.0.h";
    // const char *sourceFile = "/Users/cankov/Desktop/test.h";

    tns::Stopwatch stopwatch;
    tns::Parser parser(sourceFile, args, sizeof(args) / sizeof(args[0]));
    tns::VisitAction visitAction(&parser);

    ObjCDeclarationVisitor myVisitor;
    visitAction.visit(&myVisitor);

    std::cout << "Completed in: " << stopwatch << std::endl;

    return 0;
}




