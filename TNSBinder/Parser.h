//
//  Parser.h
//  TNSBinder
//
//  Created by Panayot Cankov on 12/23/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#ifndef __TNSBinder__Parser__
#define __TNSBinder__Parser__

#include "clang-c/Index.h"

namespace tns {
    class Parser {
        CXIndex index;
        CXTranslationUnit translationUnit;

        friend class VisitAction;
        
    public:
        Parser(const char *sourceFile, const char *const *args, int numArgs) {
            this->index = clang_createIndex(1, 1);

            if (!this->index) {
                // throw
            }

            this->translationUnit = clang_parseTranslationUnit(index, sourceFile, args, numArgs, NULL, 0, CXTranslationUnit_None);

            if (!this->translationUnit) {
                // throw
            }
        }

        ~Parser() {
            clang_disposeIndex(this->index);
            clang_disposeTranslationUnit(this->translationUnit);
        }
    };
}

#endif /* defined(__TNSBinder__Parser__) */
