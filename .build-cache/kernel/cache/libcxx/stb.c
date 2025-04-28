# 0 "libcxx/stb.c"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "libcxx/stb.c"
# 9 "libcxx/stb.c"
# 1 "include/thirdparty/stb_sprintf.h" 1
# 196 "include/thirdparty/stb_sprintf.h"
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stdarg.h" 1
# 40 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stdarg.h"
typedef __builtin_va_list __gnuc_va_list;
# 99 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stdarg.h"
typedef __gnuc_va_list va_list;
# 197 "include/thirdparty/stb_sprintf.h" 2
# 1 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h" 1
# 143 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h"
typedef long int ptrdiff_t;
# 209 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h"
typedef long unsigned int size_t;
# 321 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h"
typedef int wchar_t;
# 415 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h"
typedef struct {
  long long __max_align_ll __attribute__((__aligned__(__alignof__(long long))));
  long double __max_align_ld __attribute__((__aligned__(__alignof__(long double))));
# 426 "/usr/lib/gcc/x86_64-pc-linux-gnu/11.4.0/include/stddef.h"
} max_align_t;
# 198 "include/thirdparty/stb_sprintf.h" 2




typedef char *STBSP_SPRINTFCB(const char *buf, void *user, int len);





extern int stbsp_vsprintf(char *buf, char const *fmt, va_list va);
extern int stbsp_vsnprintf(char *buf, int count, char const *fmt, va_list va);
extern int stbsp_sprintf(char *buf, char const *fmt, ...) __attribute__((format(printf,2,3)));
extern int stbsp_snprintf(char *buf, int count, char const *fmt, ...) __attribute__((format(printf,3,4)));

extern int stbsp_vsprintfcb(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va);
extern void stbsp_set_separators(char comma, char period);
# 259 "include/thirdparty/stb_sprintf.h"
static char stbsp__period = '.';
static char stbsp__comma = ',';
static struct
{
   short temp;
   char pair[201];
} stbsp__digitpair =
{
  0,
   "00010203040506070809101112131415161718192021222324"
   "25262728293031323334353637383940414243444546474849"
   "50515253545556575859606162636465666768697071727374"
   "75767778798081828384858687888990919293949596979899"
};

 void stbsp_set_separators(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
}
# 294 "include/thirdparty/stb_sprintf.h"
static void stbsp__lead_sign(unsigned int fl, char *sign)
{
   sign[0] = 0;
   if (fl & 128) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & 4) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & 2) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

