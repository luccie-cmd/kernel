# 0 "kernel/mmu/mmu.cc"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "kernel/mmu/mmu.cc"
# 1 "include/kernel/mmu/mmu.h" 1
# 14 "include/kernel/mmu/mmu.h"
# 1 "include/kernel/mmu/heap/heap.h" 1
# 9 "include/kernel/mmu/heap/heap.h"
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstddef" 1
# 42 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstddef"
       
# 43 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstddef" 3






# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 1 3
# 278 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 3

# 278 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 3
namespace std
{
  typedef long unsigned int size_t;
  typedef long int ptrdiff_t;


  typedef decltype(nullptr) nullptr_t;

}
# 300 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 3
namespace std
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}
namespace __gnu_cxx
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}
# 586 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 3
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/os_defines.h" 1 3
# 39 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/os_defines.h" 3
# 1 "/usr/include/features.h" 1 3 4
# 415 "/usr/include/features.h" 3 4
# 1 "/usr/include/features-time64.h" 1 3 4
# 20 "/usr/include/features-time64.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 21 "/usr/include/features-time64.h" 2 3 4
# 1 "/usr/include/bits/timesize.h" 1 3 4
# 19 "/usr/include/bits/timesize.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 20 "/usr/include/bits/timesize.h" 2 3 4
# 22 "/usr/include/features-time64.h" 2 3 4
# 416 "/usr/include/features.h" 2 3 4
# 524 "/usr/include/features.h" 3 4
# 1 "/usr/include/sys/cdefs.h" 1 3 4
# 730 "/usr/include/sys/cdefs.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 731 "/usr/include/sys/cdefs.h" 2 3 4
# 1 "/usr/include/bits/long-double.h" 1 3 4
# 732 "/usr/include/sys/cdefs.h" 2 3 4
# 525 "/usr/include/features.h" 2 3 4
# 548 "/usr/include/features.h" 3 4
# 1 "/usr/include/gnu/stubs.h" 1 3 4
# 10 "/usr/include/gnu/stubs.h" 3 4
# 1 "/usr/include/gnu/stubs-64.h" 1 3 4
# 11 "/usr/include/gnu/stubs.h" 2 3 4
# 549 "/usr/include/features.h" 2 3 4
# 40 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/os_defines.h" 2 3
# 587 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 2 3


# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/cpu_defines.h" 1 3
# 590 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 2 3
# 777 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 3
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/pstl/pstl_config.h" 1 3
# 778 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/x86_64-pc-linux-gnu/bits/c++config.h" 2 3
# 50 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstddef" 2 3
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h" 1 3
# 143 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h" 3
typedef long int ptrdiff_t;
# 209 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h" 3
typedef long unsigned int size_t;
# 415 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h" 3
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
# 426 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h" 3
} max_align_t;






  typedef decltype(nullptr) nullptr_t;
# 51 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstddef" 2 3

