/************************************************************************/
/* PCM Conversion Support definitions: PCMSup.h         V2.00  07/15/95 */
/* Copyright (c) 1987-1995 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                  Array/String Length Constants                       */
/************************************************************************/
#define PCMMAXSTR      40               /* Gen purpose string length    */
#define PCMMAXMSG      81               /* Message Box string length    */
#define REGKEYLEN       8               /* Registration key length      */
#define SERSEGLEN       4               /* Serial number segment length */
#define PCMLOCBAS     256               /* PCM DLL local base msg       */
#define BYTMAXFIO  0xfc00L              /* Maximum file I/O byte count  */

/************************************************************************/
/*                  Bit/Nibble/Byte/Word Ordering Macros                */
/************************************************************************/
#define N__SWP(b)   ((BYTE)((((BYTE)((b)&0xff)) >> 4) | (((BYTE)((b)&0xff)) << 4)))
#define W__SWP(w)   ((((WORD)((w)&0xffff)) >> 8) | (((WORD)((w)&0xffff)) << 8))
#define DW_SWP(dw)  (((DWORD)W__SWP(((DWORD)(dw)) >> 16)) | (((DWORD)W__SWP((DWORD)(dw))) << 16))

/************************************************************************/
/************************************************************************/
#define dbPI        3.141592653589793   /* Pi                           */
#define db2PI       6.28318530717959    /* 2 * Pi                       */

/************************************************************************/
/************************************************************************/
typedef long            LNGFRA;         /* long "fractional" value      */
#define NRM                 16          /* Normalization shift count    */
#define ONE            (1L << NRM)      /* Normalized unity value       */   
#define MUL(x,y) ((((x)>>5)*((y)>>5))>>6)   /* Mul w/o und/ovr flow     */
#define MAC(x,y) (((x)>>5)*((y)>>5))    /* Mul/Acc w/o und/ovr flow     */
#define ACC(z)   ((z)>>6)               /* Mul/Acc w/o und/ovr flow     */

/************************************************************************/
/*              Declarations for Windows KERNEL Functions               */
/************************************************************************/
#if !(defined (WM_NULL))                                /* No Win       */ 
    typedef WORD        HANDLE;
#endif
#if !(0)                                                /* 32 bit       */ 
    typedef BYTE  (_based(void) * BYTBASPTR);
    typedef short (_based(void) * GENBASPTR);
    typedef       _segment        SEGBASPTR;  
#endif

/************************************************************************/
/*              Global Data Structures Definitions                      */
/************************************************************************/
typedef struct _PCMGLO {
    char    szLibNam  [PCMMAXSTR];      /* Library name                 */
    char    RelKeyArr [REGKEYLEN];      /* Operating/Demo Only Key      */
    char    SeqNumArr [SERSEGLEN];      /* Op/Demo sequence number      */
    char    ChkSumArr [SERSEGLEN];      /* Security check sum           */
    WORD    usRegKey;                   /* usRegKey                     */
    WORD    usDebFlg;                   /* Debug on/off flags           */
    HANDLE  hLibIns;                    /* Global instance handle       */
} PCMGLO;

/************************************************************************/
/************************************************************************/
#define ERR___DEB       0x0002          /* Debug error messages flag    */
#define KEY___DEB       0x0008          /* Debug key flag               */
#define MEM___DEB       0x0020          /* Debug memory messages flag   */
#define CFG___DEB       0x0080          /* Debug config settings flag   */
#define INI___DEB       0x0200          /* Debug init   settings flag   */

/************************************************************************/
/*              Generic Digitizing Hardware Constants                   */
/************************************************************************/
#define UNKBPS000   (float) 1000.0      /* Bytes per sample             */
#define AVABPS004   (float)    0.5 
#define BKTBPS001   (float)    0.125 
#define CPXBPS064   (float)    8.0 
#define DLGBPS004   (float)    0.5 
#define DLGBPS008   (float)    1.0 
#define FLTBPS032   (float)    4.0 
#define G11BPS008   (float)    1.0     
#define G21BPS004   (float)    0.5     
#define G22BPS004   (float)    0.5     
#define G23BPS003   (float)    0.375 
#define HARBPS001   (float)    0.125 
#define MPCBPS008   (float)    1.0 
#define MPCBPS016   (float)    2.0 
#define MSABPS004   (float)    0.5 
#define NWVBPS001   (float)    0.125 
#define PTCBPS001   (float)    0.125 
#define RTXBPS003   (float)    0.375 
#define RTXBPS004   (float)    0.5 
#define TTIBPS008   (float)    1.0  

