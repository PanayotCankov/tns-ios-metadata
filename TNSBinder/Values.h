//
//  Values.h
//  TNSBinder
//
//  Created by Panayot Cankov on 12/30/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#ifndef TNSBinder_Values_h
#define TNSBinder_Values_h

#include "clang-c/Index.h"
#include <iostream>

namespace tns {
    class String {
        CXString string;
        const char* cStr;

        friend class Decl;
        friend class Struct;
        friend class Union;
        friend class Enum;
        friend class Field;
        friend class Interface;
        friend class Method;
        friend class Type;
        friend class ArgDecl;
        friend class DeclRefType;
        friend class RecordType;
        friend class ObjCInterfaceType;

        String() = delete;
        String(const CXString& string) : string(string), cStr(clang_getCString(string)) { }

    public:
        String(const String& s) : String(s.string) { }
        String& operator= (const String& s) {
            this->string = s.string;
            this->cStr = clang_getCString(this->string);
            return *this;
        }
        String(String&& s) : string(s.string), cStr(s.cStr) {
            s.cStr = NULL;
        }

        const char* cString() const { return this->cStr; }
        
        ~String() {
            if (this->cStr) {
                clang_disposeString(string);
            }
        }
    };

    std::ostream& operator<< (std::ostream& ostr, const tns::String& str) {
        ostr << str.cString();
        return ostr;
    }
}

#endif