extern "C++"
{

namespace std
{

  using ::max_align_t;
}



namespace std
{




  enum class byte : unsigned char {};

  template<typename _IntegerType> struct __byte_operand { };
  template<> struct __byte_operand<bool> { using __type = byte; };
  template<> struct __byte_operand<char> { using __type = byte; };
  template<> struct __byte_operand<signed char> { using __type = byte; };
  template<> struct __byte_operand<unsigned char> { using __type = byte; };

  template<> struct __byte_operand<wchar_t> { using __type = byte; };




  template<> struct __byte_operand<char16_t> { using __type = byte; };
  template<> struct __byte_operand<char32_t> { using __type = byte; };
  template<> struct __byte_operand<short> { using __type = byte; };
  template<> struct __byte_operand<unsigned short> { using __type = byte; };
  template<> struct __byte_operand<int> { using __type = byte; };
  template<> struct __byte_operand<unsigned int> { using __type = byte; };
  template<> struct __byte_operand<long> { using __type = byte; };
  template<> struct __byte_operand<unsigned long> { using __type = byte; };
  template<> struct __byte_operand<long long> { using __type = byte; };
  template<> struct __byte_operand<unsigned long long> { using __type = byte; };

  template<> struct __byte_operand<__int128>
  { using __type = byte; };
  template<> struct __byte_operand<unsigned __int128>
  { using __type = byte; };
# 110 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstddef" 3
  template<typename _IntegerType>
    struct __byte_operand<const _IntegerType>
    : __byte_operand<_IntegerType> { };
  template<typename _IntegerType>
    struct __byte_operand<volatile _IntegerType>
    : __byte_operand<_IntegerType> { };
  template<typename _IntegerType>
    struct __byte_operand<const volatile _IntegerType>
    : __byte_operand<_IntegerType> { };

  template<typename _IntegerType>
    using __byte_op_t = typename __byte_operand<_IntegerType>::__type;

  template<typename _IntegerType>
    [[__gnu__::__always_inline__]]
    constexpr __byte_op_t<_IntegerType>
    operator<<(byte __b, _IntegerType __shift) noexcept
    { return (byte)(unsigned char)((unsigned)__b << __shift); }

  template<typename _IntegerType>
    [[__gnu__::__always_inline__]]
    constexpr __byte_op_t<_IntegerType>
    operator>>(byte __b, _IntegerType __shift) noexcept
    { return (byte)(unsigned char)((unsigned)__b >> __shift); }

  [[__gnu__::__always_inline__]]
  constexpr byte
  operator|(byte __l, byte __r) noexcept
  { return (byte)(unsigned char)((unsigned)__l | (unsigned)__r); }

  [[__gnu__::__always_inline__]]
  constexpr byte
  operator&(byte __l, byte __r) noexcept
  { return (byte)(unsigned char)((unsigned)__l & (unsigned)__r); }

  [[__gnu__::__always_inline__]]
  constexpr byte
  operator^(byte __l, byte __r) noexcept
  { return (byte)(unsigned char)((unsigned)__l ^ (unsigned)__r); }

  [[__gnu__::__always_inline__]]
  constexpr byte
  operator~(byte __b) noexcept
  { return (byte)(unsigned char)~(unsigned)__b; }

  template<typename _IntegerType>
    [[__gnu__::__always_inline__]]
    constexpr __byte_op_t<_IntegerType>&
    operator<<=(byte& __b, _IntegerType __shift) noexcept
    { return __b = __b << __shift; }

  template<typename _IntegerType>
    [[__gnu__::__always_inline__]]
    constexpr __byte_op_t<_IntegerType>&
    operator>>=(byte& __b, _IntegerType __shift) noexcept
    { return __b = __b >> __shift; }

  [[__gnu__::__always_inline__]]
  constexpr byte&
  operator|=(byte& __l, byte __r) noexcept
  { return __l = __l | __r; }

  [[__gnu__::__always_inline__]]
  constexpr byte&
  operator&=(byte& __l, byte __r) noexcept
  { return __l = __l & __r; }

  [[__gnu__::__always_inline__]]
  constexpr byte&
  operator^=(byte& __l, byte __r) noexcept
  { return __l = __l ^ __r; }

  template<typename _IntegerType>
    [[nodiscard,__gnu__::__always_inline__]]
    constexpr _IntegerType
    to_integer(__byte_op_t<_IntegerType> __b) noexcept
    { return _IntegerType(__b); }


}

}
# 10 "include/kernel/mmu/heap/heap.h" 2
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 1
# 32 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint"
       
