# 0 "kernel/hal/arch/x64/idt/isr.cc"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "kernel/hal/arch/x64/idt/isr.cc"






# 1 "include/kernel/hal/arch/x64/idt/isr.h" 1
# 10 "include/kernel/hal/arch/x64/idt/isr.h"
namespace hal::arch::x64::idt{
    void initGates();
};
# 8 "kernel/hal/arch/x64/idt/isr.cc" 2
# 1 "include/kernel/hal/arch/x64/idt/idt.h" 1
# 9 "include/kernel/hal/arch/x64/idt/idt.h"
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 1
# 32 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint"
       
# 33 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 3





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
# 39 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cstdint" 2 3


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
# 10 "include/kernel/hal/arch/x64/idt/idt.h" 2


# 11 "include/kernel/hal/arch/x64/idt/idt.h"
namespace hal::arch::x64::idt{
    struct IDTEntry{
        uint16_t offset0;
        uint16_t segment_sel;
        uint8_t ist : 3;
        uint8_t reserved0 : 5;
        uint8_t gate_type : 4;
        uint8_t zero : 1;
        uint8_t dpl : 2;
        uint8_t present : 1;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t reserved1;
    } __attribute__((packed));
# 42 "include/kernel/hal/arch/x64/idt/idt.h"
    void init();
    void enableGate(uint8_t gate);
    void disablePageFaultProtection();
    void enablePageFaultProtection();
    void disableUDProtection();
    void enableUDProtection();
    void registerHandler(uint8_t gate, void* function, uint8_t type);
};
# 9 "kernel/hal/arch/x64/idt/isr.cc" 2
# 1 "include/common/io/regs.h" 1
# 10 "include/common/io/regs.h"
namespace io{
    struct Registers{
        uint64_t cr3, gs, fs, es, ds, r15, r14, r13, r12, r11, r10, r9, r8, rdi, rsi, rbp, rsp, rdx, rcx, rbx, rax, interrupt_number, error_code, rip, cs, rflags, orig_rsp, ss;
    } __attribute__((packed));
}
# 10 "kernel/hal/arch/x64/idt/isr.cc" 2

