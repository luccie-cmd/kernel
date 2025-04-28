#include <cmath>

#ifdef __SSE__
double pow(double base, double exponent){
    if (exponent == 0) {
        return 1.0;
    }
    double result = 1.0;
    bool isNegativeExponent = (exponent < 0);
    if (isNegativeExponent) {
        exponent = -exponent;
    }
    while (exponent > 0) {
        if ((int)exponent % 2 == 1) {
            result *= base;
        }
        base *= base;
        exponent /= 2;
    }
    if (isNegativeExponent) {
        result = 1.0 / result;
    }
    return result;
}
#endif