# 33 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 3
# 41 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 3
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stdint.h" 1 3
# 9 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stdint.h" 3
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/14.2.1/include/stdint.h" 1 3 4
# 9 "/usr/lib/gcc/x86_64-pc-linux-gnu/14.2.1/include/stdint.h" 3 4
# 1 "/usr/include/stdint.h" 1 3 4
# 26 "/usr/include/stdint.h" 3 4
# 1 "/usr/include/bits/libc-header-start.h" 1 3 4
# 27 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/bits/types.h" 1 3 4
# 27 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 28 "/usr/include/bits/types.h" 2 3 4
# 1 "/usr/include/bits/timesize.h" 1 3 4
# 19 "/usr/include/bits/timesize.h" 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 20 "/usr/include/bits/timesize.h" 2 3 4
# 29 "/usr/include/bits/types.h" 2 3 4


typedef unsigned char __u_char;
typedef unsigned short int __u_short;
typedef unsigned int __u_int;
typedef unsigned long int __u_long;


typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef signed short int __int16_t;
typedef unsigned short int __uint16_t;
typedef signed int __int32_t;
typedef unsigned int __uint32_t;

typedef signed long int __int64_t;
typedef unsigned long int __uint64_t;






typedef __int8_t __int_least8_t;
typedef __uint8_t __uint_least8_t;
typedef __int16_t __int_least16_t;
typedef __uint16_t __uint_least16_t;
typedef __int32_t __int_least32_t;
typedef __uint32_t __uint_least32_t;
typedef __int64_t __int_least64_t;
typedef __uint64_t __uint_least64_t;



typedef long int __quad_t;
typedef unsigned long int __u_quad_t;







typedef long int __intmax_t;
typedef unsigned long int __uintmax_t;
# 141 "/usr/include/bits/types.h" 3 4
# 1 "/usr/include/bits/typesizes.h" 1 3 4
# 142 "/usr/include/bits/types.h" 2 3 4
# 1 "/usr/include/bits/time64.h" 1 3 4
# 143 "/usr/include/bits/types.h" 2 3 4


typedef unsigned long int __dev_t;
typedef unsigned int __uid_t;
typedef unsigned int __gid_t;
typedef unsigned long int __ino_t;
typedef unsigned long int __ino64_t;
typedef unsigned int __mode_t;
typedef unsigned long int __nlink_t;
typedef long int __off_t;
typedef long int __off64_t;
typedef int __pid_t;
typedef struct { int __val[2]; } __fsid_t;
typedef long int __clock_t;
typedef unsigned long int __rlim_t;
typedef unsigned long int __rlim64_t;
typedef unsigned int __id_t;
typedef long int __time_t;
typedef unsigned int __useconds_t;
typedef long int __suseconds_t;
typedef long int __suseconds64_t;

typedef int __daddr_t;
typedef int __key_t;


typedef int __clockid_t;


typedef void * __timer_t;


typedef long int __blksize_t;




typedef long int __blkcnt_t;
typedef long int __blkcnt64_t;


typedef unsigned long int __fsblkcnt_t;
typedef unsigned long int __fsblkcnt64_t;


typedef unsigned long int __fsfilcnt_t;
typedef unsigned long int __fsfilcnt64_t;


typedef long int __fsword_t;

typedef long int __ssize_t;


typedef long int __syscall_slong_t;

typedef unsigned long int __syscall_ulong_t;



typedef __off64_t __loff_t;
typedef char *__caddr_t;


typedef long int __intptr_t;


typedef unsigned int __socklen_t;




typedef int __sig_atomic_t;
# 28 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/bits/wchar.h" 1 3 4
# 29 "/usr/include/stdint.h" 2 3 4
# 1 "/usr/include/bits/wordsize.h" 1 3 4
# 30 "/usr/include/stdint.h" 2 3 4




# 1 "/usr/include/bits/stdint-intn.h" 1 3 4
# 24 "/usr/include/bits/stdint-intn.h" 3 4
typedef __int8_t int8_t;
typedef __int16_t int16_t;
typedef __int32_t int32_t;
typedef __int64_t int64_t;
# 35 "/usr/include/stdint.h" 2 3 4


