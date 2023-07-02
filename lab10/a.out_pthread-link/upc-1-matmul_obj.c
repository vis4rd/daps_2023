/* --- UPCR system headers --- */ 
#include "upcr.h" 
#include "whirl2c.h"
#include "upcr_proxy.h"
/*******************************************************
 * C file translated from WHIRL Sat Jun 10 12:17:49 2023
 *******************************************************/

/* UPC Runtime specification expected: 3.6 */
#define UPCR_WANT_MAJOR 3
#define UPCR_WANT_MINOR 6
/* UPC translator version: release 2.28.4, built on May  8 2023 at 12:26:58, host stud204-06 linux-x86_64/64, gcc v10.2.1 20210110 */
/* Included code from the initialization script */
#include</home/nfs/berkeley_upc-2022.10.0/opt/include/upcr_config.h>
#include</home/nfs/berkeley_upc-2022.10.0/opt/include/gasnet_portable_platform.h>
#include</home/nfs/berkeley_upc-2022.10.0/opt/include/upcr_preinclude/upc_types.h>
#include "upcr_geninclude/stddef.h"
#include</home/nfs/berkeley_upc-2022.10.0/opt/include/upcr_preinclude/upc_bits.h>
#include "upcr_geninclude/stdlib.h"
#include "upcr_geninclude/inttypes.h"
#include "upcr_geninclude/stdio.h"
#line 1 "upc-1-matmul.w2c.h"
/* Include builtin types and operators */

#ifndef UPCR_TRANS_EXTRA_INCL
#define UPCR_TRANS_EXTRA_INCL
extern int upcrt_gcd (int _a, int _b);
extern int _upcrt_forall_start(int _start_thread, int _step, int _lo, int _scale);
#define upcrt_forall_start(start_thread, step, lo, scale)  \
       _upcrt_forall_start(start_thread, step, lo, scale)
