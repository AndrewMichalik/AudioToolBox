/************************************************************************/
/* PCM Query Functions: PCMQry.c                        V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/
#include "windows.h"                    /* Windows SDK definitions      */
#include "genpcm.h"                     /* PCM/APCM conversion defs     */
#include "pcmsup.h"                     /* PCM/APCM xlate lib defs      */

#include <math.h>                       /* Floating point math funcs    */

/************************************************************************/
/*                  Conversion Routine References                       */
/************************************************************************/
extern  float       flByPSmp[MAXPCM000];
extern  DWORD       ulFrqDef[MAXPCM000];
extern  DWORD       ulAtDRes[MAXPCM000];
extern  DWORD       ulGrdSmp[MAXPCM000];
extern  WORD        usBlkSiz[MAXPCM000];
extern  WORD        usViwFtr[MAXPCM000];

/************************************************************************/
/************************************************************************/
#define BIG_DWORD   ((DWORD) 0xffffffffL)
#define BIG_FLOAT   ((float) BIG_DWORD)
#define SAFFTD(x)   (((x) > BIG_FLOAT) ? BIG_DWORD : (DWORD) (x))  

/************************************************************************/
/************************************************************************/
DWORD FAR PASCAL PCMCapQry (PCMTYP usPCMTyp, DWORD ulCapTyp, DWORD ulRsv001)
{
    /********************************************************************/
    /* If out of range, return default information                      */
    /* Note: This should help the system degrade more gracefully        */
    /********************************************************************/
    if (usPCMTyp >= MAXPCM000) usPCMTyp = 0;

    /********************************************************************/
    /* Return defaults for usPCMTyp == 0
    /********************************************************************/
    if (!usPCMTyp) switch (ulCapTyp) {
        case PCMDEFQRY: return (PCMTYPDEF);             /* Def PCM typ  */
        case CHNDEFQRY: return (CHNCNTDEF);             /* Def Chn cnt  */
        case FRQDEFQRY: return (SMPFRQDEF);             /* Def smp frq  */
        case ATDRESQRY: return (ATDRESDEF);             /* A/D bit res  */
        case ATDMINQRY: return (ATDMINDEF);             /* A/D minimum  */
        case ATDMAXQRY: return (ATDMAXDEF);             /* A/D maximum  */
        case GRDSMPQRY: return (ulGrdSmp[PCMTYPDEF]);   /* Guard Sample */
        case BLKSIZQRY: return (usBlkSiz[PCMTYPDEF]);   /* Byte boundary*/
        case VIWFTRQRY: return (usViwFtr[PCMTYPDEF]);   /* View Filter  */
    }
    else switch (ulCapTyp) {
        case CHNDEFQRY: return (1L); 
        case FRQDEFQRY: return (ulFrqDef[usPCMTyp]); 
        case ATDRESQRY: return (ulAtDRes[usPCMTyp]);   
        case ATDMINQRY: return ((DWORD) - ((pow (2, ulAtDRes[usPCMTyp]) / 2) - 1));
        case ATDMAXQRY: return ((DWORD) + ((pow (2, ulAtDRes[usPCMTyp]) / 2) - 1));          
        case GRDSMPQRY: return (ulGrdSmp[usPCMTyp]);    /* Guard Sample */
        case BLKSIZQRY: return (usBlkSiz[usPCMTyp]);    /* Byte boundary*/
        case VIWFTRQRY: return (usViwFtr[usPCMTyp]);    /* View Filter  */
    }

    /********************************************************************/
    /********************************************************************/
    return ((DWORD) -1L);                           /* Unknown query    */

}

/************************************************************************/
/************************************************************************/
/////////////////////////////////////////////////////////////////////////////
//	Note: MSC 8.0 bug when compiling with Compiler Optimizations = Max Speed
//	VoxFonts 3.0 generates empty files for 8 bit PCM output data. Is this 
//	related to ToolBox 2.00d EffFtr.c MSC 8.0 -Ocglt bug?
/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", off)
DWORD FAR PASCAL PCMByt2Sl (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulBytCnt, LPITCB lpITCBlk)
{
    /********************************************************************/
    /* Future: Use channel count to more precisely round for mulit chan */
    /********************************************************************/
    if (usPCMTyp >= MAXPCM000) usPCMTyp = 0;
    if (!usChnCnt) usChnCnt = CHNCNTDEF;

    /********************************************************************/
    /* Round down to the fewest "samples available" count               */
    /********************************************************************/
    if (MSAPCM004 == usPCMTyp) {
        #define MSABPBDEF   256
        #define MSASPBDEF   500
        LPADPCMWAVEFORMAT   lpAPCFmt;
        DWORD               ulBlkByt = MSABPBDEF;
        DWORD               ulBlkSmp = MSASPBDEF;
        if (NULL != lpITCBlk) {
            lpAPCFmt = &lpITCBlk->mwMSW.afWEX;
            ulBlkByt = lpAPCFmt->wfx.nBlockAlign;
            ulBlkSmp = (WORD) lpAPCFmt->wSamplesPerBlock;
        }
        if (!ulBlkByt || !ulBlkSmp) return (0L);

        /****************************************************************/
        /* Calculate full block samples                                 */
        /* MSADEC cannot handle partial blocks - find newer version?    */
        /****************************************************************/
        return (SAFFTD ((ulBytCnt / ulBlkByt) * ulBlkSmp)); 
    }

    /********************************************************************/
    /********************************************************************/
    return (SAFFTD (ulBytCnt / flByPSmp[usPCMTyp])); 
}
/////////////////////////////////////////////////////////////////////////////
//	End MSC 8.0 bug
/////////////////////////////////////////////////////////////////////////////
#pragma optimize("", on)

