//
//  Definitions.h
//  TNSBinder
//
//  Created by Panayot Cankov on 12/30/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#ifndef TNSBinder_Declarations_h
#define TNSBinder_Declarations_h

#include <iterator>

#include "clang-c/Index.h"

#include "Values.h"
#include "Types.h"

namespace tns {

    class Decl {
    protected:
        const CXIdxDeclInfo* declaration;
        Decl(const CXIdxDeclInfo* declaration) : declaration(declaration) { }
    public:
        const char* name() const { return this->declaration->entityInfo->name; }
        const String usr() const { return String(clang_getCursorUSR(this->declaration->cursor)); }
        const String containerUsr() const { return String(clang_getCursorUSR(this->declaration->semanticContainer->cursor)); }
        const String module() const {
            CXSourceLocation location = clang_getCursorLocation(this->declaration->cursor);
            CXFile file;
            unsigned l, c, o;
            clang_getFileLocation(location, &file, &l, &c, &o);
            CXTranslationUnit translationUnit = clang_Cursor_getTranslationUnit(this->declaration->cursor);
            CXModule module = clang_getModuleForFile(translationUnit, file);
            CXString name = clang_Module_getFullName(module);
            return String(name);
        }
    };

    class StructDecl : public Decl {
        StructDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    };

    class UnionDecl : public Decl {
        UnionDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    };

    class EnumDecl : public Decl {
        EnumDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
        const Type type() const { return clang_getEnumDeclIntegerType(this->declaration->cursor); }
    };

    class FieldDecl : public Decl {
        FieldDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
        const Type type() const { return Type(clang_getCursorType(this->declaration->cursor)); }
    };

    class EnumConstantDecl : public Decl {
        EnumConstantDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
        const Type type() const { return clang_getCursorType(this->declaration->cursor); }
        const long long value() const { return clang_getEnumConstantDeclValue(this->declaration->cursor); }
        const unsigned long long unsignedValue() const { return clang_getEnumConstantDeclUnsignedValue(this->declaration->cursor); }
    };

    class ArgDecl {
        CXCursor cursor;
        ArgDecl(const CXCursor cursor) : cursor(cursor) { }
        friend class ArgDeclIterator;
    public:
        const Type type() { return Type(clang_getCursorType(this->cursor)); }
        const String name() { return String(clang_getCursorDisplayName(this->cursor)); }
    };

    class ArgDeclIterator : public std::iterator<std::input_iterator_tag, ArgDecl> {
        CXCursor cursor;
        unsigned int position;
        ArgDecl argument;
        ArgDeclIterator(CXCursor cursor, unsigned int position) : cursor(cursor), position(position), argument(clang_Cursor_getArgument(cursor, position)) { }
        friend class ArgsDecl;

    public:

        ArgDeclIterator& operator++() {
            ++this->position;
            this->argument = ArgDecl(clang_Cursor_getArgument(cursor, position));
            return *this;
        }

        ArgDeclIterator operator++(int) {
            ArgDeclIterator tmp(*this);
            operator++();
            return tmp;
        }

        friend bool operator==(const ArgDeclIterator& lhs, const ArgDeclIterator& rhs) {
            return lhs.position == rhs.position;
        }

        friend bool operator!=(const ArgDeclIterator& lhs, const ArgDeclIterator& rhs) {
            return lhs.position != rhs.position;
        }

        const ArgDecl* operator->() const {
            return &this->argument;
        }

        const ArgDecl& operator*() const {
            return this->argument;
        }
    };

    class ArgsDecl {
        CXCursor cursor;
        ArgsDecl(const CXCursor& cursor) : cursor(cursor) { }
        friend class MethodDecl;
        friend class FunctionDecl;
    public:
        unsigned int count() const { return clang_Cursor_getNumArguments(this->cursor); }
        ArgDeclIterator begin() const { return ArgDeclIterator(this->cursor, 0); }
        ArgDeclIterator end() const { return ArgDeclIterator(this->cursor, this->count()); };
    };

    class FunctionDecl : public Decl {
        FunctionDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
        const ArgsDecl arguments() const { return ArgsDecl(this->declaration->cursor); }
        const Type returnType() const { return Type(clang_getCursorResultType(this->declaration->cursor)); }
    };

    class VarDecl : public Decl {
        VarDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
        const Type type() const { return clang_getCursorType(this->declaration->cursor); }
    };

    class MethodDecl : public Decl {
        bool instance;
        MethodDecl(bool instance, const CXIdxDeclInfo* declaration) : instance(instance), Decl(declaration) { }
        friend class VisitAction;
    public:
        bool isInstance() const { return this->instance; }
        const ArgsDecl arguments() const { return ArgsDecl(this->declaration->cursor); }
        const Type returnType() const { return Type(clang_getCursorResultType(this->declaration->cursor)); }
    };

    class Protocol {
        const CXIdxObjCProtocolRefInfo* protocol;
        Protocol(const CXIdxObjCProtocolRefInfo* protocol) : protocol(protocol) { }
        friend class ProtocolIterator;
    public:
        const char* name() const { return this->protocol->protocol->name; }
    };

    class ProtocolIterator : public std::iterator<std::input_iterator_tag, Protocol> {
        const CXIdxObjCProtocolRefInfo *const *protocols;
        Protocol protocol;
        ProtocolIterator(const CXIdxObjCProtocolRefInfo *const *protocols) : protocols(protocols), protocol(*protocols) { }
        friend class Protocols;
    public:

        ProtocolIterator& operator++() {
            ++this->protocols;
            this->protocol = Protocol(*this->protocols);
            return *this;
        }

        ProtocolIterator operator++(int) {
            ProtocolIterator tmp(*this);
            operator++();
            return tmp;
        }

        friend bool operator==(const ProtocolIterator& lhs, const ProtocolIterator& rhs) {
            return lhs.protocols == rhs.protocols;
        }

        friend bool operator!=(const ProtocolIterator& lhs, const ProtocolIterator& rhs) {
            return lhs.protocols != rhs.protocols;
        }

        const Protocol* operator->() const {
            return &this->protocol;
        }

        const Protocol& operator*() const {
            return this->protocol;
        }
    };

    class Protocols {
        const CXIdxObjCProtocolRefListInfo* protocols;
        Protocols(const CXIdxObjCProtocolRefListInfo* protocols) : protocols(protocols) { }
        friend class InterfaceDecl;
    public:
        ProtocolIterator begin() const { return ProtocolIterator(protocols->protocols); }
        ProtocolIterator end() const { return ProtocolIterator(protocols->protocols + protocols->numProtocols); };
    };

    class InterfaceDecl : public Decl {
        const CXIdxObjCInterfaceDeclInfo* objCInterfaceDeclInfo;
        InterfaceDecl(const CXIdxDeclInfo* declaration) : Decl(declaration), objCInterfaceDeclInfo(clang_index_getObjCInterfaceDeclInfo(declaration)) { }
        friend class VisitAction;
    public:
        bool hasSuper() const { return this->objCInterfaceDeclInfo->superInfo; }
        const char* superName() const { return this->objCInterfaceDeclInfo->superInfo->base->name; }
        Protocols protocols() const { return Protocols(this->objCInterfaceDeclInfo->protocols); }
    };

    class CategoryDecl : public Decl {
        CategoryDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
    };

    class ProtocolDecl : public Decl {
        ProtocolDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
    };

    class PropertyDecl : public Decl {
        PropertyDecl(const CXIdxDeclInfo* declaration) : Decl(declaration) { }
        friend class VisitAction;
    public:
        const Type type() const { return Type(clang_getCursorType(this->declaration->cursor)); }
    };
}

#endif