int32_t UPCR_TLD_DEFINE_TENTATIVE(upcrt_forall_control, 4, 4);
#define upcr_forall_control upcrt_forall_control
#ifndef UPCR_EXIT_FUNCTION
#define UPCR_EXIT_FUNCTION() ((void)0)
#endif
#if UPCR_RUNTIME_SPEC_MAJOR > 3 || (UPCR_RUNTIME_SPEC_MAJOR == 3 && UPCR_RUNTIME_SPEC_MINOR >= 8)
  #define UPCRT_STARTUP_SHALLOC(sptr, blockbytes, numblocks, mult_by_threads, elemsz, typestr) \
      { &(sptr), (blockbytes), (numblocks), (mult_by_threads), (elemsz), #sptr, (typestr) }
#else
  #define UPCRT_STARTUP_SHALLOC(sptr, blockbytes, numblocks, mult_by_threads, elemsz, typestr) \
      { &(sptr), (blockbytes), (numblocks), (mult_by_threads) }
#endif
#define UPCRT_STARTUP_PSHALLOC UPCRT_STARTUP_SHALLOC

/**** Autonb optimization ********/

extern void _upcrt_memput_nb(upcr_shared_ptr_t _dst, const void *_src, size_t _n);
#define upcrt_memput_nb(dst, src, n) \
       (upcri_srcpos(), _upcrt_memput_nb(dst, src, n))

#endif


/* Types */
/* File-level vars and routines */
extern int user_main();


#define UPCR_SHARED_SIZE_ 8
#define UPCR_PSHARED_SIZE_ 8
upcr_shared_ptr_t a;
upcr_shared_ptr_t c;
upcr_pshared_ptr_t b;

void UPCRI_ALLOC_upc_1_matmul_14692469224153583903(void) { 
UPCR_BEGIN_FUNCTION();
upcr_startup_shalloc_t _bupc_info[] = { 
UPCRT_STARTUP_SHALLOC(a, 64, 2, 1, 4, "A2H_A16_R16_i"), 
UPCRT_STARTUP_SHALLOC(c, 64, 2, 1, 4, "A2H_A16_R16_i"), 
 };
upcr_startup_pshalloc_t _bupc_pinfo[] = { 
UPCRT_STARTUP_PSHALLOC(b, 4, 32, 1, 4, "A16_A2H_R1_i"), 
 };

UPCR_SET_SRCPOS("_upc_1_matmul_14692469224153583903_ALLOC",0);
upcr_startup_shalloc(_bupc_info, sizeof(_bupc_info) / sizeof(upcr_startup_shalloc_t));
upcr_startup_pshalloc(_bupc_pinfo, sizeof(_bupc_pinfo) / sizeof(upcr_startup_pshalloc_t));
}

void UPCRI_INIT_upc_1_matmul_14692469224153583903(void) { 
UPCR_BEGIN_FUNCTION();
UPCR_SET_SRCPOS("_upc_1_matmul_14692469224153583903_INIT",0);
}

#line 52 "upc-1-matmul.c"
extern int user_main()
#line 52 "upc-1-matmul.c"
{
#line 52 "upc-1-matmul.c"
  UPCR_BEGIN_FUNCTION();
  register _INT32 _bupc_comma;
  register _INT32 _bupc_comma0;
  register _INT32 _bupc_comma1;
  register _UINT64 _bupc_comma2;
  int i;
  int j;
  int sum;
  int k;
  upcr_shared_ptr_t _bupc_Mptra0;
  upcr_shared_ptr_t _bupc_Mptra1;
  upcr_shared_ptr_t _bupc_Mptra2;
  upcr_shared_ptr_t _bupc_Mptra3;
  upcr_shared_ptr_t _bupc_Mptra4;
  upcr_pshared_ptr_t _bupc_Mptra5;
  upcr_pshared_ptr_t _bupc_Mptra6;
  upcr_pshared_ptr_t _bupc_Mptra7;
  upcr_pshared_ptr_t _bupc_Mptra8;
  upcr_pshared_ptr_t _bupc_Mptra9;
  upcr_pshared_ptr_t _bupc_Mptra10;
  upcr_shared_ptr_t _bupc_Mptra11;
  upcr_shared_ptr_t _bupc_Mptra12;
  int _bupc_spillld13;
  upcr_pshared_ptr_t _bupc_Mptra14;
  upcr_pshared_ptr_t _bupc_Mptra15;
  int _bupc_spillld16;
  upcr_shared_ptr_t _bupc_Mptra17;
  upcr_shared_ptr_t _bupc_Mptra18;
  upcr_shared_ptr_t _bupc_Mptra19;
  upcr_shared_ptr_t _bupc_Mptra20;
  upcr_shared_ptr_t _bupc_Mptra21;
  int _bupc_spillld22;
  upcr_pshared_ptr_t _bupc_Mptra23;
  upcr_pshared_ptr_t _bupc_Mptra24;
  int _bupc_spillld25;
  upcr_shared_ptr_t _bupc_Mptra26;
  upcr_shared_ptr_t _bupc_Mptra27;
  upcr_shared_ptr_t _bupc_Mptra28;
  upcr_shared_ptr_t _bupc_Mptra29;
  upcr_shared_ptr_t _bupc_Mptra30;
  upcr_shared_ptr_t _bupc_Mptra31;
  int _bupc_spillld32;
  
#line 57 "upc-1-matmul.c"
  if((*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))))
#line 57 "upc-1-matmul.c"
  {
#line 57 "upc-1-matmul.c"
    i = 0;
#line 57 "upc-1-matmul.c"
    while(i < (((int) upcr_threads () ) * 2))
    {
#line 59 "upc-1-matmul.c"
      j = 0;
#line 59 "upc-1-matmul.c"
      while(j <= 15)
#line 59 "upc-1-matmul.c"
      {
#line 61 "upc-1-matmul.c"
        _bupc_Mptra0 = UPCR_ADD_SHARED(a, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 61 "upc-1-matmul.c"
        _bupc_Mptra1 = UPCR_ADD_SHARED(_bupc_Mptra0, 4ULL, j, 16ULL);
#line 61 "upc-1-matmul.c"
        UPCR_PUT_SHARED_VAL(_bupc_Mptra1, 0, (i + j) + 1, 4);
#line 62 "upc-1-matmul.c"
        _38 :;
#line 62 "upc-1-matmul.c"
        j = j + 1;
      }
#line 63 "upc-1-matmul.c"
      _39 :;
#line 63 "upc-1-matmul.c"
      i = i + 1;
    }
  }
  else
#line 57 "upc-1-matmul.c"
  {
#line 57 "upc-1-matmul.c"
    (*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))) = 1;
#line 57 "upc-1-matmul.c"
    i = 0;
