/************************************************************************/
/* VISI Standard Type and Conversion Defs: VISDef.h     V2.10  10/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#if !(defined (BOOL) || defined (BYTE) || defined (WORD) || defined (DWORD))
    typedef int             BOOL;
    typedef unsigned char   BYTE;
    typedef unsigned short  WORD;
    typedef unsigned long   DWORD;
    typedef unsigned int    UINT;
#endif
#if !(defined (FAR) || defined (PASCAL) || defined (LPSTR) || defined (LPVOID))
    #define FAR             far
    #define PASCAL          pascal
    typedef char far        *LPSTR;
    typedef const char far  *LPCSTR;
    typedef void far        *LPVOID;
#endif
#if !(defined (LOBYTE) || defined (HIBYTE))
    #define LOBYTE(w)       ((unsigned char)(w))
    #define HIBYTE(w)       ((unsigned char)(((WORD)(w) >> 8) & 0xff))
    #define LOWORD(l)	    ((WORD)(l))
    #define HIWORD(l)	    ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
    #define MAKELONG(a, b)	((long)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#endif
#if !(defined (FALSE) || defined (TRUE))
    #define FALSE           0
    #define TRUE            1
#endif

/************************************************************************/
/*                  C to Assembly interface macros                      */
/************************************************************************/
#if !(defined (CPUSHA) || defined (CPOPA))
    #define CPUSHA _asm {_asm pushf  _asm push di _asm push si _asm push ds}
    #define CPOPA  _asm {_asm pop ds _asm pop  si _asm pop  di _asm popf}
#endif

/************************************************************************/
/*                  8086/80286 shift instruction macros                 */
/************************************************************************/
#if (defined (M_I286))
    #define ROL2(x) _asm {_asm rol x,2}
    #define ROL3(x) _asm {_asm rol x,3}
    #define ROL4(x) _asm {_asm rol x,4}
    #define ROL5(x) _asm {_asm rol x,5}
    #define SAL2(x) _asm {_asm sal x,2}
    #define SAL3(x) _asm {_asm sal x,3}
    #define SAL4(x) _asm {_asm sal x,4}
    #define SAL5(x) _asm {_asm sal x,5}
    #define SHL2(x) _asm {_asm shl x,2}
    #define SHL3(x) _asm {_asm shl x,3}
    #define SHL4(x) _asm {_asm shl x,4}
    #define SHL5(x) _asm {_asm shl x,5}
    #define SAR2(x) _asm {_asm sar x,2}
    #define SAR3(x) _asm {_asm sar x,3}
    #define SAR4(x) _asm {_asm sar x,4}
    #define SAR5(x) _asm {_asm sar x,5}
    #define SHR2(x) _asm {_asm shr x,2}
    #define SHR3(x) _asm {_asm shr x,3}
    #define SHR4(x) _asm {_asm shr x,4}
    #define SHR5(x) _asm {_asm shr x,5}
#else
    #define ROL2(x) _asm {_asm rol x,1 _asm rol x,1}
    #define ROL3(x) _asm {_asm rol x,1 _asm rol x,1 _asm rol x,1}
    #define ROL4(x) ROL2(x) ROL2(x)
    #define ROL5(x) ROL2(x) ROL3(x)
    #define SAL2(x) _asm {_asm sal x,1 _asm sal x,1}
    #define SAL3(x) _asm {_asm sal x,1 _asm sal x,1 _asm sal x,1}
    #define SAL4(x) SAL2(x) SAL2(x)
    #define SAL5(x) SAL2(x) SAL3(x)
    #define SHL2(x) _asm {_asm shl x,1 _asm shl x,1}
    #define SHL3(x) _asm {_asm shl x,1 _asm shl x,1 _asm shl x,1}
    #define SHL4(x) SHL2(x) SHL2(x)
    #define SHL5(x) SHL2(x) SHL3(x)
    #define SAR2(x) _asm {_asm sar x,1 _asm sar x,1}
    #define SAR3(x) _asm {_asm sar x,1 _asm sar x,1 _asm sar x,1}
    #define SAR4(x) SAR2(x) SAR2(x)
    #define SAR5(x) SAR2(x) SAR3(x)
    #define SHR2(x) _asm {_asm shr x,1 _asm shr x,1}
    #define SHR3(x) _asm {_asm shr x,1 _asm shr x,1 _asm shr x,1}
    #define SHR4(x) SHR2(x) SHR2(x)
    #define SHR5(x) SHR2(x) SHR3(x)
#endif

