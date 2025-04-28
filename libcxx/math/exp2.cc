#ifdef __SSE__
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
#endif