#define UNKFRQ000       0L              /* Default frequency            */
#define AVAFRQ004       0L 
#define BKTFRQ001       0L 
#define CPXFRQ064       0L 
#define DLGFRQ004    6000L 
#define DLGFRQ008    6000L 
#define FLTFRQ032       0L 
#define G11FRQ008    8000L     
#define G21FRQ004    8000L     
#define G22FRQ004       0L     
#define G23FRQ003    8000L 
#define HARFRQ001   32000L 
#define MPCFRQ008   11025L 
#define MPCFRQ016   11025L 
#define MSAFRQ004   11025L 
#define NWVFRQ001   32000L 
#define PTCFRQ001   32000L 
#define RTXFRQ003    8000L 
#define RTXFRQ004    8000L 
#define TTIFRQ008    6757L  

#define UNKATD000       0L                  /* A to D resolution            */
#define AVAATD004      12L 
#define BKTATD001      12L 
#define CPXATD064      22L 
#define DLGATD004      12L 
#define DLGATD008      12L 
#define FLTATD032      22L 
#define G11ATD008      14L     
#define G21ATD004      14L     
#define G22ATD004      14L     
#define G23ATD003      14L 
#define HARATD001      10L 
#define MPCATD008       8L 
#define MPCATD016      16L 
#define MSAATD004      16L 
#define NWVATD001      10L 
#define PTCATD001      10L 
#define RTXATD003      12L 
#define RTXATD004      12L 
#define TTIATD008      11L  
                       
#define UNKGRD000       0L
#define AVAGRD004    4096L
#define BKTGRD001    1024L
#define CPXGRD064       0L
#define DLGGRD004    4096L
#define DLGGRD008       0L
#define FLTGRD032       0L
#define G11GRD008       0L    
#define G21GRD004    4096L    
#define G22GRD004    4096L    
#define G23GRD003    4096L
#define HARGRD001    1024L
#define MPCGRD008       0L
#define MPCGRD016       0L
#define MSAGRD004    1024L
#define NWVGRD001    1024L
#define PTCGRD001    1024L
#define RTXGRD003    4096L
#define RTXGRD004    4096L
#define TTIGRD008       0L

#define UNKBLK024      24               /* 24 div by 1,4,8,12 & 16 bit  */               

#define UNKVIW000       0               /* View filter sample count     */
#define AVAVIW004      20               /* ADPCM's use approx 20 samps. */
#define BKTVIW001      16               /* Prevents buffer overwrite    */
#define CPXVIW064       1               /*   for in-place conversions.  */ 
#define DLGVIW004      20 
#define DLGVIW008       4 
#define FLTVIW032       1 
#define G11VIW008       4                    
#define G21VIW004      20         
#define G22VIW004      20         
#define G23VIW003      20     
#define HARVIW001      32     
#define MPCVIW008       4 
#define MPCVIW016       2 
#define MSAVIW004      20 
#define NWVVIW001      32
#define PTCVIW001      32 
#define RTXVIW003      20
#define RTXVIW004      20 
#define TTIVIW008       1 

/************************************************************************/
/*                      Dialogic Hardware Constants                     */
/************************************************************************/
#if (defined (DLG)) /****************************************************/
    #define PCMTYPDEF     DLGPCM004     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     DLGFRQ004     /* Default sample frequency     */
    #define ATDRESDEF     DLGATD004     /* A/D sample resolution        */
    #define ATDMINDEF   0xfffff800L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x000007ffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (DLG) || defined (_P_DLG))) /*****************************/
    #define DlgA04toIni NulPCMtoIni
    #define DlgA04toCmp NulPCMtoCmp
    #define DlgA04toG16 NulPCMtoG16
    #define DlgG16toA04 NulG16toPCM
    #define DlgA04toM32 NulPCMtoM32
    #define DlgSiltoA04 NulSiltoPCM
    #define DlgF08toIni NulPCMtoIni
    #define DlgF08toCmp NulPCMtoCmp
    #define DlgF08toG16 NulPCMtoG16
    #define DlgG16toF08 NulG16toPCM
    #define DlgF08toM32 NulPCMtoM32
    #define DlgSiltoF08 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*                  CCITT G.711 Hardware Constants                      */