# 1 "/usr/include/bits/stdint-uintn.h" 1 3 4
# 24 "/usr/include/bits/stdint-uintn.h" 3 4
typedef __uint8_t uint8_t;
typedef __uint16_t uint16_t;
typedef __uint32_t uint32_t;
typedef __uint64_t uint64_t;
# 38 "/usr/include/stdint.h" 2 3 4



# 1 "/usr/include/bits/stdint-least.h" 1 3 4
# 25 "/usr/include/bits/stdint-least.h" 3 4
typedef __int_least8_t int_least8_t;
typedef __int_least16_t int_least16_t;
typedef __int_least32_t int_least32_t;
typedef __int_least64_t int_least64_t;


typedef __uint_least8_t uint_least8_t;
typedef __uint_least16_t uint_least16_t;
typedef __uint_least32_t uint_least32_t;
typedef __uint_least64_t uint_least64_t;
# 42 "/usr/include/stdint.h" 2 3 4





typedef signed char int_fast8_t;

typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
# 60 "/usr/include/stdint.h" 3 4
typedef unsigned char uint_fast8_t;

typedef unsigned long int uint_fast16_t;
typedef unsigned long int uint_fast32_t;
typedef unsigned long int uint_fast64_t;
# 76 "/usr/include/stdint.h" 3 4
typedef long int intptr_t;


typedef unsigned long int uintptr_t;
# 90 "/usr/include/stdint.h" 3 4
typedef __intmax_t intmax_t;
typedef __uintmax_t uintmax_t;
# 10 "/usr/lib/gcc/x86_64-pc-linux-gnu/14.2.1/include/stdint.h" 2 3 4
# 10 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stdint.h" 2 3
# 42 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 2 3


namespace std
{

  using ::int8_t;
  using ::int16_t;
  using ::int32_t;
  using ::int64_t;

  using ::int_fast8_t;
  using ::int_fast16_t;
  using ::int_fast32_t;
  using ::int_fast64_t;

  using ::int_least8_t;
  using ::int_least16_t;
  using ::int_least32_t;
  using ::int_least64_t;

  using ::intmax_t;
  using ::intptr_t;

  using ::uint8_t;
  using ::uint16_t;
  using ::uint32_t;
  using ::uint64_t;

  using ::uint_fast8_t;
  using ::uint_fast16_t;
  using ::uint_fast32_t;
  using ::uint_fast64_t;

  using ::uint_least8_t;
  using ::uint_least16_t;
  using ::uint_least32_t;
  using ::uint_least64_t;

  using ::uintmax_t;
  using ::uintptr_t;





}
# 11 "include/kernel/mmu/heap/heap.h" 2


# 12 "include/kernel/mmu/heap/heap.h"
namespace mmu{
namespace heap{
    struct node{
        size_t size;
        size_t freedSize;
        size_t allocSize;
        bool free;
        node* prev;
        node* next;
    };
    void initialize(uint64_t pmm_size, uint64_t vmm_max);
    bool isInitialized();
    void* allocate(size_t size);
    void free(void* ptr, size_t size);
    void free(void* ptr);
    void printInfo();
};
};
# 15 "include/kernel/mmu/mmu.h" 2
# 1 "include/kernel/mmu/pmm/pmm.h" 1
# 12 "include/kernel/mmu/pmm/pmm.h"
namespace mmu{
namespace pmm{
    struct node{
        size_t size;
        node* next;
    };
    void initialize();
    bool isInitialized();
    uint64_t allocate();
    uint64_t allocVirtual(uint64_t size);
    void free(void* ptr);
    void printInfo();
};
};
# 16 "include/kernel/mmu/mmu.h" 2
# 1 "include/kernel/mmu/vmm/vmm.h" 1
# 16 "include/kernel/mmu/vmm/vmm.h"
# 1 "include/kernel/task/types.h" 1
# 14 "include/kernel/task/types.h"
namespace task{
    using pid_t = size_t;
};
# 17 "include/kernel/mmu/vmm/vmm.h" 2