DWORD FAR PASCAL PCMSmp2Bh (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulSmpCnt, LPITCB lpITCBlk)
{
    /********************************************************************/
    /* Future: Use channel count to more precisely round for mulit chan */
    /********************************************************************/
    if (usPCMTyp >= MAXPCM000) usPCMTyp = 0;
    if (!usChnCnt) usChnCnt = CHNCNTDEF;

    /********************************************************************/
    /* Round up to the nearest "bytes required" count                   */
    /********************************************************************/
    if (MSAPCM004 == usPCMTyp) {
        #define MSABPBDEF   256
        #define MSASPBDEF   500
        LPADPCMWAVEFORMAT   lpAPCFmt;
        DWORD               ulBlkByt = MSABPBDEF;
        DWORD               ulBlkSmp = MSASPBDEF;
        if (NULL != lpITCBlk) {
            lpAPCFmt = &lpITCBlk->mwMSW.afWEX;
            ulBlkByt = lpAPCFmt->wfx.nBlockAlign;
            ulBlkSmp = (WORD) lpAPCFmt->wSamplesPerBlock;
        }
        if (!ulBlkByt || !ulBlkSmp) return (0L);

        /****************************************************************/
        /* Round up to nearest full block byte count                    */
        /****************************************************************/
        ulSmpCnt = (DWORD) (ceil (ulSmpCnt / (float) ulBlkSmp) * ulBlkSmp);
        return (SAFFTD ((ulSmpCnt / ulBlkSmp) * (float) ulBlkByt)); 
    }

    /********************************************************************/
    /********************************************************************/
    return (SAFFTD (ceil ((float) ulSmpCnt * flByPSmp[usPCMTyp]))); 

}

DWORD FAR PASCAL PCMSmpLow (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulSmpCnt, LPITCB lpITCBlk)
{
    /********************************************************************/
    /* Future: Use channel count to more precisely round for multi chan */
    /********************************************************************/
    if (usPCMTyp >= MAXPCM000) usPCMTyp = 0;
    if (!usChnCnt) usChnCnt = CHNCNTDEF;

    /********************************************************************/
    /* Round down to the nearest "samples to request" count             */
    /* Return zero if request is less than block size                   */
    /********************************************************************/
    if (MSAPCM004 == usPCMTyp) {
        #define MSABPBDEF   256
        #define MSASPBDEF   500
        LPADPCMWAVEFORMAT   lpAPCFmt;
        DWORD               ulBlkByt = MSABPBDEF;
        DWORD               ulBlkSmp = MSASPBDEF;
        if (NULL != lpITCBlk) {
            lpAPCFmt = &lpITCBlk->mwMSW.afWEX;
            ulBlkByt = lpAPCFmt->wfx.nBlockAlign;
            ulBlkSmp = (WORD) lpAPCFmt->wSamplesPerBlock;
        }
        if (!ulBlkSmp) return (0L);
        /****************************************************************/
        /* Return unevenly blocked count if request < than block size   */
        /****************************************************************/
        return (ulBlkSmp * ((DWORD) (ulSmpCnt / ulBlkSmp)));
    }

    /********************************************************************/
    /********************************************************************/
    if (0L == (DWORD) (ulSmpCnt * flByPSmp[usPCMTyp])) return (ulSmpCnt);
    return (SAFFTD (SAFFTD (ulSmpCnt * flByPSmp[usPCMTyp]) / flByPSmp[usPCMTyp]));
}

DWORD FAR PASCAL PCMSmpHgh (PCMTYP usPCMTyp, WORD usChnCnt, DWORD ulSmpCnt, LPITCB lpITCBlk)
{
    /********************************************************************/
    /* Future: Use channel count to more precisely round for mulit chan */
    /********************************************************************/
    if (usPCMTyp >= MAXPCM000) usPCMTyp = 0;
    if (!usChnCnt) usChnCnt = CHNCNTDEF;

    /********************************************************************/
    /* Round up to the nearest "samples to request" count               */
    /********************************************************************/
    return (SAFFTD (0L));
}
