//
//  Stopwatch.h
//  TNSBinder
//
//  Created by Panayot Cankov on 12/23/14.
//  Copyright (c) 2014 Telerik A D. All rights reserved.
//

#ifndef __TNSBinder__Stopwatch__
#define __TNSBinder__Stopwatch__

#include <iostream>
#include <chrono>

namespace tns {
    class Stopwatch {
        std::chrono::high_resolution_clock::time_point start;
    public:
        Stopwatch() {
            this->start = std::chrono::high_resolution_clock::now();
        }

        friend std::ostream& operator<<(std::ostream& stream, const tns::Stopwatch stopwatch);
    };

    std::ostream& operator<<(std::ostream& stream, const tns::Stopwatch stopwatch) {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - stopwatch.start);
        stream << duration.count();
        return stream;
    }
}

#endif /* defined(__TNSBinder__Stopwatch__) */