#line 57 "upc-1-matmul.c"
    while(i < (((int) upcr_threads () ) * 2))
#line 57 "upc-1-matmul.c"
    {
      _bupc_Mptra2 = UPCR_ADD_SHARED(a, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
      _bupc_comma = upcr_hasMyAffinity_shared(_bupc_Mptra2);
      if(_bupc_comma != 0)
      {
#line 59 "upc-1-matmul.c"
        j = 0;
#line 59 "upc-1-matmul.c"
        while(j <= 15)
#line 59 "upc-1-matmul.c"
        {
#line 61 "upc-1-matmul.c"
          _bupc_Mptra3 = UPCR_ADD_SHARED(a, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 61 "upc-1-matmul.c"
          _bupc_Mptra4 = UPCR_ADD_SHARED(_bupc_Mptra3, 4ULL, j, 16ULL);
#line 61 "upc-1-matmul.c"
          UPCR_PUT_SHARED_VAL(_bupc_Mptra4, 0, (i + j) + 1, 4);
#line 62 "upc-1-matmul.c"
          _2 :;
#line 62 "upc-1-matmul.c"
          j = j + 1;
        }
      }
#line 63 "upc-1-matmul.c"
      _1 :;
#line 63 "upc-1-matmul.c"
      i = i + 1;
    }
#line 57 "upc-1-matmul.c"
    (*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))) = 0;
  }
#line 65 "upc-1-matmul.c"
  i = 0;
#line 65 "upc-1-matmul.c"
  while(i <= 15)
#line 65 "upc-1-matmul.c"
  {
#line 67 "upc-1-matmul.c"
    if((*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))))
#line 67 "upc-1-matmul.c"
    {
#line 67 "upc-1-matmul.c"
      j = 0;
#line 67 "upc-1-matmul.c"
      while(j <= 15)
      {
#line 69 "upc-1-matmul.c"
        _bupc_Mptra5 = UPCR_ADD_PSHARED1(b, 4ULL, (_INT64)(((_UINT64)(((int) upcr_threads () )) * (_UINT64)(j)) * 2ULL));
#line 69 "upc-1-matmul.c"
        _bupc_Mptra6 = UPCR_ADD_PSHARED1(_bupc_Mptra5, 4ULL, i);
#line 69 "upc-1-matmul.c"
        UPCR_PUT_PSHARED_VAL(_bupc_Mptra6, 0, (j - i) + (225 / (j + 1)), 4);
#line 70 "upc-1-matmul.c"
        _40 :;
#line 70 "upc-1-matmul.c"
        j = j + 1;
      }
    }
    else
#line 67 "upc-1-matmul.c"
    {
#line 67 "upc-1-matmul.c"
      (*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))) = 1;
#line 67 "upc-1-matmul.c"
      j = 0;
#line 67 "upc-1-matmul.c"
      while(j <= 15)
#line 67 "upc-1-matmul.c"
      {
        _bupc_Mptra7 = UPCR_ADD_PSHARED1(b, 4ULL, (_INT64)(((_UINT64)(((int) upcr_threads () )) * (_UINT64)(i)) * 2ULL));
        _bupc_Mptra8 = UPCR_ADD_PSHARED1(_bupc_Mptra7, 4ULL, j);
        _bupc_comma0 = upcr_hasMyAffinity_pshared(_bupc_Mptra8);
        if(_bupc_comma0 != 0)
        {
#line 69 "upc-1-matmul.c"
          _bupc_Mptra9 = UPCR_ADD_PSHARED1(b, 4ULL, (_INT64)(((_UINT64)(((int) upcr_threads () )) * (_UINT64)(j)) * 2ULL));
#line 69 "upc-1-matmul.c"
          _bupc_Mptra10 = UPCR_ADD_PSHARED1(_bupc_Mptra9, 4ULL, i);
#line 69 "upc-1-matmul.c"
          UPCR_PUT_PSHARED_VAL(_bupc_Mptra10, 0, (j - i) + (225 / (j + 1)), 4);
        }
#line 70 "upc-1-matmul.c"
        _4 :;
#line 70 "upc-1-matmul.c"
        j = j + 1;
      }
#line 67 "upc-1-matmul.c"
      (*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))) = 0;
    }
#line 71 "upc-1-matmul.c"
    _3 :;
#line 71 "upc-1-matmul.c"
    i = i + 1;
  }
