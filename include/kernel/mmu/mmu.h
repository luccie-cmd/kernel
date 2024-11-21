/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_MMU_MMU_H_)
#define _KERNEL_MMU_MMU_H_
#define GIGABYTE (1024 * 1024 * 1024ULL)
#define MEGABYTE (1024 * 1024ULL)
#define KILOBYTE (1024ULL)
#define PAGE_SIZE 0x1000

#include "heap/heap.h"
#include "pmm/pmm.h"
#include "vmm/vmm.h"

#endif // _KERNEL_MMU_MMU_H_
