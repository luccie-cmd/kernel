/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <cstring>
#include <cstdint>
#include <common/dbg/dbg.h>

extern "C" void* memset(void* dest, int ch, std::size_t count){
    dbg::addTrace(__PRETTY_FUNCTION__);
    uint8_t cch = (uint8_t)ch;
    uint8_t* cdest = (uint8_t*)dest;
    for(size_t i = 0; i < count; ++i){
        cdest[i] = cch;
    }
    dbg::popTrace();
    return cdest;
}