static unsigned int stbsp__strlen_limited(char const *s, unsigned int limit)
{
   char const * sn = s;


   for (;;) {
      if (((unsigned long long)sn & 3) == 0)
         break;

      if (!limit || *sn == 0)
         return (unsigned int)(sn - s);

      ++sn;
      --limit;
   }






   while (limit >= 4) {
      unsigned int v = *(unsigned int *)sn;

      if ((v - 0x01010101) & (~v) & 0x80808080UL)
         break;

      sn += 4;
      limit -= 4;
   }


   while (limit && *sn) {
      ++sn;
      --limit;
   }

   return (unsigned int)(sn - s);
}

 int stbsp_vsprintfcb(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      signed int fw, pr, tz;
      unsigned int fl;
# 392 "include/thirdparty/stb_sprintf.h"
      for (;;) {
         while (((unsigned long long)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
            *bf++ = f[0];
            ++f;
         }
         for (;;) {



            unsigned int v, c;
            v = *(unsigned int *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((512 - (int)(bf - buf)) < 4)
                  goto schk1;
# 426 "include/thirdparty/stb_sprintf.h"
            {
                *(unsigned int *)bf = v;
            }
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;


      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;


      for (;;) {
         switch (f[0]) {

         case '-':
            fl |= 1;
            ++f;
            continue;

         case '+':
            fl |= 2;
            ++f;
            continue;

         case ' ':
            fl |= 4;
            ++f;
            continue;

         case '#':
            fl |= 8;
            ++f;
            continue;

         case '\'':
            fl |= 64;
            ++f;
            continue;

         case '$':
            if (fl & 256) {
               if (fl & 2048) {
                  fl |= 4096;
               } else {
                  fl |= 2048;
               }
            } else {
               fl |= 256;
            }
            ++f;
            continue;

         case '_':
            fl |= 1024;
            ++f;
            continue;

         case '0':
            fl |= 16;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:


      if (f[0] == '*') {
         fw = __builtin_va_arg(va,unsigned int);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }

      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = __builtin_va_arg(va,unsigned int);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }


      switch (f[0]) {

      case 'h':
         fl |= 512;
         ++f;
         if (f[0] == 'h')
            ++f;
         break;

      case 'l':
         fl |= ((sizeof(long) == 8) ? 32 : 0);
         ++f;
         if (f[0] == 'l') {
            fl |= 32;
            ++f;
         }
         break;

      case 'j':
         fl |= (sizeof(size_t) == 8) ? 32 : 0;
         ++f;
         break;

      case 'z':
         fl |= (sizeof(ptrdiff_t) == 8) ? 32 : 0;
         ++f;
         break;
      case 't':
         fl |= (sizeof(ptrdiff_t) == 8) ? 32 : 0;
         ++f;
         break;

      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= 32;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? 32 : 0);
            ++f;
         }
         break;
      default: break;
      }


      switch (f[0]) {

         char num[512];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         unsigned int l, n, cs;
         unsigned long long n64;




         signed int dp;

         char const *sn;

      case 's':

         s = __builtin_va_arg(va,char *);
         if (s == 0)
            s = (char *)"null";


         l = stbsp__strlen_limited(s, (pr >= 0) ? (unsigned int)pr : ~0u);
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;

         dp = 0;

         cs = 0;

         goto scopy;

      case 'c':

         s = num + 512 - 1;
         *s = (char)__builtin_va_arg(va,int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;

         dp = 0;

         cs = 0;
         goto scopy;

      case 'n':
      {
         int *d = __builtin_va_arg(va,int *);
         *d = tlen + (int)(bf - buf);
      } break;



      case 'A':
      case 'a':
      case 'G':
      case 'g':
      case 'E':
      case 'e':
      case 'f':
         __builtin_va_arg(va,double);
         s = (char *)"No float";
         l = 8;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         cs = 0;
         (void)sizeof(dp);
         goto scopy;
# 986 "include/thirdparty/stb_sprintf.h"
      case 'B':
      case 'b':
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & 8) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o':
         h = hexu;
         lead[0] = 0;
         if (fl & 8) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p':
         fl |= (sizeof(void *) == 8) ? 32 : 0;
         pr = sizeof(void *) * 2;
         fl &= ~16;


      case 'X':
      case 'x':
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & 8) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:

         if (fl & 32)
            n64 = __builtin_va_arg(va,unsigned long long);
         else
            n64 = __builtin_va_arg(va,unsigned int);

         s = num + 512;

         dp = 0;


         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = 0;
               goto scopy;
            }
         }

         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((signed int)((num + 512) - s) < pr)))
               break;
            if (fl & 64) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };

         cs = (unsigned int)((num + 512) - s) + ((((l >> 4) & 15)) << 24);

         l = (unsigned int)((num + 512) - s);

         goto scopy;

      case 'u':
      case 'i':
      case 'd':

         if (fl & 32) {
            signed long long i64 = __builtin_va_arg(va,signed long long);
            n64 = (unsigned long long)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (unsigned long long)-i64;
               fl |= 128;
            }
         } else {
            signed int i = __builtin_va_arg(va,signed int);
            n64 = (unsigned int)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (unsigned int)-i;
               fl |= 128;
            }
         }