#line 73 "upc-1-matmul.c"
  upcr_barrier(1393070367, 1);
#line 97 "upc-1-matmul.c"
  if((*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))))
#line 97 "upc-1-matmul.c"
  {
#line 97 "upc-1-matmul.c"
    i = 0;
#line 97 "upc-1-matmul.c"
    while(i < (((int) upcr_threads () ) * 2))
    {
#line 99 "upc-1-matmul.c"
      j = 0;
#line 99 "upc-1-matmul.c"
      while(j <= 15)
#line 99 "upc-1-matmul.c"
      {
#line 101 "upc-1-matmul.c"
        sum = 0;
#line 102 "upc-1-matmul.c"
        k = 0;
#line 102 "upc-1-matmul.c"
        while(k <= 15)
#line 102 "upc-1-matmul.c"
        {
#line 103 "upc-1-matmul.c"
          _bupc_Mptra11 = UPCR_ADD_SHARED(a, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 103 "upc-1-matmul.c"
          _bupc_Mptra12 = UPCR_ADD_SHARED(_bupc_Mptra11, 4ULL, k, 16ULL);
#line 103 "upc-1-matmul.c"
          UPCR_GET_SHARED(&_bupc_spillld13, _bupc_Mptra12, 0, 4);
#line 103 "upc-1-matmul.c"
          _bupc_Mptra14 = UPCR_ADD_PSHARED1(b, 4ULL, (_INT64)(((_UINT64)(((int) upcr_threads () )) * (_UINT64)(k)) * 2ULL));
#line 103 "upc-1-matmul.c"
          _bupc_Mptra15 = UPCR_ADD_PSHARED1(_bupc_Mptra14, 4ULL, j);
#line 103 "upc-1-matmul.c"
          UPCR_GET_PSHARED(&_bupc_spillld16, _bupc_Mptra15, 0, 4);
#line 103 "upc-1-matmul.c"
          sum = sum + (_bupc_spillld13 * _bupc_spillld16);
#line 103 "upc-1-matmul.c"
          _41 :;
#line 103 "upc-1-matmul.c"
          k = k + 1;
        }
#line 104 "upc-1-matmul.c"
        _bupc_Mptra17 = UPCR_ADD_SHARED(c, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 104 "upc-1-matmul.c"
        _bupc_Mptra18 = UPCR_ADD_SHARED(_bupc_Mptra17, 4ULL, j, 16ULL);
#line 104 "upc-1-matmul.c"
        UPCR_PUT_SHARED_VAL(_bupc_Mptra18, 0, sum, 4);
#line 105 "upc-1-matmul.c"
        _42 :;
#line 105 "upc-1-matmul.c"
        j = j + 1;
      }
#line 106 "upc-1-matmul.c"
      _43 :;
#line 106 "upc-1-matmul.c"
      i = i + 1;
    }
  }
  else
#line 97 "upc-1-matmul.c"
  {
#line 97 "upc-1-matmul.c"
    (*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))) = 1;
#line 97 "upc-1-matmul.c"
    i = 0;
#line 97 "upc-1-matmul.c"
    while(i < (((int) upcr_threads () ) * 2))
#line 97 "upc-1-matmul.c"
    {
      _bupc_Mptra19 = UPCR_ADD_SHARED(a, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
      _bupc_comma1 = upcr_hasMyAffinity_shared(_bupc_Mptra19);
      if(_bupc_comma1 != 0)
      {
#line 99 "upc-1-matmul.c"
        j = 0;
#line 99 "upc-1-matmul.c"
        while(j <= 15)
#line 99 "upc-1-matmul.c"
        {
#line 101 "upc-1-matmul.c"
          sum = 0;
#line 102 "upc-1-matmul.c"
          k = 0;
#line 102 "upc-1-matmul.c"
          while(k <= 15)
#line 102 "upc-1-matmul.c"
          {
#line 103 "upc-1-matmul.c"
            _bupc_Mptra20 = UPCR_ADD_SHARED(a, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 103 "upc-1-matmul.c"
            _bupc_Mptra21 = UPCR_ADD_SHARED(_bupc_Mptra20, 4ULL, k, 16ULL);
#line 103 "upc-1-matmul.c"
            UPCR_GET_SHARED(&_bupc_spillld22, _bupc_Mptra21, 0, 4);
#line 103 "upc-1-matmul.c"
            _bupc_Mptra23 = UPCR_ADD_PSHARED1(b, 4ULL, (_INT64)(((_UINT64)(((int) upcr_threads () )) * (_UINT64)(k)) * 2ULL));
#line 103 "upc-1-matmul.c"
            _bupc_Mptra24 = UPCR_ADD_PSHARED1(_bupc_Mptra23, 4ULL, j);
#line 103 "upc-1-matmul.c"
            UPCR_GET_PSHARED(&_bupc_spillld25, _bupc_Mptra24, 0, 4);
#line 103 "upc-1-matmul.c"
            sum = sum + (_bupc_spillld22 * _bupc_spillld25);
#line 103 "upc-1-matmul.c"
            _7 :;
#line 103 "upc-1-matmul.c"
            k = k + 1;
          }
#line 104 "upc-1-matmul.c"
          _bupc_Mptra26 = UPCR_ADD_SHARED(c, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 104 "upc-1-matmul.c"
          _bupc_Mptra27 = UPCR_ADD_SHARED(_bupc_Mptra26, 4ULL, j, 16ULL);
#line 104 "upc-1-matmul.c"
          UPCR_PUT_SHARED_VAL(_bupc_Mptra27, 0, sum, 4);
#line 105 "upc-1-matmul.c"
          _6 :;
#line 105 "upc-1-matmul.c"
          j = j + 1;
        }
      }
#line 106 "upc-1-matmul.c"
      _5 :;
#line 106 "upc-1-matmul.c"
      i = i + 1;
    }
#line 97 "upc-1-matmul.c"
    (*((_INT32 *) UPCR_TLD_ADDR( upcr_forall_control ))) = 0;
  }
#line 108 "upc-1-matmul.c"
  upcr_barrier(1393070368, 1);
#line 111 "upc-1-matmul.c"
  if(((int) upcr_mythread () ) == 0)
#line 111 "upc-1-matmul.c"
  {
#line 113 "upc-1-matmul.c"
    i = 0;
#line 113 "upc-1-matmul.c"
    while(i < (((int) upcr_threads () ) * 2))
#line 113 "upc-1-matmul.c"
    {
#line 115 "upc-1-matmul.c"
      printf("\n");
#line 116 "upc-1-matmul.c"
      j = 0;
#line 116 "upc-1-matmul.c"
      while(j <= 15)
#line 116 "upc-1-matmul.c"
      {
#line 118 "upc-1-matmul.c"
        if(((_UINT32)((j + (i * 16))) & 3U) == 0U)
#line 118 "upc-1-matmul.c"
        {
#line 119 "upc-1-matmul.c"
          _bupc_Mptra28 = UPCR_ADD_SHARED(c, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 119 "upc-1-matmul.c"
          _bupc_Mptra29 = UPCR_ADD_SHARED(_bupc_Mptra28, 4ULL, j, 16ULL);
#line 119 "upc-1-matmul.c"
          _bupc_comma2 = UPCR_THREADOF_SHARED(_bupc_Mptra29);
#line 119 "upc-1-matmul.c"
          printf("\nThread\t%d: ", (int)(_INT32)(_bupc_comma2));
        }
#line 120 "upc-1-matmul.c"
        _bupc_Mptra30 = UPCR_ADD_SHARED(c, 4ULL, (_INT64)((_UINT64)(i) * 16ULL), 16ULL);
#line 120 "upc-1-matmul.c"
        _bupc_Mptra31 = UPCR_ADD_SHARED(_bupc_Mptra30, 4ULL, j, 16ULL);
#line 120 "upc-1-matmul.c"
        UPCR_GET_SHARED(&_bupc_spillld32, _bupc_Mptra31, 0, 4);
#line 120 "upc-1-matmul.c"
        printf(" c[%d][%d] = %d \t", i, j, _bupc_spillld32);
#line 121 "upc-1-matmul.c"
        _9 :;
#line 121 "upc-1-matmul.c"
        j = j + 1;
      }
#line 122 "upc-1-matmul.c"
      _8 :;
#line 122 "upc-1-matmul.c"
      i = i + 1;
    }
#line 124 "upc-1-matmul.c"
    printf("\nSuccess: application did not crash\n");
  }
#line 127 "upc-1-matmul.c"
  UPCR_EXIT_FUNCTION();
#line 127 "upc-1-matmul.c"
  return 0;
} /* user_main */

#line 1 "_SYSTEM"
/**************************************************************************/
/* upcc-generated strings for configuration consistency checks            */

GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_GASNetConfig_gen, 
 "$GASNetConfig: (/tmp/upcc-9kluczka-328144-1686392268/upc-1-matmul.trans.c) RELEASE=2022.9.0,SPEC=0.16,CONDUIT=UDP(UDP-3.19/REFERENCE-2022.9),THREADMODEL=PAR,SEGMENT=FAST,PTR=64bit,CACHE_LINE_BYTES=64,noalign,pshm,nodebug,notrace,nostats,nodebugmalloc,nosrclines,timers_native,membars_native,atomics_native,atomic32_native,atomic64_native,notiopt $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_UPCRConfig_gen,
 "$UPCRConfig: (/tmp/upcc-9kluczka-328144-1686392268/upc-1-matmul.trans.c) VERSION=2022.10.0,PLATFORMENV=shared-distributed,SHMEM=pthreads/pshm,SHAREDPTRREP=packed/p20:t10:a34,TRANS=berkeleyupc,nodebug,nogasp,dynamicthreads $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_translatetime, 
 "$UPCTranslateTime: (upc-1-matmul.o) Sat Jun 10 12:17:49 2023 $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_GASNetConfig_obj, 
 "$GASNetConfig: (upc-1-matmul.o) " GASNET_CONFIG_STRING " $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_UPCRConfig_obj,
 "$UPCRConfig: (upc-1-matmul.o) " UPCR_CONFIG_STRING UPCRI_THREADCONFIG_STR " $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_translator, 
 "$UPCTranslator: (upc-1-matmul.o) /opt/nfs/berkeley_upc_translator-2.28.4/targ (stud204-06) $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_upcver, 
 "$UPCVersion: (upc-1-matmul.o) 2022.10.0 $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_compileline, 
 "$UPCCompileLine: (upc-1-matmul.o) /home/nfs/berkeley_upc-2022.10.0/opt/bin/upcc.pl -Wc,-O2 -network=udp -pthreads=4 upc-1-matmul.c -vvv $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_compiletime, 
 "$UPCCompileTime: (upc-1-matmul.o) " __DATE__ " " __TIME__ " $");
#ifndef UPCRI_CC /* ensure backward compatilibity for http netcompile */
#define UPCRI_CC <unknown>
#endif
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_backendcompiler, 
 "$UPCRBackendCompiler: (upc-1-matmul.o) " _STRINGIFY(UPCRI_CC) " $");

#ifdef GASNETT_CONFIGURE_MISMATCH
  GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_configuremismatch, 
   "$UPCRConfigureMismatch: (upc-1-matmul.o) 1 $");
  GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_configuredcompiler, 
   "$UPCRConfigureCompiler: (upc-1-matmul.o) " GASNETT_PLATFORM_COMPILER_IDSTR " $");
  GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_buildcompiler, 
   "$UPCRBuildCompiler: (upc-1-matmul.o) " PLATFORM_COMPILER_IDSTR " $");
#endif

/**************************************************************************/
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_transver_2,
 "$UPCTranslatorVersion: (upc-1-matmul.o) 2.28.4, built on May  8 2023 at 12:26:58, host stud204-06 linux-x86_64/64, gcc v10.2.1 20210110 $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_shareddata_a,
 "$UPCRStaticSharedData: (a) 64 2 1 4 A2H_A16_R16_i $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_shareddata_c,
 "$UPCRStaticSharedData: (c) 64 2 1 4 A2H_A16_R16_i $");
GASNETT_IDENT(UPCRI_IdentString_upc_1_matmul_o_1686392269_shareddata_b,
 "$UPCRStaticSharedData: (b) 4 32 1 4 A16_A2H_R1_i $");
GASNETT_IDENT(UPCRI_IdentString_INIT_upc_1_matmul_14692469224153583903,"$UPCRInitFn: (upc-1-matmul.trans.c) UPCRI_INIT_upc_1_matmul_14692469224153583903 $");
GASNETT_IDENT(UPCRI_IdentString_ALLOC_upc_1_matmul_14692469224153583903,"$UPCRAllocFn: (upc-1-matmul.trans.c) UPCRI_ALLOC_upc_1_matmul_14692469224153583903 $");