/************************************************************************/
#if (defined (G11)) /****************************************************/
    #define PCMTYPDEF     G11PCMF08     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     G11FRQ008     /* Default sample frequency     */
    #define ATDRESDEF     G11ATD008     /* A/D sample resolution        */
    #define ATDMINDEF   0xffffe000L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x00001fffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (G11) || defined (_P_G11))) /*****************************/
    #define G11F08toIni NulPCMtoIni
    #define G11F08toCmp NulPCMtoCmp
    #define G11F08toG16 NulPCMtoG16
    #define G11G16toF08 NulG16toPCM
    #define G11F08toM32 NulPCMtoM32
    #define G11SiltoF08 NulSiltoPCM
    #define G11I08toIni NulPCMtoIni
    #define G11I08toCmp NulPCMtoCmp
    #define G11I08toG16 NulPCMtoG16
    #define G11G16toI08 NulG16toPCM
    #define G11I08toM32 NulPCMtoM32
    #define G11SiltoI08 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*                  CCITT G.721 Hardware Constants                      */
/************************************************************************/
#if (defined (G21)) /****************************************************/
    #define PCMTYPDEF     G21PCM004     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     G21FRQ004     /* Default sample frequency     */
    #define ATDRESDEF     G21ATD004     /* A/D sample resolution        */
    #define ATDMINDEF   0xffffe000L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x00001fffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (G21) || defined (_P_G21))) /*****************************/
    #define G21A04toIni NulPCMtoIni
    #define G21A04toCmp NulPCMtoCmp
    #define G21A04toG16 NulPCMtoG16
    #define G21G16toA04 NulG16toPCM
    #define G21A04toM32 NulPCMtoM32
    #define G21SiltoA04 NulSiltoPCM
    #define G23A03toIni NulPCMtoCmp
    #define G23ITCtoA03 NulPCMtoIni
    #define G23A03toG16 NulPCMtoG16
    #define G23G16toA03 NulG16toPCM
    #define G23A03toM32 NulPCMtoM32
    #define G23SiltoA03 NulSiltoPCM
    #define G23A05toIni NulPCMtoCmp
    #define G23ITCtoA05 NulPCMtoIni
    #define G23A05toG16 NulPCMtoG16
    #define G23G16toA05 NulG16toPCM
    #define G23A05toM32 NulPCMtoM32
    #define G23SiltoA05 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*                  Harris Semiconductor Constants                      */
/************************************************************************/
#if (defined (HAR)) /****************************************************/
    #define PCMTYPDEF     HARPCM001     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     HARFRQ001     /* Default sample frequency     */
    #define ATDRESDEF     HARATD001     /* A/D sample resolution        */
    #define ATDMINDEF   0xfffffe00L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x000001ffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (HAR) || defined (_P_HAR))) /*****************************/
    #define HarC01toAlo NulPCMtoAlo
    #define HarC01toIni NulPCMtoIni
    #define HarC01toCmp NulPCMtoCmp
    #define HarC01toRel NulPCMtoRel
    #define HarC01toG16 NulPCMtoG16
    #define HarG16toC01 NulG16toPCM
    #define HarC01toM32 NulPCMtoM32
    #define HarSiltoC01 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*                  NewVoice Hardware Constants                         */
/************************************************************************/
#if (defined (NWV)) /****************************************************/
    #define PCMTYPDEF     NWVPCM001     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     NWVFRQ001     /* Default sample frequency     */
    #define ATDRESDEF     NWVATD001     /* A/D sample resolution        */
    #define ATDMINDEF   0xfffffe00L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x000001ffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (NWV) || defined (HAR) || defined (PTC) || defined (_P_NWV)))
    #define NwVC01toAlo NulPCMtoAlo
    #define NwVC01toIni NulPCMtoIni
    #define NwVC01toCmp NulPCMtoCmp
    #define NwVC01toRel NulPCMtoRel
    #define NwVC01toG16 NulPCMtoG16
    #define NwVG16toC01 NulG16toPCM
    #define NwVC01toM32 NulPCMtoM32
    #define NwVSiltoC01 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*              Perception Technology Hardware Constants                */
/************************************************************************/
#if (defined (PTC)) /****************************************************/
    #define PCMTYPDEF     PTCPCM001     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     PTCFRQ001     /* Default sample frequency     */
    #define ATDRESDEF     PTCATD001     /* A/D sample resolution        */
    #define ATDMINDEF   0xfffffe00L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x000001ffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (PTC) || defined (_P_PTC))) /*****************************/
    #define PTCC01toAlo NulPCMtoAlo
    #define PTCC01toIni NulPCMtoIni
    #define PTCC01toCmp NulPCMtoCmp
    #define PTCC01toRel NulPCMtoRel
    #define PTCC01toG16 NulPCMtoG16
    #define PTCG16toC01 NulG16toPCM
    #define PTCC01toM32 NulPCMtoM32
    #define PTCSiltoC01 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*                      Rhetorex Hardware Constants                     */
