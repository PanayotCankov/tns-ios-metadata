//
//  Visit.h
//  TNSBinder
//
//  Created by Panayot Cankov on 12/23/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#ifndef TNSBinder_VIsit_h
#define TNSBinder_VIsit_h

#include <iostream>
#include <assert.h>


#include "Parser.h"

#include "Types.h"
#include "Declarations.h"

namespace tns {
    class VisitAction {
        const Parser* parser;
        CXIndexAction action;
    public:
        VisitAction(const Parser* parser) : parser(parser) {
            this->action = clang_IndexAction_create(parser->index);
        }

        template <class Visitor>
        void visit(Visitor* visitor) {
            static IndexerCallbacks indexerCallbacks = {
                .indexDeclaration = tns::VisitAction::declaration<Visitor>,
            };
            clang_indexTranslationUnit(action, visitor, &indexerCallbacks, sizeof(indexerCallbacks), CXIndexOpt_SuppressWarnings, this->parser->translationUnit);
        }

        ~VisitAction() {
            clang_IndexAction_dispose(this->action);
        }

    private:
        template <class Visitor>
        static void declaration(CXClientData data, const CXIdxDeclInfo *declaration) {
            Visitor* visitor = ((Visitor*)data);

            switch (declaration->cursor.kind) {
                case CXCursor_UnexposedDecl: // 1
                    //std::cout << "TODO: " << declaration->cursor.kind;
                    return;
                case CXCursor_StructDecl: // 2
                    visitor->visitStruct(StructDecl(declaration));
                    return;
                case CXCursor_UnionDecl: // 3
                    visitor->visitUnion(UnionDecl(declaration));
                    return;
                case CXCursor_EnumDecl: // 5
                    visitor->visitEnum(EnumDecl(declaration));
                    return;
                case CXCursor_FieldDecl: // 6
                    visitor->visitField(FieldDecl(declaration));
                    return;
                case CXCursor_EnumConstantDecl: // 7
                    visitor->visitEnumConstant(EnumConstantDecl(declaration));
                    return;
                case CXCursor_FunctionDecl: // 8
                    visitor->visitFunction(FunctionDecl(declaration));
                    return;
                case CXCursor_VarDecl: // 9
                    visitor->visitVar(VarDecl(declaration));
                    return;
                case CXCursor_ObjCInterfaceDecl: // 11
                    visitor->visitInterface(InterfaceDecl(declaration));
                    return;
                case CXCursor_ObjCCategoryDecl: // 12
                    visitor->visitCategory(CategoryDecl(declaration));
                    return;
                case CXCursor_ObjCProtocolDecl: // 13
                    visitor->visitProtocol(ProtocolDecl(declaration));
                    return;
                case CXCursor_ObjCPropertyDecl: // 14
                    visitor->visitProperty(PropertyDecl(declaration));
                    return;
                case CXCursor_ObjCIvarDecl: // 15
                    // std::cout << "TODO: " << declaration->cursor.kind;
                    return;
                case CXCursor_ObjCInstanceMethodDecl: // 16
                    visitor->visitMethod(MethodDecl(true, declaration));
                    return;
                case CXCursor_ObjCClassMethodDecl: // 17
                    visitor->visitMethod(MethodDecl(false, declaration));
                    return;
                case CXCursor_TypedefDecl: // 20
                case CXCursor_ObjCProtocolRef: // 41
                case CXCursor_ObjCClassRef: // 42
                default:
                    // std::cout << "TODO: " << declaration->cursor.kind;
                    return;
            }
        }
    };
}



#endif
