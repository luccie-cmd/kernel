/*
 * Copyright (c) - All Rights Reserved.
 *
 * See the LICENSE file for more information.
 */

#if !defined(_KERNEL_MMU_VMM_TYPES_H_)
#define _KERNEL_MMU_VMM_TYPES_H_
#include <cstdint>

namespace mmu::vmm{
    struct PML4{
        uint64_t present    : 1; // Is the page present?
        uint64_t rw         : 1; // Can read/write to/from the page?
        uint64_t user       : 1; // Can user access this page?
        uint64_t pwt        : 1; // Page write through
        uint64_t pcd        : 1; // Page cache disable
        uint64_t accesed    : 1; // Is page accessed?
        uint64_t ignored    : 1; // Ignored field.
        uint64_t mbz        : 2; // Must be zero.
        uint64_t ats0       : 3; // Available to software.
        uint64_t pdpe_ptr   : 40; // Physical page number to the PDP tables.
        uint64_t ats1       : 11; // Available to the software.
        uint64_t no_execute : 1;  // Disable execution of code on this page.
    } __attribute__((packed));
    static_assert(sizeof(PML4) == 8, "Structure PML4 isn't 8 bytes big.");

    struct PDPE{
        uint64_t present    : 1; // Is the page present?
        uint64_t rw         : 1; // Can read/write to/from the page?
        uint64_t user       : 1; // Can user access this page?
        uint64_t pwt        : 1; // Page write through
        uint64_t pcd        : 1; // Page cache disable
        uint64_t accesed    : 1; // Is page accessed?
        uint64_t ignored    : 1; // Ignored field.
        uint64_t mbz        : 1; // Must be zero.
        uint64_t ignored2   : 1; // Ignored field.
        uint64_t ats0       : 3; // Available to software.
        uint64_t pde_ptr    : 40; // Physical page number to the PD tables.
        uint64_t ats1       : 11; // Available to the software.
        uint64_t no_execute : 1;  // Disable execution of code on this page.
    } __attribute__((packed));
    static_assert(sizeof(PDPE) == 8, "Structure PDP isn't 8 bytes big.");

    struct PDE{
        uint64_t present    : 1; // Is the page present?
        uint64_t rw         : 1; // Can read/write to/from the page?
        uint64_t user       : 1; // Can user access this page?
        uint64_t pwt        : 1; // Page write through
        uint64_t pcd        : 1; // Page cache disable
        uint64_t accesed    : 1; // Is page accessed?
        uint64_t ignored    : 1; // Ignored field.
        uint64_t mbz        : 1; // Must be zero.
        uint64_t ignored2   : 1; // Ignored field.
        uint64_t ats0       : 3; // Available to software.
        uint64_t pte_ptr    : 40; // Physical page number to the PT tables.
        uint64_t ats1       : 11; // Available to the software.
        uint64_t no_execute : 1;  // Disable execution of code on this page.
    } __attribute__((packed));
    static_assert(sizeof(PDE) == 8, "Structure PD isn't 8 bytes big.");

    struct PTE{
        uint64_t present    : 1; // Is the page present?
        uint64_t rw         : 1; // Can read/write to/from the page?
        uint64_t user       : 1; // Can user access this page?
        uint64_t pwt        : 1; // Page write through
        uint64_t pcd        : 1; // Page cache disable
        uint64_t accesed    : 1; // Is page accessed?
        uint64_t dirty      : 1; // Was the field written to?
        uint64_t pat        : 1; // Page attribute table.
        uint64_t global     : 1; // Is page global (unvalidated)
        uint64_t ats0       : 3; // Available to software.
        uint64_t papn_ppn   : 40; // Physical page number to the physical address.
        uint64_t ats1       : 7;  // Available to the software.
        uint64_t pkeys      : 4;  // Protection keys
        uint64_t no_execute : 1;  // Disable execution of code on this page.
    } __attribute__((packed));
    static_assert(sizeof(PTE) == 8, "Structure PT isn't 8 bytes big.");
};

#endif // _KERNEL_MMU_VMM_TYPES_H_