/************************************************************************/
#if (defined (RTX)) /****************************************************/
    #define PCMTYPDEF     RTXPCM004     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     RTXFRQ004     /* Default sample frequency     */
    #define ATDRESDEF     RTXATD004     /* A/D sample resolution        */
    #define ATDMINDEF   0xfffff800L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x000007ffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (RTX) || defined (_P_RTX))) /*****************************/
    #define RtxA04toAlo NulPCMtoAlo
    #define RtxA04toIni NulPCMtoIni
    #define RtxA04toCmp NulPCMtoCmp
    #define RtxA04toRel NulPCMtoRel
    #define RtxA04toG16 NulPCMtoG16
    #define RtxG16toA04 NulG16toPCM
    #define RtxA04toM32 NulPCMtoM32
    #define RtxSiltoA04 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*              Talking Technology Hardware Constants                   */
/************************************************************************/
#if (defined (TTI)) /****************************************************/
    #define PCMTYPDEF     TTIPCM008     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF     TTIFRQ008     /* Default sample frequency     */
    #define ATDRESDEF     TTIATD008     /* A/D sample resolution        */
    #define ATDMINDEF   0xfffffc00L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x000003ffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (TTI) || defined (_P_TTI))) /*****************************/
    #define PL2A04toIni NulPCMtoIni
    #define PL2A04toCmp NulPCMtoCmp
    #define PL2A04toG16 NulPCMtoG16
    #define PL2G16toA04 NulG16toPCM
    #define PL2A04toM32 NulPCMtoM32
    #define PL2SiltoA04 NulSiltoPCM
    #define PL2P08toIni NulPCMtoIni
    #define PL2P08toCmp NulPCMtoCmp
    #define PL2P08toG16 NulPCMtoG16
    #define PL2G16toP08 NulG16toPCM
    #define PL2P08toM32 NulPCMtoM32
    #define PL2SiltoP08 NulSiltoPCM
#endif /*****************************************************************/

/************************************************************************/
/*                  Multimedia Hardware Constants                       */
/************************************************************************/
#if (defined (MPC)) /****************************************************/
    #define PCMTYPDEF     LINPCM016     /* Default PCM type             */
    #define CHNCNTDEF            1L     /* Default channel count        */
    #define SMPFRQDEF        11025L     /* Default sample frequency     */
    #define ATDRESDEF     LINATD016     /* A/D sample resolution        */
    #define ATDMINDEF   0xffff8000L     /* A/D limited two's comp min   */
    #define ATDMAXDEF   0x00007fffL     /* A/D limited two's comp max   */
#endif /*****************************************************************/
#if (!(defined (MSW) || defined (_P_MSW))) /*****************************/
    #define MSWA04toIni NulPCMtoIni
    #define MSWA04toG16 NulPCMtoG16
    #define MSWG16toA04 NulG16toPCM
#endif /*****************************************************************/

/************************************************************************/
/*                          Resource Control                            */
/************************************************************************/
WORD    PCMResIni   (HANDLE);
WORD    PCMResTrm   (HANDLE);

/************************************************************************/
/************************************************************************/
typedef WORD    (FAR PASCAL *SETSILPRC) (BYTE FAR *, WORD);

typedef LPITCB  (FAR PASCAL *ALOITCPRC) (LPITCB, LPITCI);
typedef LPITCB  (FAR PASCAL *INIITCPRC) (LPITCB);
typedef WORD    (FAR PASCAL *CMPITCPRC) (SEGBASPTR, BYTBASPTR, 
                WORD FAR *, WORD, LPITCB);
typedef LPITCB  (FAR PASCAL *RELITCPRC) (LPITCB);

typedef WORD    (FAR PASCAL *PCMTOGPRC) (SEGBASPTR, BYTBASPTR, 
                WORD FAR *, GENBASPTR, WORD, LPITCB);
typedef WORD    (FAR PASCAL *GTOPCMPRC) (SEGBASPTR, GENBASPTR, 
                WORD FAR *, BYTBASPTR, WORD, LPITCB);
typedef WORD    (FAR PASCAL *PCMTOMPRC) (SEGBASPTR, BYTBASPTR,
                WORD FAR *, GENBASPTR, WORD, DWORD, LPITCB);

