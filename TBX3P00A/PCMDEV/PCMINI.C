/************************************************************************/
/* PCM DLL Initialization Support: PCMIni.c             V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "genpcm.h"                     /* PCM/APCM conversion defs     */
#include "pcmsup.h"                     /* PCM/APCM xlate lib defs      */

#include <string.h>                     /* String manipulation funcs    */

/************************************************************************/
/************************************************************************/
void    ulMemSet (LPVOID, DWORD, WORD);
void    flMemSet (LPVOID, float, WORD);

/************************************************************************/
/*                  Conversion Routine References                       */
/************************************************************************/
SETSILPRC   fpSetSil[MAXPCM000];
ALOITCPRC   fpAloITC[MAXPCM000];
INIITCPRC   fpIniITC[MAXPCM000];
CMPITCPRC   fpCmpITC[MAXPCM000];
RELITCPRC   fpRelITC[MAXPCM000];
PCMTOGPRC   fpPCMToG[MAXPCM000];
GTOPCMPRC   fpGenToP[MAXPCM000];
PCMTOMPRC   fpPCMToM[MAXPCM000];

float       flByPSmp[MAXPCM000];
DWORD       ulFrqDef[MAXPCM000];
DWORD       ulAtDRes[MAXPCM000];
DWORD       ulGrdSmp[MAXPCM000];
WORD        usBlkSiz[MAXPCM000];
WORD        usViwFtr[MAXPCM000];

/************************************************************************/
/*                      Null Conversion Routines                        */
/************************************************************************/
LPITCB  FAR PASCAL NulPCMtoAlo (LPITCB lpITC, LPVOID lpITCPrm)
           { return ((LPITCB) lpITC); }
LPITCB  FAR PASCAL NulPCMtoIni (LPITCB lpITC)
           { return ((LPITCB) lpITC); }
WORD    FAR PASCAL NulPCMtoCmp (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
           WORD FAR *lpusRsv001, WORD usSmpCnt, LPITCB lpITC)
           { return (usSmpCnt); }
LPITCB  FAR PASCAL NulPCMtoRel (LPITCB lpITC)
           { return ((LPITCB) lpITC); }
WORD    FAR PASCAL NulPCMtoG16 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
           WORD FAR *lpusSrcByt, GENBASPTR psDstBuf, WORD usSmpCnt, LPITCB lpITC)
           { return (usSmpCnt); }
WORD    FAR PASCAL NulG16toPCM (_segment _sBufSeg, GENBASPTR psSrcBuf,
           WORD FAR *lpusSrcByt, BYTBASPTR pcDstBuf, WORD usSmpCnt, LPITCB lpITC)
           { return (*lpusSrcByt); }
WORD    FAR PASCAL NulPCMtoM32 (_segment _sBufSeg, BYTBASPTR pcSrcBuf,
           WORD FAR *lpusSrcByt, GENBASPTR psDstBuf, WORD usOscCnt, DWORD ulSetSiz, LPITCB lpITC)
           { return (usOscCnt); }
WORD    FAR PASCAL NulSiltoPCM (BYTE FAR * lpcDstBuf, WORD usBufSiz)
           { return (usBufSiz); }

