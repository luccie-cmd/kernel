#include <cctype>

extern "C" int toupper(int c)
{
    return (c <= 'z' && c >= 'a' ? c - 'a' + 'A' : c);
}