/************************************************************************/
/*                  Dialogic D4x conversion functions                   */
/************************************************************************/
LPITCB  FAR PASCAL  DlgA04toIni (LPITCB);
WORD    FAR PASCAL  DlgA04toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  DlgA04toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  DlgG16toA04 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  DlgA04toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  DlgSiltoA04 (BYTE FAR *, WORD);

LPITCB  FAR PASCAL  DlgF08toIni (LPITCB);
WORD    FAR PASCAL  DlgF08toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  DlgF08toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  DlgG16toF08 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  DlgF08toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  DlgSiltoF08 (BYTE FAR *, WORD);

/************************************************************************/
/*                  Floating point conversion functions                 */
/************************************************************************/
WORD    FAR PASCAL  FLTF32toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  FLTG16toF32 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);

/************************************************************************/
/*                  CCITT G.711 conversion functions                    */
/************************************************************************/
LPITCB  FAR PASCAL  G11I08toIni (LPITCB);
WORD    FAR PASCAL  G11I08toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  G11I08toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  G11G16toI08 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  G11I08toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  G11SiltoI08 (BYTE FAR *, WORD);

LPITCB  FAR PASCAL  G11F08toIni (LPITCB);
WORD    FAR PASCAL  G11F08toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  G11F08toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  G11G16toF08 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  G11F08toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  G11SiltoF08 (BYTE FAR *, WORD);

/************************************************************************/
/*                  CCITT G.721 conversion functions                    */
/************************************************************************/
LPITCB  FAR PASCAL  G21A04toIni (LPITCB);
WORD    FAR PASCAL  G21A04toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  G21A04toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  G21G16toA04 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  G21A04toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  G21SiltoA04 (BYTE FAR *, WORD);

/************************************************************************/
/*              Harris Semiconductor CVSD Conversion functions          */
/************************************************************************/
LPITCB  FAR PASCAL  HarC01toAlo (LPITCB, LPVOID);
LPITCB  FAR PASCAL  HarC01toIni (LPITCB);
WORD    FAR PASCAL  HarC01toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
LPITCB  FAR PASCAL  HarC01toRel (LPITCB);
WORD    FAR PASCAL  HarC01toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  HarG16toC01 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  HarC01toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  HarSiltoC01 (BYTE FAR *, WORD);

/************************************************************************/
/*                  Multimedia conversion functions                     */
/************************************************************************/
LPITCB  FAR PASCAL  MPCL16toIni (LPITCB);
WORD    FAR PASCAL  MPCL16toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  MPCG16toL16 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  MPCL16toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  MPCSiltoL16 (BYTE FAR *, WORD);
WORD    FAR PASCAL  MPCP08toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
LPITCB  FAR PASCAL  MPCP08toIni (LPITCB);
WORD    FAR PASCAL  MPCG16toP08 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  MPCP08toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  MPCSiltoP08 (BYTE FAR *, WORD);

/************************************************************************/
/*                  Microsoft Wave conversion functions                 */
/************************************************************************/
LPITCB  FAR PASCAL  MSWA04toIni (LPITCB);
WORD    FAR PASCAL  MSWA04toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  MSWG16toA04 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);

/************************************************************************/
/*                  New Voice CVSD Conversion functions                 */
/************************************************************************/
LPITCB  FAR PASCAL  NwVC01toAlo (LPITCB, LPVOID);
LPITCB  FAR PASCAL  NwVC01toIni (LPITCB);
WORD    FAR PASCAL  NwVC01toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
LPITCB  FAR PASCAL  NwVC01toRel (LPITCB);
WORD    FAR PASCAL  NwVC01toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  NwVG16toC01 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  NwVC01toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  NwVSiltoC01 (BYTE FAR *, WORD);

/************************************************************************/
/*              Perception Technology CVSD Conversion functions         */
/************************************************************************/
LPITCB  FAR PASCAL  PTCC01toAlo (LPITCB, LPVOID);
LPITCB  FAR PASCAL  PTCC01toIni (LPITCB);
WORD    FAR PASCAL  PTCC01toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
LPITCB  FAR PASCAL  PTCC01toRel (LPITCB);
WORD    FAR PASCAL  PTCC01toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  PTCG16toC01 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  PTCC01toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  PTCSiltoC01 (BYTE FAR *, WORD);

