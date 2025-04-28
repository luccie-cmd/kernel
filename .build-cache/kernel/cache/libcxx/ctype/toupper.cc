# 0 "libcxx/ctype/toupper.cc"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "libcxx/ctype/toupper.cc"
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cctype" 1
# 39 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cctype"
       
# 40 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cctype" 3

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
# 42 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cctype" 2 3
# 1 "/usr/include/ctype.h" 1 3 4
# 26 "/usr/include/ctype.h" 3 4
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
# 27 "/usr/include/ctype.h" 2 3 4

extern "C" {
# 39 "/usr/include/ctype.h" 3 4
# 1 "/usr/include/bits/endian.h" 1 3 4
# 35 "/usr/include/bits/endian.h" 3 4
# 1 "/usr/include/bits/endianness.h" 1 3 4
# 36 "/usr/include/bits/endian.h" 2 3 4
# 40 "/usr/include/ctype.h" 2 3 4






enum
{
  _ISupper = ((0) < 8 ? ((1 << (0)) << 8) : ((1 << (0)) >> 8)),
  _ISlower = ((1) < 8 ? ((1 << (1)) << 8) : ((1 << (1)) >> 8)),
  _ISalpha = ((2) < 8 ? ((1 << (2)) << 8) : ((1 << (2)) >> 8)),
  _ISdigit = ((3) < 8 ? ((1 << (3)) << 8) : ((1 << (3)) >> 8)),
  _ISxdigit = ((4) < 8 ? ((1 << (4)) << 8) : ((1 << (4)) >> 8)),
  _ISspace = ((5) < 8 ? ((1 << (5)) << 8) : ((1 << (5)) >> 8)),
  _ISprint = ((6) < 8 ? ((1 << (6)) << 8) : ((1 << (6)) >> 8)),
  _ISgraph = ((7) < 8 ? ((1 << (7)) << 8) : ((1 << (7)) >> 8)),
  _ISblank = ((8) < 8 ? ((1 << (8)) << 8) : ((1 << (8)) >> 8)),
  _IScntrl = ((9) < 8 ? ((1 << (9)) << 8) : ((1 << (9)) >> 8)),
  _ISpunct = ((10) < 8 ? ((1 << (10)) << 8) : ((1 << (10)) >> 8)),
  _ISalnum = ((11) < 8 ? ((1 << (11)) << 8) : ((1 << (11)) >> 8))
};
# 79 "/usr/include/ctype.h" 3 4
extern const unsigned short int **__ctype_b_loc (void)
     noexcept (true) __attribute__ ((__const__));
extern const __int32_t **__ctype_tolower_loc (void)
     noexcept (true) __attribute__ ((__const__));
extern const __int32_t **__ctype_toupper_loc (void)
     noexcept (true) __attribute__ ((__const__));
# 108 "/usr/include/ctype.h" 3 4
extern int isalnum (int) noexcept (true);
extern int isalpha (int) noexcept (true);
extern int iscntrl (int) noexcept (true);
extern int isdigit (int) noexcept (true);
extern int islower (int) noexcept (true);
extern int isgraph (int) noexcept (true);
extern int isprint (int) noexcept (true);
extern int ispunct (int) noexcept (true);
extern int isspace (int) noexcept (true);
extern int isupper (int) noexcept (true);
extern int isxdigit (int) noexcept (true);



extern int tolower (int __c) noexcept (true);


extern int toupper (int __c) noexcept (true);




extern int isblank (int) noexcept (true);




extern int isctype (int __c, int __mask) noexcept (true);






extern int isascii (int __c) noexcept (true);



extern int toascii (int __c) noexcept (true);



extern int _toupper (int) noexcept (true);
extern int _tolower (int) noexcept (true);
# 237 "/usr/include/ctype.h" 3 4
# 1 "/usr/include/bits/types/locale_t.h" 1 3 4
# 22 "/usr/include/bits/types/locale_t.h" 3 4
# 1 "/usr/include/bits/types/__locale_t.h" 1 3 4
# 27 "/usr/include/bits/types/__locale_t.h" 3 4
struct __locale_struct
{

  struct __locale_data *__locales[13];


  const unsigned short int *__ctype_b;
  const int *__ctype_tolower;
  const int *__ctype_toupper;


  const char *__names[13];
};

typedef struct __locale_struct *__locale_t;
# 23 "/usr/include/bits/types/locale_t.h" 2 3 4

typedef __locale_t locale_t;
# 238 "/usr/include/ctype.h" 2 3 4
# 251 "/usr/include/ctype.h" 3 4
extern int isalnum_l (int, locale_t) noexcept (true);
extern int isalpha_l (int, locale_t) noexcept (true);
extern int iscntrl_l (int, locale_t) noexcept (true);
extern int isdigit_l (int, locale_t) noexcept (true);
extern int islower_l (int, locale_t) noexcept (true);
extern int isgraph_l (int, locale_t) noexcept (true);
extern int isprint_l (int, locale_t) noexcept (true);
extern int ispunct_l (int, locale_t) noexcept (true);
extern int isspace_l (int, locale_t) noexcept (true);
extern int isupper_l (int, locale_t) noexcept (true);
extern int isxdigit_l (int, locale_t) noexcept (true);

extern int isblank_l (int, locale_t) noexcept (true);



extern int __tolower_l (int __c, locale_t __l) noexcept (true);
extern int tolower_l (int __c, locale_t __l) noexcept (true);


extern int __toupper_l (int __c, locale_t __l) noexcept (true);
extern int toupper_l (int __c, locale_t __l) noexcept (true);
# 327 "/usr/include/ctype.h" 3 4
}
# 43 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cctype" 2 3
# 62 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/c++/cctype" 3
namespace std
{
  using ::isalnum;
  using ::isalpha;
  using ::iscntrl;
  using ::isdigit;
  using ::isgraph;
  using ::islower;
  using ::isprint;
  using ::ispunct;
  using ::isspace;
  using ::isupper;
  using ::isxdigit;
  using ::tolower;
  using ::toupper;
}







namespace std
{
  using ::isblank;
}
# 2 "libcxx/ctype/toupper.cc" 2


# 3 "libcxx/ctype/toupper.cc"
extern "C" int toupper(int c){
    return (c <= 'z' && c >= 'a' ? c - 'a' + 'A' : c);
}
