/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_TASK_TYPES_H_)
#define _KERNEL_TASK_TYPES_H_
#include <cstddef>
#define KERNEL_PID 0
#define   INIT_PID 1
#define USER_STACK_TOP 0x00007FFFFFFFF000ULL
#define ONDEMAND_MAP_ADDRESS 0xDEADC000

namespace task{
    using pid_t = size_t;
};

#endif // _KERNEL_TASK_TYPES_H_
