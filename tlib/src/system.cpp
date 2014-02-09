//=======================================================================
// Copyright Baptiste Wicht 2013-2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

#include <system.hpp>

void exit(size_t return_code) {
    asm volatile("mov rax, 0x666; int 50"
        : //No outputs
        : "b" (return_code)
        : "rax");

    __builtin_unreachable();
}

int64_t exec(const char* executable) {
    int64_t pid;
    asm volatile("mov rax, 5; int 50; mov %0, rax"
        : "=m" (pid)
        : "b" (reinterpret_cast<size_t>(executable))
        : "rax");
    return pid;
}

void await_termination(size_t pid) {
    asm volatile("mov rax, 6; int 50;"
        : //No outputs
        : "b" (pid)
        : "rax");
}

int64_t exec_and_wait(const char* executable){
    int64_t pid = exec(executable);

    if(pid < 0){
        return pid;
    } else {
        await_termination(pid);
        return 0;
    }
}

void sleep_ms(size_t ms){
    asm volatile("mov rax, 4; int 50"
        : //No outputs
        : "b" (ms)
        : "rax");
}