//=======================================================================
// Copyright Baptiste Wicht 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <print.hpp>
#include <system.hpp>

volatile uint64_t current = 45;

uint64_t fibonacci_slow(uint64_t s){
    if(s == 1 || s == 2){
        return current;
    }

    return fibonacci_slow(s - 1) + fibonacci_slow(s - 2);
}

int main(){
    uint64_t i = 0;

    while(i < 10){
        print_line(fibonacci_slow(current));
        ++i;
    }

    exit(0);
    return 0;
}