#if !defined(_COMMON_SPINLOCK_H_)
#define _COMMON_SPINLOCK_H_
#include <cstdint>

namespace std{
    class Spinlock{
        public:
            Spinlock() {
                state = 0;
            }
            ~Spinlock() {}
            void lock() {
                uint8_t tmp;
                do {
                    tmp = 1;
                    __asm__("lock xchg %0, %1"
                            : "+r"(tmp), "+m"(this->state)
                            : : "memory");
                } while (tmp != 0);
            }
            void unlock() {
                this->state = 0;
            }
        private:
            uint8_t state;
    };
};

#endif // _COMMON_SPINLOCK_H_