# 1 "include/kernel/mmu/vmm/types.h" 1
# 11 "include/kernel/mmu/vmm/types.h"
namespace mmu::vmm{
    struct PML4{
        uint64_t present : 1;
        uint64_t rw : 1;
        uint64_t user : 1;
        uint64_t pwt : 1;
        uint64_t pcd : 1;
        uint64_t accesed : 1;
        uint64_t ignored : 1;
        uint64_t mbz : 2;
        uint64_t ats0 : 3;
        uint64_t pdpe_ptr : 40;
        uint64_t ats1 : 11;
        uint64_t no_execute : 1;
    } __attribute__((packed));
    static_assert(sizeof(PML4) == 8, "Structure PML4 isn't 8 bytes big.");

    struct PDPE{
        uint64_t present : 1;
        uint64_t rw : 1;
        uint64_t user : 1;
        uint64_t pwt : 1;
        uint64_t pcd : 1;
        uint64_t accesed : 1;
        uint64_t ignored : 1;
        uint64_t mbz : 1;
        uint64_t ignored2 : 1;
        uint64_t ats0 : 3;
        uint64_t pde_ptr : 40;
        uint64_t ats1 : 11;
        uint64_t no_execute : 1;
    } __attribute__((packed));
    static_assert(sizeof(PDPE) == 8, "Structure PDP isn't 8 bytes big.");

    struct PDE{
        uint64_t present : 1;
        uint64_t rw : 1;
        uint64_t user : 1;
        uint64_t pwt : 1;
        uint64_t pcd : 1;
        uint64_t accesed : 1;
        uint64_t ignored : 1;
        uint64_t mbz : 1;
        uint64_t ignored2 : 1;
        uint64_t ats0 : 3;
        uint64_t pte_ptr : 40;
        uint64_t ats1 : 11;
        uint64_t no_execute : 1;
    } __attribute__((packed));
    static_assert(sizeof(PDE) == 8, "Structure PD isn't 8 bytes big.");

    struct PTE{
        uint64_t present : 1;
        uint64_t rw : 1;
        uint64_t user : 1;
        uint64_t pwt : 1;
        uint64_t pcd : 1;
        uint64_t accesed : 1;
        uint64_t dirty : 1;
        uint64_t pat : 1;
        uint64_t global : 1;
        uint64_t ats0 : 3;
        uint64_t papn_ppn : 40;
        uint64_t ats1 : 7;
        uint64_t pkeys : 4;
        uint64_t no_execute : 1;
    } __attribute__((packed));
    static_assert(sizeof(PTE) == 8, "Structure PT isn't 8 bytes big.");
};
# 19 "include/kernel/mmu/vmm/vmm.h" 2
# 27 "include/kernel/mmu/vmm/vmm.h"
namespace mmu{
namespace vmm{
    struct vmm_address{
        uint64_t padding : 16;
        uint64_t pml4e : 9;
        uint64_t pdpe : 9;
        uint64_t pde : 9;
        uint64_t pte : 9;
        uint64_t offset : 12;
    } __attribute__((packed));
    void initialize();
    bool isInitialized();
    PML4* getPML4(task::pid_t pid);
    void mapPage(PML4* pml4, size_t physicalAddr, size_t virtualAddr, int protFlags, int mapFlags);
    void mapPage(size_t virtualAddr);
    uint64_t getPhysicalAddr(PML4* pml4, uint64_t addr, bool silent);
    uint64_t makeVirtual(uint64_t addr);
    uint64_t getHHDM();
};
};
# 17 "include/kernel/mmu/mmu.h" 2

namespace mmu{
    void printInfo();
}
# 2 "kernel/mmu/mmu.cc" 2

namespace mmu{
    void printInfo(){
        pmm::printInfo();
        heap::printInfo();
    }
}