# 1098 "include/thirdparty/stb_sprintf.h"
         s = num + 512;
         l = 0;

         for (;;) {

            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (unsigned int)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (unsigned int)n64;
               n64 = 0;
            }
            if ((fl & 64) == 0) {
               do {
                  s -= 2;
                  *(unsigned short *)s = *(unsigned short *)&stbsp__digitpair.pair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & 64) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + 512)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & 64) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);


         l = (unsigned int)((num + 512) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:

         if (pr < (signed int)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (signed int)n)
            fw = n;
         fw -= n;
         pr -= l;


         if ((fl & 1) == 0) {
            if (fl & 16)
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~64;
            }
         }


         if (fw + pr) {
            signed int i;
            unsigned int c;


            if ((fl & 1) == 0)
               while (fw > 0) {
                  i = fw; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
                  fw -= i;
                  while (i) {
                     if ((((unsigned long long)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(unsigned int *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
               }


            sn = lead + 1;
            while (lead[0]) {
               i = lead[0]; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
            }


            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & 64) ? ((unsigned int)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               i = pr; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
               pr -= i;
               if ((fl & 64) == 0) {
                  while (i) {
                     if ((((unsigned long long)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(unsigned int *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & 64) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
            }
         }


         sn = lead + 1;
         while (lead[0]) {
            signed int i;
            i = lead[0]; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         n = l;
         while (n) {
            signed int i;
            i = n; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            n -= i;
            while (i >= 4) { *(unsigned int volatile *)bf = *(unsigned int volatile *)s; bf += 4; s += 4; i -= 4; }





            while (i) {
               *bf++ = *s++;
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         while (tz) {
            signed int i;
            i = tz; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            tz -= i;
            while (i) {
               if ((((unsigned long long)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(unsigned int *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         sn = tail + 1;
         while (tail[0]) {
            signed int i;
            i = tail[0]; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
         }


         if (fl & 1)
            if (fw > 0) {
               while (fw) {
                  signed int i;
                  i = fw; if (callback) { int lg = 512 - (int)(bf - buf); if (i > lg) i = lg; };
                  fw -= i;
                  while (i) {
                     if ((((unsigned long long)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(unsigned int *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  { if (callback) { { int len = (int)(bf - buf); if ((len + (1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; } };
               }
            }
         break;

      default:
         s = num + 512 - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;

         dp = 0;

         cs = 0;
         goto scopy;
      }
      ++f;
   }
endfmt:

   if (!callback)
      *bf = 0;
   else
      { { int len = (int)(bf - buf); if ((len + (512 - 1)) >= 512) { tlen += len; if (0 == (bf = buf = callback(buf, user, len))) goto done; } }; };

done:
   return tlen + (int)(bf - buf);
}
# 1388 "include/thirdparty/stb_sprintf.h"
 int stbsp_sprintf(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   __builtin_va_start(va,fmt);
   result = stbsp_vsprintfcb(0, 0, buf, fmt, va);
   __builtin_va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   int length;
   char tmp[512];
} stbsp__context;

static char *stbsp__clamp_callback(const char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;
   c->length += len;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         const char *s, *se;
         char *d;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return c->tmp;
   return (c->count >= 512) ? c->buf : c->tmp;
}

static char * stbsp__count_clamp_callback( const char * buf, void * user, int len )
{
   stbsp__context * c = (stbsp__context*)user;
   (void) sizeof(buf);

   c->length += len;
   return c->tmp;
}

 int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;

   if ( (count == 0) && !buf )
   {
      c.length = 0;

      stbsp_vsprintfcb( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
   }
   else
   {
      int l;

      c.buf = buf;
      c.count = count;
      c.length = 0;

      stbsp_vsprintfcb( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );


      l = (int)( c.buf - buf );
      if ( l >= count )
         l = count - 1;
      buf[l] = 0;
   }

   return c.length;
}

 int stbsp_snprintf(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   __builtin_va_start(va,fmt);

   result = stbsp_vsnprintf(buf, count, fmt, va);
   __builtin_va_end(va);

   return result;
}

 int stbsp_vsprintf(char *buf, char const *fmt, va_list va)
{
   return stbsp_vsprintfcb(0, 0, buf, fmt, va);
}
# 10 "libcxx/stb.c" 2
