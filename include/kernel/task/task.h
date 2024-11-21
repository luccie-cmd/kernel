#if !defined(_KERNEL_TASK_TASK_H_)
#define _KERNEL_TASK_TASK_H_
#include "types.h"

namespace task{
    void initialize();
    bool isInitialized();
    pid_t getCurrentPID();
};

#endif // _KERNEL_TASK_TASK_H_
