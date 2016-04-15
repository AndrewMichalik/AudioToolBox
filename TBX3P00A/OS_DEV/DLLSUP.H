/************************************************************************/
/* Windows DLL Support Defs: DLLSup.c                   V2.00  05/20/93 */
/* Copyright (c) 1987-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/

///************************************************************************/
///*                      DLL Support Functions                           */
///************************************************************************/
//VISMEMHDL   _fGloAloBlk (WORD, DWORD, DWORD, DWORD, DWORD FAR *);
//LPVOID      _fGloDupLck (WORD, VISMEMHDL FAR *); 

/************************************************************************/
/************************************************************************/
char FAR *  _ffcvt  (double, int, int FAR *, int FAR *);
char FAR *  _fitoa  (int, char FAR *, int);
char FAR *  _fultoa (unsigned long, char FAR *, int);
float       _fatof  (char FAR *);
int         _fatoi   (char FAR *);
long        _fatol  (char FAR *);


