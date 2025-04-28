# 0 "libcxx/math/exp2.cc"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "libcxx/math/exp2.cc"

extern "C" double exp2(double x) {
    if (x == 0) return 1.0;
    double result = 1.0;
    double base = 2.0;
    while (x != 0) {
        if (static_cast<int>(x) % 2 != 0) {
            result *= base;
        }
        base *= base;
        x /= 2;
    }
    return result;
}