/************************************************************************/
/************************************************************************/
WORD PCMResIni (HANDLE hLibIns)
{

    /********************************************************************/
    /* Initialize set to silence function array                         */
    /********************************************************************/
    _fmemset (fpSetSil, 0, sizeof (fpSetSil));
    fpSetSil[UNKPCM000] = NulSiltoPCM;
    fpSetSil[AVAPCM004] = NulSiltoPCM;
    fpSetSil[BKTPCM001] = NulSiltoPCM;
    fpSetSil[CPXPCM064] = NulSiltoPCM;
    fpSetSil[DLGPCM004] = DlgSiltoA04;
    fpSetSil[DLGPCM008] = DlgSiltoF08;
    fpSetSil[FLTPCM032] = NulSiltoPCM;
    fpSetSil[G11PCMF08] = G11SiltoF08;    
    fpSetSil[G11PCMI08] = G11SiltoI08;
    fpSetSil[G21PCM004] = G21SiltoA04;    
    fpSetSil[G22PCM004] = NulSiltoPCM;    
    fpSetSil[G23PCM003] = NulSiltoPCM;
    fpSetSil[HARPCM001] = HarSiltoC01;
    fpSetSil[MPCPCM008] = MPCSiltoP08;
    fpSetSil[MPCPCM016] = MPCSiltoL16;
    fpSetSil[MSAPCM004] = NulSiltoPCM;
    fpSetSil[NWVPCM001] = NwVSiltoC01;
    fpSetSil[PTCPCM001] = PTCSiltoC01;
    fpSetSil[RTXPCM003] = NulSiltoPCM;
    fpSetSil[RTXPCM004] = RtxSiltoA04;
    fpSetSil[TTIPCM008] = PL2SiltoP08;

    /********************************************************************/
    /* Initialize ITC Block allocation function array                   */
    /********************************************************************/
    _fmemset (fpAloITC, 0, sizeof (fpAloITC));
    fpAloITC[UNKPCM000] = NulPCMtoAlo;
    fpAloITC[AVAPCM004] = NulPCMtoAlo;
    fpAloITC[BKTPCM001] = NulPCMtoAlo;
    fpAloITC[CPXPCM064] = NulPCMtoAlo;
    fpAloITC[DLGPCM004] = NulPCMtoAlo;
    fpAloITC[DLGPCM008] = NulPCMtoAlo;
    fpAloITC[FLTPCM032] = NulPCMtoAlo;
    fpAloITC[G11PCMF08] = NulPCMtoAlo;    
    fpAloITC[G11PCMI08] = NulPCMtoAlo;    
    fpAloITC[G21PCM004] = NulPCMtoAlo;    
    fpAloITC[G22PCM004] = NulPCMtoAlo;    
    fpAloITC[G23PCM003] = NulPCMtoAlo;
    fpAloITC[HARPCM001] = HarC01toAlo;
    fpAloITC[MPCPCM008] = NulPCMtoAlo;
    fpAloITC[MPCPCM016] = NulPCMtoAlo;
    fpAloITC[MSAPCM004] = NulPCMtoAlo;
    fpAloITC[NWVPCM001] = NwVC01toAlo;
    fpAloITC[PTCPCM001] = PTCC01toAlo;
    fpAloITC[RTXPCM003] = NulPCMtoAlo;
    fpAloITC[RTXPCM004] = RtxA04toAlo;
    fpAloITC[TTIPCM008] = NulPCMtoAlo;

    /********************************************************************/
    /* Initialize ITC Block reset function array                        */
    /********************************************************************/
    _fmemset (fpIniITC, 0, sizeof (fpIniITC));
    fpIniITC[UNKPCM000] = NulPCMtoIni;
    fpIniITC[AVAPCM004] = NulPCMtoIni;
    fpIniITC[BKTPCM001] = NulPCMtoIni;
    fpIniITC[CPXPCM064] = NulPCMtoIni;
    fpIniITC[DLGPCM004] = DlgA04toIni;
    fpIniITC[DLGPCM008] = DlgF08toIni;
    fpIniITC[FLTPCM032] = NulPCMtoIni;
    fpIniITC[G11PCMF08] = G11F08toIni;    
    fpIniITC[G11PCMI08] = G11I08toIni;    
    fpIniITC[G21PCM004] = G21A04toIni;    
    fpIniITC[G22PCM004] = NulPCMtoIni;    
    fpIniITC[G23PCM003] = NulPCMtoIni;
    fpIniITC[HARPCM001] = HarC01toIni;
    fpIniITC[MPCPCM008] = MPCP08toIni;
    fpIniITC[MPCPCM016] = MPCL16toIni;
    fpIniITC[MSAPCM004] = MSWA04toIni;
    fpIniITC[NWVPCM001] = NwVC01toIni;
    fpIniITC[PTCPCM001] = PTCC01toIni;
    fpIniITC[RTXPCM003] = NulPCMtoIni;
    fpIniITC[RTXPCM004] = RtxA04toIni;
    fpIniITC[TTIPCM008] = PL2P08toIni;

    /********************************************************************/
    /* Initialize ITC Block compute history function array              */
    /********************************************************************/
    _fmemset (fpCmpITC, 0, sizeof (fpCmpITC));
    fpCmpITC[UNKPCM000] = NulPCMtoCmp;
    fpCmpITC[AVAPCM004] = NulPCMtoCmp;
    fpCmpITC[BKTPCM001] = NulPCMtoCmp;
    fpCmpITC[CPXPCM064] = NulPCMtoCmp;
    fpCmpITC[DLGPCM004] = DlgA04toCmp;
    fpCmpITC[DLGPCM008] = DlgF08toCmp;
    fpCmpITC[FLTPCM032] = NulPCMtoCmp;
    fpCmpITC[G11PCMF08] = G11F08toCmp;    
    fpCmpITC[G11PCMI08] = G11I08toCmp;
    fpCmpITC[G21PCM004] = G21A04toCmp;    
    fpCmpITC[G22PCM004] = NulPCMtoCmp;    
    fpCmpITC[G23PCM003] = NulPCMtoCmp;
    fpCmpITC[HARPCM001] = HarC01toCmp;
    fpCmpITC[MPCPCM008] = NulPCMtoCmp;
    fpCmpITC[MPCPCM016] = NulPCMtoCmp;
    fpCmpITC[MSAPCM004] = NulPCMtoCmp;
    fpCmpITC[NWVPCM001] = NwVC01toCmp;
    fpCmpITC[PTCPCM001] = PTCC01toCmp;
    fpCmpITC[RTXPCM003] = NulPCMtoCmp;
    fpCmpITC[RTXPCM004] = RtxA04toCmp;
    fpCmpITC[TTIPCM008] = PL2P08toCmp;

    /********************************************************************/
    /* Initialize ITC Block release allocation function array           */
    /********************************************************************/
    _fmemset (fpRelITC, 0, sizeof (fpRelITC));
    fpRelITC[UNKPCM000] = NulPCMtoRel;
    fpRelITC[AVAPCM004] = NulPCMtoRel;
    fpRelITC[BKTPCM001] = NulPCMtoRel;
    fpRelITC[CPXPCM064] = NulPCMtoRel;
    fpRelITC[DLGPCM004] = NulPCMtoRel;
    fpRelITC[DLGPCM008] = NulPCMtoRel;
    fpRelITC[FLTPCM032] = NulPCMtoRel;
    fpRelITC[G11PCMF08] = NulPCMtoRel;    
    fpRelITC[G11PCMI08] = NulPCMtoRel;    
    fpRelITC[G21PCM004] = NulPCMtoRel;    
    fpRelITC[G22PCM004] = NulPCMtoRel;    
    fpRelITC[G23PCM003] = NulPCMtoRel;
    fpRelITC[HARPCM001] = HarC01toRel;
    fpRelITC[MPCPCM008] = NulPCMtoRel;
    fpRelITC[MPCPCM016] = NulPCMtoRel;
    fpRelITC[MSAPCM004] = NulPCMtoRel;
    fpRelITC[NWVPCM001] = NwVC01toRel;
    fpRelITC[PTCPCM001] = PTCC01toRel;
    fpRelITC[RTXPCM003] = NulPCMtoRel;
    fpRelITC[RTXPCM004] = RtxA04toRel;
    fpRelITC[TTIPCM008] = NulPCMtoRel;

    /********************************************************************/
    /* Initialize PCM to generic PCM function array                     */
    /********************************************************************/
    _fmemset (fpPCMToG, 0, sizeof (fpPCMToG));
    fpPCMToG[UNKPCM000] = NulPCMtoG16;
    fpPCMToG[AVAPCM004] = NulPCMtoG16;
    fpPCMToG[BKTPCM001] = NulPCMtoG16;
    fpPCMToG[CPXPCM064] = NulPCMtoG16;
    fpPCMToG[DLGPCM004] = DlgA04toG16;
    fpPCMToG[DLGPCM008] = DlgF08toG16;
    fpPCMToG[FLTPCM032] = FLTF32toG16;
    fpPCMToG[G11PCMF08] = G11F08toG16;    
    fpPCMToG[G11PCMI08] = G11I08toG16;
    fpPCMToG[G21PCM004] = G21A04toG16;    
    fpPCMToG[G22PCM004] = NulPCMtoG16;    
    fpPCMToG[G23PCM003] = NulPCMtoG16;
    fpPCMToG[HARPCM001] = HarC01toG16;
    fpPCMToG[MPCPCM008] = MPCP08toG16;
    fpPCMToG[MPCPCM016] = MPCL16toG16;
    fpPCMToG[MSAPCM004] = MSWA04toG16;
    fpPCMToG[NWVPCM001] = NwVC01toG16;
    fpPCMToG[PTCPCM001] = PTCC01toG16;
    fpPCMToG[RTXPCM003] = NulPCMtoG16;
    fpPCMToG[RTXPCM004] = RtxA04toG16;
    fpPCMToG[TTIPCM008] = PL2P08toG16;

    /********************************************************************/
    /* Initialize Generic PCM to PCM function array                     */
    /********************************************************************/
    _fmemset (fpGenToP, 0, sizeof (fpGenToP));
    fpGenToP[UNKPCM000] = NulG16toPCM;
    fpGenToP[AVAPCM004] = NulG16toPCM;
    fpGenToP[BKTPCM001] = NulG16toPCM;
    fpGenToP[CPXPCM064] = NulG16toPCM;
    fpGenToP[DLGPCM004] = DlgG16toA04;
    fpGenToP[DLGPCM008] = DlgG16toF08;
    fpGenToP[FLTPCM032] = FLTG16toF32;
    fpGenToP[G11PCMF08] = G11G16toF08;    
    fpGenToP[G11PCMI08] = G11G16toI08;
    fpGenToP[G21PCM004] = G21G16toA04;    
    fpGenToP[G22PCM004] = NulG16toPCM;    
    fpGenToP[G23PCM003] = NulG16toPCM;
    fpGenToP[HARPCM001] = HarG16toC01;
    fpGenToP[MPCPCM008] = MPCG16toP08;
    fpGenToP[MPCPCM016] = MPCG16toL16;
    fpGenToP[MSAPCM004] = NulG16toPCM;
    fpGenToP[NWVPCM001] = NwVG16toC01;
    fpGenToP[PTCPCM001] = PTCG16toC01;
    fpGenToP[RTXPCM003] = NulG16toPCM;
    fpGenToP[RTXPCM004] = RtxG16toA04;
    fpGenToP[TTIPCM008] = PL2G16toP08;

    /********************************************************************/
    /* Initialize PCM to Max/Min function array                         */
    /********************************************************************/
    _fmemset (fpPCMToM, 0, sizeof (fpPCMToM));
    fpPCMToM[UNKPCM000] = NulPCMtoM32;
    fpPCMToM[AVAPCM004] = NulPCMtoM32;
    fpPCMToM[BKTPCM001] = NulPCMtoM32;
    fpPCMToM[CPXPCM064] = NulPCMtoM32;
    fpPCMToM[DLGPCM004] = DlgA04toM32;
    fpPCMToM[DLGPCM008] = DlgF08toM32;
    fpPCMToM[FLTPCM032] = NulPCMtoM32;
    fpPCMToM[G11PCMF08] = NulPCMtoM32;    
    fpPCMToM[G11PCMI08] = NulPCMtoM32;
    fpPCMToM[G21PCM004] = NulPCMtoM32;    
    fpPCMToM[G22PCM004] = NulPCMtoM32;    
    fpPCMToM[G23PCM003] = NulPCMtoM32;
    fpPCMToM[HARPCM001] = NulPCMtoM32;
    fpPCMToM[MPCPCM008] = MPCP08toM32;
    fpPCMToM[MPCPCM016] = MPCL16toM32;
    fpPCMToM[MSAPCM004] = NulPCMtoM32;
    fpPCMToM[NWVPCM001] = NwVC01toM32;
    fpPCMToM[PTCPCM001] = NulPCMtoM32;
    fpPCMToM[RTXPCM003] = NulPCMtoM32;
    fpPCMToM[RTXPCM004] = NulPCMtoM32;
    fpPCMToM[TTIPCM008] = PL2P08toM32;

    /********************************************************************/
    /* Initialize ITC Block compute history function array              */
    /********************************************************************/
    flMemSet (flByPSmp, UNKBPS000, sizeof (flByPSmp) / sizeof (flByPSmp[0]));
    flByPSmp[UNKPCM000] = UNKBPS000;
    flByPSmp[AVAPCM004] = AVABPS004;
    flByPSmp[BKTPCM001] = BKTBPS001;
    flByPSmp[CPXPCM064] = CPXBPS064;
    flByPSmp[DLGPCM004] = DLGBPS004;
    flByPSmp[DLGPCM008] = DLGBPS008;
    flByPSmp[FLTPCM032] = FLTBPS032;
    flByPSmp[G11PCMF08] = G11BPS008;
    flByPSmp[G11PCMI08] = G11BPS008;
    flByPSmp[G21PCM004] = G21BPS004;
    flByPSmp[G22PCM004] = G22BPS004;
    flByPSmp[G23PCM003] = G23BPS003;
    flByPSmp[HARPCM001] = HARBPS001;
    flByPSmp[MPCPCM008] = MPCBPS008;
    flByPSmp[MPCPCM016] = MPCBPS016;
    flByPSmp[MSAPCM004] = MSABPS004;
    flByPSmp[NWVPCM001] = NWVBPS001;
    flByPSmp[PTCPCM001] = PTCBPS001;
    flByPSmp[RTXPCM003] = RTXBPS003;
    flByPSmp[RTXPCM004] = RTXBPS004;
    flByPSmp[TTIPCM008] = TTIBPS008;

    /********************************************************************/
    /* Initialize Default Frequency array                               */
    /********************************************************************/
    ulMemSet (ulFrqDef, UNKFRQ000, sizeof (ulFrqDef) / sizeof (ulFrqDef[0]));
    ulFrqDef[UNKPCM000] = UNKFRQ000;
    ulFrqDef[AVAPCM004] = AVAFRQ004;
    ulFrqDef[BKTPCM001] = BKTFRQ001;
    ulFrqDef[CPXPCM064] = CPXFRQ064;
    ulFrqDef[DLGPCM004] = DLGFRQ004;
    ulFrqDef[DLGPCM008] = DLGFRQ008;
    ulFrqDef[FLTPCM032] = FLTFRQ032;
    ulFrqDef[G11PCMF08] = G11FRQ008;    
    ulFrqDef[G11PCMI08] = G11FRQ008;
    ulFrqDef[G21PCM004] = G21FRQ004;    
    ulFrqDef[G22PCM004] = G22FRQ004;    
    ulFrqDef[G23PCM003] = G23FRQ003;
    ulFrqDef[HARPCM001] = HARFRQ001;
    ulFrqDef[MPCPCM008] = MPCFRQ008;
    ulFrqDef[MPCPCM016] = MPCFRQ016;
    ulFrqDef[MSAPCM004] = MSAFRQ004;
    ulFrqDef[NWVPCM001] = NWVFRQ001;
    ulFrqDef[PTCPCM001] = PTCFRQ001;
    ulFrqDef[RTXPCM003] = RTXFRQ003;
    ulFrqDef[RTXPCM004] = RTXFRQ004;
    ulFrqDef[TTIPCM008] = TTIFRQ008; 

    /********************************************************************/
    /* Initialize A to D Resolution array                               */
    /********************************************************************/
    ulMemSet (ulAtDRes, UNKATD000, sizeof (ulAtDRes) / sizeof (ulAtDRes[0]));
    ulAtDRes[UNKPCM000] = UNKATD000;
    ulAtDRes[AVAPCM004] = AVAATD004;
    ulAtDRes[BKTPCM001] = BKTATD001;
    ulAtDRes[CPXPCM064] = CPXATD064;
    ulAtDRes[DLGPCM004] = DLGATD004;
    ulAtDRes[DLGPCM008] = DLGATD008;
    ulAtDRes[FLTPCM032] = FLTATD032;
    ulAtDRes[G11PCMF08] = G11ATD008;    
    ulAtDRes[G11PCMI08] = G11ATD008;
    ulAtDRes[G21PCM004] = G21ATD004;    
    ulAtDRes[G22PCM004] = G22ATD004;    
    ulAtDRes[G23PCM003] = G23ATD003;
    ulAtDRes[HARPCM001] = HARATD001;
    ulAtDRes[MPCPCM008] = MPCATD008;
    ulAtDRes[MPCPCM016] = MPCATD016;
    ulAtDRes[MSAPCM004] = MSAATD004;
    ulAtDRes[NWVPCM001] = NWVATD001;
    ulAtDRes[PTCPCM001] = PTCATD001;
    ulAtDRes[RTXPCM003] = RTXATD003;
    ulAtDRes[RTXPCM004] = RTXATD004;
    ulAtDRes[TTIPCM008] = TTIATD008; 

    /********************************************************************/
    /* Initialize Guard Sample Count array                              */
    /********************************************************************/
    ulMemSet (ulGrdSmp, UNKPCM000, sizeof (ulGrdSmp) / sizeof (ulGrdSmp[0]));
    ulGrdSmp[UNKPCM000] = UNKGRD000;
    ulGrdSmp[AVAPCM004] = AVAGRD004;
    ulGrdSmp[BKTPCM001] = BKTGRD001;
    ulGrdSmp[CPXPCM064] = CPXGRD064;
    ulGrdSmp[DLGPCM004] = DLGGRD004;
    ulGrdSmp[DLGPCM008] = DLGGRD008;
    ulGrdSmp[FLTPCM032] = FLTGRD032;
    ulGrdSmp[G11PCMF08] = G11GRD008;    
    ulGrdSmp[G11PCMI08] = G11GRD008;
    ulGrdSmp[G21PCM004] = G21GRD004;    
    ulGrdSmp[G22PCM004] = G22GRD004;    
    ulGrdSmp[G23PCM003] = G23GRD003;
    ulGrdSmp[HARPCM001] = HARGRD001;
    ulGrdSmp[MPCPCM008] = MPCGRD008;
    ulGrdSmp[MPCPCM016] = MPCGRD016;
    ulGrdSmp[MSAPCM004] = MSAGRD004;
    ulGrdSmp[NWVPCM001] = NWVGRD001;
    ulGrdSmp[PTCPCM001] = PTCGRD001;
    ulGrdSmp[RTXPCM003] = RTXGRD003;
    ulGrdSmp[RTXPCM004] = RTXGRD004;
    ulGrdSmp[TTIPCM008] = TTIGRD008;

    /********************************************************************/
    /* Initialize PCM Block Size array                                  */
    /********************************************************************/
    _fmemset (usBlkSiz, 0, sizeof (usBlkSiz));
    usBlkSiz[UNKPCM000] = UNKBLK024;                
    usBlkSiz[AVAPCM004] = UNKBLK024;
    usBlkSiz[BKTPCM001] = UNKBLK024;
    usBlkSiz[CPXPCM064] = UNKBLK024;
    usBlkSiz[DLGPCM004] = UNKBLK024;
    usBlkSiz[DLGPCM008] = UNKBLK024;
    usBlkSiz[FLTPCM032] = UNKBLK024;
    usBlkSiz[G11PCMF08] = UNKBLK024;    
    usBlkSiz[G11PCMI08] = UNKBLK024;
    usBlkSiz[G21PCM004] = UNKBLK024;    
    usBlkSiz[G22PCM004] = UNKBLK024;    
    usBlkSiz[G23PCM003] = UNKBLK024;
    usBlkSiz[HARPCM001] = UNKBLK024;
    usBlkSiz[MPCPCM008] = UNKBLK024;
    usBlkSiz[MPCPCM016] = UNKBLK024;
    usBlkSiz[MSAPCM004] = UNKBLK024;
    usBlkSiz[NWVPCM001] = UNKBLK024;
    usBlkSiz[PTCPCM001] = UNKBLK024;
    usBlkSiz[RTXPCM003] = UNKBLK024;
    usBlkSiz[RTXPCM004] = UNKBLK024;
    usBlkSiz[TTIPCM008] = UNKBLK024; 

    /********************************************************************/
    /* Initialize View Filter Resolution array                          */
    /********************************************************************/
    _fmemset (usViwFtr, 0, sizeof (usViwFtr));
    usViwFtr[UNKPCM000] = UNKVIW000;
    usViwFtr[AVAPCM004] = AVAVIW004;
    usViwFtr[BKTPCM001] = BKTVIW001;
    usViwFtr[CPXPCM064] = CPXVIW064;
    usViwFtr[DLGPCM004] = DLGVIW004;
    usViwFtr[DLGPCM008] = DLGVIW008;
    usViwFtr[FLTPCM032] = FLTVIW032;
    usViwFtr[G11PCMF08] = G11VIW008;    
    usViwFtr[G11PCMI08] = G11VIW008;
    usViwFtr[G21PCM004] = G21VIW004;    
    usViwFtr[G22PCM004] = G22VIW004;    
    usViwFtr[G23PCM003] = G23VIW003;
    usViwFtr[HARPCM001] = HARVIW001;
    usViwFtr[MPCPCM008] = MPCVIW008;
    usViwFtr[MPCPCM016] = MPCVIW016;
    usViwFtr[MSAPCM004] = MSAVIW004;
    usViwFtr[NWVPCM001] = NWVVIW001;
    usViwFtr[PTCPCM001] = PTCVIW001;
    usViwFtr[RTXPCM003] = RTXVIW003;
    usViwFtr[RTXPCM004] = RTXVIW004;
    usViwFtr[TTIPCM008] = TTIVIW008; 

    /********************************************************************/
    /********************************************************************/
    return (0);

}

/************************************************************************/
/************************************************************************/
void    ulMemSet (LPVOID lpDstBlk, DWORD ulSetLng, WORD usLngCnt)
{
    while (usLngCnt-- > 0) *((DWORD FAR *)lpDstBlk)++ = ulSetLng;
}
void    flMemSet (LPVOID lpDstBlk, float flSetLng, WORD usLngCnt)
{
    while (usLngCnt-- > 0) *((float FAR *)lpDstBlk)++ = flSetLng;
}

