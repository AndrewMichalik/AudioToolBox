/************************************************************************/
/* Generic Windows Memory Support Defs: WinMem.h        V2.00  07/20/93 */
/* Copyright (c) 1987-1992 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
typedef int (far pascal *MSGDSPPRC) (const char *, void *);
extern  MSGDSPPRC   MsgDspPrc;

/************************************************************************/
/************************************************************************/
unsigned short cdecl        MsgDspUsr (const char *, ...);
unsigned short cdecl        MsgAskUsr (const char *, ...);

unsigned short far pascal   MsgBegPol (void far *);
unsigned short far pascal   MsgDspPol (void far *, unsigned long, unsigned long);
unsigned short far pascal   MsgEndPol (void far *);

