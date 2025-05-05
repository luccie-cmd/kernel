/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#include <cstdio>
#include <thirdparty/stb_sprintf.h>

extern "C" int vsnprintf(char* buffer, size_t bufsz, const char* format, va_list vlist)
{
    return stbsp_vsnprintf(buffer, bufsz, format, vlist);
}