/************************************************************************/
/*                  Rhetorex D4x conversion functions                   */
/************************************************************************/
LPITCB  FAR PASCAL  RtxA04toAlo (LPITCB, LPVOID);
LPITCB  FAR PASCAL  RtxA04toIni (LPITCB);
WORD    FAR PASCAL  RtxA04toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
LPITCB  FAR PASCAL  RtxA04toRel (LPITCB);
WORD    FAR PASCAL  RtxA04toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  RtxG16toA04 (SEGBASPTR, GENBASPTR, WORD FAR *, 
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  RtxA04toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  RtxSiltoA04 (BYTE FAR *, WORD);

/************************************************************************/
/*                  TTI PCM conversion functions                        */
/************************************************************************/
LPITCB  FAR PASCAL  PL2A04toIni (LPITCB);
WORD    FAR PASCAL  PL2A04toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  PL2A04toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  PL2G16toA04 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  PL2A04toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  PL2SiltoA04 (BYTE FAR *, WORD);

LPITCB  FAR PASCAL  PL2P08toIni (LPITCB);
WORD    FAR PASCAL  PL2P08toCmp (SEGBASPTR, BYTBASPTR, WORD FAR *, WORD, LPITCB);
WORD    FAR PASCAL  PL2P08toG16 (SEGBASPTR, BYTBASPTR, WORD FAR *,
                    GENBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  PL2G16toP08 (SEGBASPTR, GENBASPTR, WORD FAR *,
                    BYTBASPTR, WORD, LPITCB);
WORD    FAR PASCAL  PL2P08toM32 (SEGBASPTR, BYTBASPTR, WORD FAR *, 
                    GENBASPTR, WORD, DWORD, LPITCB); 
WORD    FAR PASCAL  PL2SiltoP08 (BYTE FAR *, WORD);

/************************************************************************/
/*                  Internal conversion functions                       */
/************************************************************************/
typedef WORD (FAR PASCAL *PCMBLKCBK)(BYTE FAR *,WORD,LPVOID);
typedef struct _RW_CBK {
        short   sFilHdl;
        short   sFIOEnc;
        int     iErrCod;
} RW_CBK;

WORD    PCMPtoG16_LB (BYTE FAR *, WORD, DWORD, PCMTYP, WORD, LPITCB, 
        WORD FAR *, WORD FAR *, PCMBLKCBK, LPVOID);
WORD    PCMG16toP_SB (BYTE FAR *, WORD, DWORD, PCMTYP, WORD, LPITCB, 
        WORD FAR *, WORD FAR *);

/************************************************************************/
/************************************************************************/
#define SI_UNKPCM000       32 + PCMLOCBAS   /* Unknown                  */
#define SI_AVAPCM004       33 + PCMLOCBAS   /* AVA 4 bit                */
#define SI_BKTPCM001       34 + PCMLOCBAS   /* BrookTrout CVSD          */   
#define SI_DLGPCM004       35 + PCMLOCBAS   /* Dialogic 4 bit           */
#define SI_DLGPCM008       36 + PCMLOCBAS   /* Dialogic 8 bit           */
#define SI_FLTPCM032       37 + PCMLOCBAS   /* Float 32 bit             */
#define SI_FTRPCM032       38 + PCMLOCBAS   /* FFT Real 32 bit          */
#define SI_G11PCMI08       39 + PCMLOCBAS   /* CCITT G.711 Inv A-law    */
#define SI_G11PCMF08       40 + PCMLOCBAS   /* CCITT G.711 Folded u-Law */
#define SI_G21PCM004       41 + PCMLOCBAS   /* CCITT G.721              */
#define SI_G22PCM004       42 + PCMLOCBAS   /* CCITT G.722              */
#define SI_G23PCM003       43 + PCMLOCBAS   /* CCITT G.723 3 bit        */
#define SI_HARPCM001       44 + PCMLOCBAS   /* Harris Semi CVSD         */
#define SI_MPCPCM008       45 + PCMLOCBAS   /* Multimedia unsigned 8    */    
#define SI_MPCPCM016       46 + PCMLOCBAS   /* Multimedia linear  16    */    
#define SI_MSAPCM004       47 + PCMLOCBAS   /* MS Wave ADPCM            */
#define SI_NWVPCM001       48 + PCMLOCBAS   /* New Voice CVSD           */
#define SI_PTCPCM001       49 + PCMLOCBAS   /* Perception Tech CVSD     */   
#define SI_RTXPCM003       50 + PCMLOCBAS   /* Rhetorex ADPCM 3 bit     */   
#define SI_RTXPCM004       51 + PCMLOCBAS   /* Rhetorex ADPCM 4 bit     */   
#define SI_TTIPCM008       52 + PCMLOCBAS   /* TTI 8 bit                */