extern "C" void isrHandler0(io::Registers* regs);
extern "C" void isrHandler1(io::Registers* regs);
extern "C" void isrHandler2(io::Registers* regs);
extern "C" void isrHandler3(io::Registers* regs);
extern "C" void isrHandler4(io::Registers* regs);
extern "C" void isrHandler5(io::Registers* regs);
extern "C" void isrHandler6(io::Registers* regs);
extern "C" void isrHandler7(io::Registers* regs);
extern "C" void isrHandler8(io::Registers* regs);
extern "C" void isrHandler9(io::Registers* regs);
extern "C" void isrHandler10(io::Registers* regs);
extern "C" void isrHandler11(io::Registers* regs);
extern "C" void isrHandler12(io::Registers* regs);
extern "C" void isrHandler13(io::Registers* regs);
extern "C" void isrHandler14(io::Registers* regs);
extern "C" void isrHandler15(io::Registers* regs);
extern "C" void isrHandler16(io::Registers* regs);
extern "C" void isrHandler17(io::Registers* regs);
extern "C" void isrHandler18(io::Registers* regs);
extern "C" void isrHandler19(io::Registers* regs);
extern "C" void isrHandler20(io::Registers* regs);
extern "C" void isrHandler21(io::Registers* regs);
extern "C" void isrHandler22(io::Registers* regs);
extern "C" void isrHandler23(io::Registers* regs);
extern "C" void isrHandler24(io::Registers* regs);
extern "C" void isrHandler25(io::Registers* regs);
extern "C" void isrHandler26(io::Registers* regs);
extern "C" void isrHandler27(io::Registers* regs);
extern "C" void isrHandler28(io::Registers* regs);
extern "C" void isrHandler29(io::Registers* regs);
extern "C" void isrHandler30(io::Registers* regs);
extern "C" void isrHandler31(io::Registers* regs);
extern "C" void isrHandler32(io::Registers* regs);
extern "C" void isrHandler33(io::Registers* regs);
extern "C" void isrHandler34(io::Registers* regs);
extern "C" void isrHandler35(io::Registers* regs);
extern "C" void isrHandler36(io::Registers* regs);
extern "C" void isrHandler37(io::Registers* regs);
extern "C" void isrHandler38(io::Registers* regs);
extern "C" void isrHandler39(io::Registers* regs);
extern "C" void isrHandler40(io::Registers* regs);
extern "C" void isrHandler41(io::Registers* regs);
extern "C" void isrHandler42(io::Registers* regs);
extern "C" void isrHandler43(io::Registers* regs);
extern "C" void isrHandler44(io::Registers* regs);
extern "C" void isrHandler45(io::Registers* regs);
extern "C" void isrHandler46(io::Registers* regs);
extern "C" void isrHandler47(io::Registers* regs);
extern "C" void isrHandler48(io::Registers* regs);
extern "C" void isrHandler49(io::Registers* regs);
extern "C" void isrHandler50(io::Registers* regs);
extern "C" void isrHandler51(io::Registers* regs);
extern "C" void isrHandler52(io::Registers* regs);
extern "C" void isrHandler53(io::Registers* regs);
extern "C" void isrHandler54(io::Registers* regs);
extern "C" void isrHandler55(io::Registers* regs);
extern "C" void isrHandler56(io::Registers* regs);
extern "C" void isrHandler57(io::Registers* regs);
extern "C" void isrHandler58(io::Registers* regs);
extern "C" void isrHandler59(io::Registers* regs);
extern "C" void isrHandler60(io::Registers* regs);
extern "C" void isrHandler61(io::Registers* regs);
extern "C" void isrHandler62(io::Registers* regs);
extern "C" void isrHandler63(io::Registers* regs);
extern "C" void isrHandler64(io::Registers* regs);
extern "C" void isrHandler65(io::Registers* regs);
extern "C" void isrHandler66(io::Registers* regs);
extern "C" void isrHandler67(io::Registers* regs);
extern "C" void isrHandler68(io::Registers* regs);
extern "C" void isrHandler69(io::Registers* regs);
extern "C" void isrHandler70(io::Registers* regs);
extern "C" void isrHandler71(io::Registers* regs);
extern "C" void isrHandler72(io::Registers* regs);
extern "C" void isrHandler73(io::Registers* regs);
extern "C" void isrHandler74(io::Registers* regs);
extern "C" void isrHandler75(io::Registers* regs);
extern "C" void isrHandler76(io::Registers* regs);
extern "C" void isrHandler77(io::Registers* regs);
extern "C" void isrHandler78(io::Registers* regs);
extern "C" void isrHandler79(io::Registers* regs);
extern "C" void isrHandler80(io::Registers* regs);
extern "C" void isrHandler81(io::Registers* regs);
extern "C" void isrHandler82(io::Registers* regs);
extern "C" void isrHandler83(io::Registers* regs);
extern "C" void isrHandler84(io::Registers* regs);
extern "C" void isrHandler85(io::Registers* regs);
extern "C" void isrHandler86(io::Registers* regs);
extern "C" void isrHandler87(io::Registers* regs);
extern "C" void isrHandler88(io::Registers* regs);
extern "C" void isrHandler89(io::Registers* regs);
extern "C" void isrHandler90(io::Registers* regs);
extern "C" void isrHandler91(io::Registers* regs);
extern "C" void isrHandler92(io::Registers* regs);
extern "C" void isrHandler93(io::Registers* regs);
extern "C" void isrHandler94(io::Registers* regs);
extern "C" void isrHandler95(io::Registers* regs);
extern "C" void isrHandler96(io::Registers* regs);
extern "C" void isrHandler97(io::Registers* regs);
extern "C" void isrHandler98(io::Registers* regs);
extern "C" void isrHandler99(io::Registers* regs);
extern "C" void isrHandler100(io::Registers* regs);
extern "C" void isrHandler101(io::Registers* regs);
extern "C" void isrHandler102(io::Registers* regs);
extern "C" void isrHandler103(io::Registers* regs);
extern "C" void isrHandler104(io::Registers* regs);
extern "C" void isrHandler105(io::Registers* regs);
extern "C" void isrHandler106(io::Registers* regs);
extern "C" void isrHandler107(io::Registers* regs);
extern "C" void isrHandler108(io::Registers* regs);
extern "C" void isrHandler109(io::Registers* regs);
extern "C" void isrHandler110(io::Registers* regs);
extern "C" void isrHandler111(io::Registers* regs);
extern "C" void isrHandler112(io::Registers* regs);
extern "C" void isrHandler113(io::Registers* regs);
extern "C" void isrHandler114(io::Registers* regs);
extern "C" void isrHandler115(io::Registers* regs);
extern "C" void isrHandler116(io::Registers* regs);
extern "C" void isrHandler117(io::Registers* regs);
extern "C" void isrHandler118(io::Registers* regs);
extern "C" void isrHandler119(io::Registers* regs);
extern "C" void isrHandler120(io::Registers* regs);
extern "C" void isrHandler121(io::Registers* regs);
extern "C" void isrHandler122(io::Registers* regs);
extern "C" void isrHandler123(io::Registers* regs);
extern "C" void isrHandler124(io::Registers* regs);
extern "C" void isrHandler125(io::Registers* regs);
extern "C" void isrHandler126(io::Registers* regs);
extern "C" void isrHandler127(io::Registers* regs);
extern "C" void isrHandler128(io::Registers* regs);
extern "C" void isrHandler129(io::Registers* regs);
extern "C" void isrHandler130(io::Registers* regs);
extern "C" void isrHandler131(io::Registers* regs);
extern "C" void isrHandler132(io::Registers* regs);
extern "C" void isrHandler133(io::Registers* regs);
extern "C" void isrHandler134(io::Registers* regs);
extern "C" void isrHandler135(io::Registers* regs);
extern "C" void isrHandler136(io::Registers* regs);
extern "C" void isrHandler137(io::Registers* regs);
extern "C" void isrHandler138(io::Registers* regs);
extern "C" void isrHandler139(io::Registers* regs);
extern "C" void isrHandler140(io::Registers* regs);
extern "C" void isrHandler141(io::Registers* regs);
extern "C" void isrHandler142(io::Registers* regs);
extern "C" void isrHandler143(io::Registers* regs);
extern "C" void isrHandler144(io::Registers* regs);
extern "C" void isrHandler145(io::Registers* regs);
extern "C" void isrHandler146(io::Registers* regs);
extern "C" void isrHandler147(io::Registers* regs);
extern "C" void isrHandler148(io::Registers* regs);
extern "C" void isrHandler149(io::Registers* regs);
extern "C" void isrHandler150(io::Registers* regs);
extern "C" void isrHandler151(io::Registers* regs);
extern "C" void isrHandler152(io::Registers* regs);
extern "C" void isrHandler153(io::Registers* regs);
extern "C" void isrHandler154(io::Registers* regs);
extern "C" void isrHandler155(io::Registers* regs);
extern "C" void isrHandler156(io::Registers* regs);
extern "C" void isrHandler157(io::Registers* regs);
extern "C" void isrHandler158(io::Registers* regs);
extern "C" void isrHandler159(io::Registers* regs);
extern "C" void isrHandler160(io::Registers* regs);
extern "C" void isrHandler161(io::Registers* regs);
extern "C" void isrHandler162(io::Registers* regs);
extern "C" void isrHandler163(io::Registers* regs);
extern "C" void isrHandler164(io::Registers* regs);
extern "C" void isrHandler165(io::Registers* regs);
extern "C" void isrHandler166(io::Registers* regs);
extern "C" void isrHandler167(io::Registers* regs);
extern "C" void isrHandler168(io::Registers* regs);
extern "C" void isrHandler169(io::Registers* regs);
extern "C" void isrHandler170(io::Registers* regs);
extern "C" void isrHandler171(io::Registers* regs);
extern "C" void isrHandler172(io::Registers* regs);
extern "C" void isrHandler173(io::Registers* regs);
extern "C" void isrHandler174(io::Registers* regs);
extern "C" void isrHandler175(io::Registers* regs);
extern "C" void isrHandler176(io::Registers* regs);
extern "C" void isrHandler177(io::Registers* regs);
extern "C" void isrHandler178(io::Registers* regs);
extern "C" void isrHandler179(io::Registers* regs);
extern "C" void isrHandler180(io::Registers* regs);
extern "C" void isrHandler181(io::Registers* regs);
extern "C" void isrHandler182(io::Registers* regs);
extern "C" void isrHandler183(io::Registers* regs);
extern "C" void isrHandler184(io::Registers* regs);
extern "C" void isrHandler185(io::Registers* regs);
extern "C" void isrHandler186(io::Registers* regs);
extern "C" void isrHandler187(io::Registers* regs);
extern "C" void isrHandler188(io::Registers* regs);
extern "C" void isrHandler189(io::Registers* regs);
extern "C" void isrHandler190(io::Registers* regs);
extern "C" void isrHandler191(io::Registers* regs);
extern "C" void isrHandler192(io::Registers* regs);
extern "C" void isrHandler193(io::Registers* regs);
extern "C" void isrHandler194(io::Registers* regs);
extern "C" void isrHandler195(io::Registers* regs);
extern "C" void isrHandler196(io::Registers* regs);
extern "C" void isrHandler197(io::Registers* regs);
extern "C" void isrHandler198(io::Registers* regs);
extern "C" void isrHandler199(io::Registers* regs);
extern "C" void isrHandler200(io::Registers* regs);
extern "C" void isrHandler201(io::Registers* regs);
extern "C" void isrHandler202(io::Registers* regs);
extern "C" void isrHandler203(io::Registers* regs);
extern "C" void isrHandler204(io::Registers* regs);
extern "C" void isrHandler205(io::Registers* regs);
extern "C" void isrHandler206(io::Registers* regs);
extern "C" void isrHandler207(io::Registers* regs);
extern "C" void isrHandler208(io::Registers* regs);
extern "C" void isrHandler209(io::Registers* regs);
extern "C" void isrHandler210(io::Registers* regs);
extern "C" void isrHandler211(io::Registers* regs);
extern "C" void isrHandler212(io::Registers* regs);
extern "C" void isrHandler213(io::Registers* regs);
extern "C" void isrHandler214(io::Registers* regs);
extern "C" void isrHandler215(io::Registers* regs);
extern "C" void isrHandler216(io::Registers* regs);
extern "C" void isrHandler217(io::Registers* regs);
extern "C" void isrHandler218(io::Registers* regs);
extern "C" void isrHandler219(io::Registers* regs);
extern "C" void isrHandler220(io::Registers* regs);
extern "C" void isrHandler221(io::Registers* regs);
extern "C" void isrHandler222(io::Registers* regs);
extern "C" void isrHandler223(io::Registers* regs);
extern "C" void isrHandler224(io::Registers* regs);
extern "C" void isrHandler225(io::Registers* regs);
extern "C" void isrHandler226(io::Registers* regs);
extern "C" void isrHandler227(io::Registers* regs);
extern "C" void isrHandler228(io::Registers* regs);
extern "C" void isrHandler229(io::Registers* regs);
extern "C" void isrHandler230(io::Registers* regs);
extern "C" void isrHandler231(io::Registers* regs);
extern "C" void isrHandler232(io::Registers* regs);
extern "C" void isrHandler233(io::Registers* regs);
extern "C" void isrHandler234(io::Registers* regs);
extern "C" void isrHandler235(io::Registers* regs);
extern "C" void isrHandler236(io::Registers* regs);
extern "C" void isrHandler237(io::Registers* regs);
extern "C" void isrHandler238(io::Registers* regs);
extern "C" void isrHandler239(io::Registers* regs);
extern "C" void isrHandler240(io::Registers* regs);
extern "C" void isrHandler241(io::Registers* regs);
extern "C" void isrHandler242(io::Registers* regs);
extern "C" void isrHandler243(io::Registers* regs);
extern "C" void isrHandler244(io::Registers* regs);
extern "C" void isrHandler245(io::Registers* regs);
extern "C" void isrHandler246(io::Registers* regs);
extern "C" void isrHandler247(io::Registers* regs);
extern "C" void isrHandler248(io::Registers* regs);
extern "C" void isrHandler249(io::Registers* regs);
extern "C" void isrHandler250(io::Registers* regs);
extern "C" void isrHandler251(io::Registers* regs);
extern "C" void isrHandler252(io::Registers* regs);
extern "C" void isrHandler253(io::Registers* regs);
extern "C" void isrHandler254(io::Registers* regs);
extern "C" void isrHandler255(io::Registers* regs);
namespace hal::arch::x64::idt{
    void initGates(){
        registerHandler(0, (void*)isrHandler0, 0xE);
        registerHandler(1, (void*)isrHandler1, 0xE);
        registerHandler(2, (void*)isrHandler2, 0xE);
        registerHandler(3, (void*)isrHandler3, 0xE);
        registerHandler(4, (void*)isrHandler4, 0xE);
        registerHandler(5, (void*)isrHandler5, 0xE);
        registerHandler(6, (void*)isrHandler6, 0xE);
        registerHandler(7, (void*)isrHandler7, 0xE);
        registerHandler(8, (void*)isrHandler8, 0xE);
        registerHandler(9, (void*)isrHandler9, 0xE);
        registerHandler(10, (void*)isrHandler10, 0xE);
        registerHandler(11, (void*)isrHandler11, 0xE);
        registerHandler(12, (void*)isrHandler12, 0xE);
        registerHandler(13, (void*)isrHandler13, 0xE);
        registerHandler(14, (void*)isrHandler14, 0xE);
        registerHandler(15, (void*)isrHandler15, 0xE);
        registerHandler(16, (void*)isrHandler16, 0xE);
        registerHandler(17, (void*)isrHandler17, 0xE);
        registerHandler(18, (void*)isrHandler18, 0xE);
        registerHandler(19, (void*)isrHandler19, 0xE);
        registerHandler(20, (void*)isrHandler20, 0xE);
        registerHandler(21, (void*)isrHandler21, 0xE);
        registerHandler(22, (void*)isrHandler22, 0xE);
        registerHandler(23, (void*)isrHandler23, 0xE);
        registerHandler(24, (void*)isrHandler24, 0xE);
        registerHandler(25, (void*)isrHandler25, 0xE);
        registerHandler(26, (void*)isrHandler26, 0xE);
        registerHandler(27, (void*)isrHandler27, 0xE);
        registerHandler(28, (void*)isrHandler28, 0xE);
        registerHandler(29, (void*)isrHandler29, 0xE);
        registerHandler(30, (void*)isrHandler30, 0xE);
        registerHandler(31, (void*)isrHandler31, 0xE);
        registerHandler(32, (void*)isrHandler32, 0xE);
        registerHandler(33, (void*)isrHandler33, 0xE);
        registerHandler(34, (void*)isrHandler34, 0xE);
        registerHandler(35, (void*)isrHandler35, 0xE);
        registerHandler(36, (void*)isrHandler36, 0xE);
        registerHandler(37, (void*)isrHandler37, 0xE);
        registerHandler(38, (void*)isrHandler38, 0xE);
        registerHandler(39, (void*)isrHandler39, 0xE);
        registerHandler(40, (void*)isrHandler40, 0xE);
        registerHandler(41, (void*)isrHandler41, 0xE);
        registerHandler(42, (void*)isrHandler42, 0xE);
        registerHandler(43, (void*)isrHandler43, 0xE);
        registerHandler(44, (void*)isrHandler44, 0xE);
        registerHandler(45, (void*)isrHandler45, 0xE);
        registerHandler(46, (void*)isrHandler46, 0xE);
        registerHandler(47, (void*)isrHandler47, 0xE);
        registerHandler(48, (void*)isrHandler48, 0xE);
        registerHandler(49, (void*)isrHandler49, 0xE);
        registerHandler(50, (void*)isrHandler50, 0xE);
        registerHandler(51, (void*)isrHandler51, 0xE);
        registerHandler(52, (void*)isrHandler52, 0xE);
        registerHandler(53, (void*)isrHandler53, 0xE);
        registerHandler(54, (void*)isrHandler54, 0xE);
        registerHandler(55, (void*)isrHandler55, 0xE);
        registerHandler(56, (void*)isrHandler56, 0xE);
        registerHandler(57, (void*)isrHandler57, 0xE);
        registerHandler(58, (void*)isrHandler58, 0xE);
        registerHandler(59, (void*)isrHandler59, 0xE);
        registerHandler(60, (void*)isrHandler60, 0xE);
        registerHandler(61, (void*)isrHandler61, 0xE);
        registerHandler(62, (void*)isrHandler62, 0xE);
        registerHandler(63, (void*)isrHandler63, 0xE);
        registerHandler(64, (void*)isrHandler64, 0xE);
        registerHandler(65, (void*)isrHandler65, 0xE);
        registerHandler(66, (void*)isrHandler66, 0xE);
        registerHandler(67, (void*)isrHandler67, 0xE);
        registerHandler(68, (void*)isrHandler68, 0xE);
        registerHandler(69, (void*)isrHandler69, 0xE);
        registerHandler(70, (void*)isrHandler70, 0xE);
        registerHandler(71, (void*)isrHandler71, 0xE);
        registerHandler(72, (void*)isrHandler72, 0xE);
        registerHandler(73, (void*)isrHandler73, 0xE);
        registerHandler(74, (void*)isrHandler74, 0xE);
        registerHandler(75, (void*)isrHandler75, 0xE);
        registerHandler(76, (void*)isrHandler76, 0xE);
        registerHandler(77, (void*)isrHandler77, 0xE);
        registerHandler(78, (void*)isrHandler78, 0xE);
        registerHandler(79, (void*)isrHandler79, 0xE);
        registerHandler(80, (void*)isrHandler80, 0xE);
        registerHandler(81, (void*)isrHandler81, 0xE);
        registerHandler(82, (void*)isrHandler82, 0xE);
        registerHandler(83, (void*)isrHandler83, 0xE);
        registerHandler(84, (void*)isrHandler84, 0xE);
        registerHandler(85, (void*)isrHandler85, 0xE);
        registerHandler(86, (void*)isrHandler86, 0xE);
        registerHandler(87, (void*)isrHandler87, 0xE);
        registerHandler(88, (void*)isrHandler88, 0xE);
        registerHandler(89, (void*)isrHandler89, 0xE);
        registerHandler(90, (void*)isrHandler90, 0xE);
        registerHandler(91, (void*)isrHandler91, 0xE);
        registerHandler(92, (void*)isrHandler92, 0xE);
        registerHandler(93, (void*)isrHandler93, 0xE);
        registerHandler(94, (void*)isrHandler94, 0xE);
        registerHandler(95, (void*)isrHandler95, 0xE);
        registerHandler(96, (void*)isrHandler96, 0xE);
        registerHandler(97, (void*)isrHandler97, 0xE);
        registerHandler(98, (void*)isrHandler98, 0xE);
        registerHandler(99, (void*)isrHandler99, 0xE);
        registerHandler(100, (void*)isrHandler100, 0xE);
        registerHandler(101, (void*)isrHandler101, 0xE);
        registerHandler(102, (void*)isrHandler102, 0xE);
        registerHandler(103, (void*)isrHandler103, 0xE);
        registerHandler(104, (void*)isrHandler104, 0xE);
        registerHandler(105, (void*)isrHandler105, 0xE);
        registerHandler(106, (void*)isrHandler106, 0xE);
        registerHandler(107, (void*)isrHandler107, 0xE);
        registerHandler(108, (void*)isrHandler108, 0xE);
        registerHandler(109, (void*)isrHandler109, 0xE);
        registerHandler(110, (void*)isrHandler110, 0xE);
        registerHandler(111, (void*)isrHandler111, 0xE);
        registerHandler(112, (void*)isrHandler112, 0xE);
        registerHandler(113, (void*)isrHandler113, 0xE);
        registerHandler(114, (void*)isrHandler114, 0xE);
        registerHandler(115, (void*)isrHandler115, 0xE);
        registerHandler(116, (void*)isrHandler116, 0xE);
        registerHandler(117, (void*)isrHandler117, 0xE);
        registerHandler(118, (void*)isrHandler118, 0xE);
        registerHandler(119, (void*)isrHandler119, 0xE);
        registerHandler(120, (void*)isrHandler120, 0xE);
        registerHandler(121, (void*)isrHandler121, 0xE);
        registerHandler(122, (void*)isrHandler122, 0xE);
        registerHandler(123, (void*)isrHandler123, 0xE);
        registerHandler(124, (void*)isrHandler124, 0xE);
        registerHandler(125, (void*)isrHandler125, 0xE);
        registerHandler(126, (void*)isrHandler126, 0xE);
        registerHandler(127, (void*)isrHandler127, 0xE);
        registerHandler(128, (void*)isrHandler128, 0xE);
        registerHandler(129, (void*)isrHandler129, 0xE);
        registerHandler(130, (void*)isrHandler130, 0xE);
        registerHandler(131, (void*)isrHandler131, 0xE);
        registerHandler(132, (void*)isrHandler132, 0xE);
        registerHandler(133, (void*)isrHandler133, 0xE);
        registerHandler(134, (void*)isrHandler134, 0xE);
        registerHandler(135, (void*)isrHandler135, 0xE);
        registerHandler(136, (void*)isrHandler136, 0xE);
        registerHandler(137, (void*)isrHandler137, 0xE);
        registerHandler(138, (void*)isrHandler138, 0xE);
        registerHandler(139, (void*)isrHandler139, 0xE);
        registerHandler(140, (void*)isrHandler140, 0xE);
        registerHandler(141, (void*)isrHandler141, 0xE);
        registerHandler(142, (void*)isrHandler142, 0xE);
        registerHandler(143, (void*)isrHandler143, 0xE);
        registerHandler(144, (void*)isrHandler144, 0xE);
        registerHandler(145, (void*)isrHandler145, 0xE);
        registerHandler(146, (void*)isrHandler146, 0xE);
        registerHandler(147, (void*)isrHandler147, 0xE);
        registerHandler(148, (void*)isrHandler148, 0xE);
        registerHandler(149, (void*)isrHandler149, 0xE);
        registerHandler(150, (void*)isrHandler150, 0xE);
        registerHandler(151, (void*)isrHandler151, 0xE);
        registerHandler(152, (void*)isrHandler152, 0xE);
        registerHandler(153, (void*)isrHandler153, 0xE);
        registerHandler(154, (void*)isrHandler154, 0xE);
        registerHandler(155, (void*)isrHandler155, 0xE);
        registerHandler(156, (void*)isrHandler156, 0xE);
        registerHandler(157, (void*)isrHandler157, 0xE);
        registerHandler(158, (void*)isrHandler158, 0xE);
        registerHandler(159, (void*)isrHandler159, 0xE);
        registerHandler(160, (void*)isrHandler160, 0xE);
        registerHandler(161, (void*)isrHandler161, 0xE);
        registerHandler(162, (void*)isrHandler162, 0xE);
        registerHandler(163, (void*)isrHandler163, 0xE);
        registerHandler(164, (void*)isrHandler164, 0xE);
        registerHandler(165, (void*)isrHandler165, 0xE);
        registerHandler(166, (void*)isrHandler166, 0xE);
        registerHandler(167, (void*)isrHandler167, 0xE);
        registerHandler(168, (void*)isrHandler168, 0xE);
        registerHandler(169, (void*)isrHandler169, 0xE);
        registerHandler(170, (void*)isrHandler170, 0xE);
        registerHandler(171, (void*)isrHandler171, 0xE);
        registerHandler(172, (void*)isrHandler172, 0xE);
        registerHandler(173, (void*)isrHandler173, 0xE);
        registerHandler(174, (void*)isrHandler174, 0xE);
        registerHandler(175, (void*)isrHandler175, 0xE);
        registerHandler(176, (void*)isrHandler176, 0xE);
        registerHandler(177, (void*)isrHandler177, 0xE);
        registerHandler(178, (void*)isrHandler178, 0xE);
        registerHandler(179, (void*)isrHandler179, 0xE);
        registerHandler(180, (void*)isrHandler180, 0xE);
        registerHandler(181, (void*)isrHandler181, 0xE);
        registerHandler(182, (void*)isrHandler182, 0xE);
        registerHandler(183, (void*)isrHandler183, 0xE);
        registerHandler(184, (void*)isrHandler184, 0xE);
        registerHandler(185, (void*)isrHandler185, 0xE);
        registerHandler(186, (void*)isrHandler186, 0xE);
        registerHandler(187, (void*)isrHandler187, 0xE);
        registerHandler(188, (void*)isrHandler188, 0xE);
        registerHandler(189, (void*)isrHandler189, 0xE);
        registerHandler(190, (void*)isrHandler190, 0xE);
        registerHandler(191, (void*)isrHandler191, 0xE);
        registerHandler(192, (void*)isrHandler192, 0xE);
        registerHandler(193, (void*)isrHandler193, 0xE);
        registerHandler(194, (void*)isrHandler194, 0xE);
        registerHandler(195, (void*)isrHandler195, 0xE);
        registerHandler(196, (void*)isrHandler196, 0xE);
        registerHandler(197, (void*)isrHandler197, 0xE);
        registerHandler(198, (void*)isrHandler198, 0xE);
        registerHandler(199, (void*)isrHandler199, 0xE);
        registerHandler(200, (void*)isrHandler200, 0xE);
        registerHandler(201, (void*)isrHandler201, 0xE);
        registerHandler(202, (void*)isrHandler202, 0xE);
        registerHandler(203, (void*)isrHandler203, 0xE);
        registerHandler(204, (void*)isrHandler204, 0xE);
        registerHandler(205, (void*)isrHandler205, 0xE);
        registerHandler(206, (void*)isrHandler206, 0xE);
        registerHandler(207, (void*)isrHandler207, 0xE);
        registerHandler(208, (void*)isrHandler208, 0xE);
        registerHandler(209, (void*)isrHandler209, 0xE);
        registerHandler(210, (void*)isrHandler210, 0xE);
        registerHandler(211, (void*)isrHandler211, 0xE);
        registerHandler(212, (void*)isrHandler212, 0xE);
        registerHandler(213, (void*)isrHandler213, 0xE);
        registerHandler(214, (void*)isrHandler214, 0xE);
        registerHandler(215, (void*)isrHandler215, 0xE);
        registerHandler(216, (void*)isrHandler216, 0xE);
        registerHandler(217, (void*)isrHandler217, 0xE);
        registerHandler(218, (void*)isrHandler218, 0xE);
        registerHandler(219, (void*)isrHandler219, 0xE);
        registerHandler(220, (void*)isrHandler220, 0xE);
        registerHandler(221, (void*)isrHandler221, 0xE);
        registerHandler(222, (void*)isrHandler222, 0xE);
        registerHandler(223, (void*)isrHandler223, 0xE);
        registerHandler(224, (void*)isrHandler224, 0xE);
        registerHandler(225, (void*)isrHandler225, 0xE);
        registerHandler(226, (void*)isrHandler226, 0xE);
        registerHandler(227, (void*)isrHandler227, 0xE);
        registerHandler(228, (void*)isrHandler228, 0xE);
        registerHandler(229, (void*)isrHandler229, 0xE);
        registerHandler(230, (void*)isrHandler230, 0xE);
        registerHandler(231, (void*)isrHandler231, 0xE);
        registerHandler(232, (void*)isrHandler232, 0xE);
        registerHandler(233, (void*)isrHandler233, 0xE);
        registerHandler(234, (void*)isrHandler234, 0xE);
        registerHandler(235, (void*)isrHandler235, 0xE);
        registerHandler(236, (void*)isrHandler236, 0xE);
        registerHandler(237, (void*)isrHandler237, 0xE);
        registerHandler(238, (void*)isrHandler238, 0xE);
        registerHandler(239, (void*)isrHandler239, 0xE);
        registerHandler(240, (void*)isrHandler240, 0xE);
        registerHandler(241, (void*)isrHandler241, 0xE);
        registerHandler(242, (void*)isrHandler242, 0xE);
        registerHandler(243, (void*)isrHandler243, 0xE);
        registerHandler(244, (void*)isrHandler244, 0xE);
        registerHandler(245, (void*)isrHandler245, 0xE);
        registerHandler(246, (void*)isrHandler246, 0xE);
        registerHandler(247, (void*)isrHandler247, 0xE);
        registerHandler(248, (void*)isrHandler248, 0xE);
        registerHandler(249, (void*)isrHandler249, 0xE);
        registerHandler(250, (void*)isrHandler250, 0xE);
        registerHandler(251, (void*)isrHandler251, 0xE);
        registerHandler(252, (void*)isrHandler252, 0xE);
        registerHandler(253, (void*)isrHandler253, 0xE);
        registerHandler(254, (void*)isrHandler254, 0xE);
        registerHandler(255, (void*)isrHandler255, 0xE);
    }
};
