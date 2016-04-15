/************************************************************************/
/* VoxFnt User Registration Definitions: VxFReg.h       V2.00  12/20/92 */
/* Copyright (c) 1989-1993 Andrew J. Michalik                           */
/*                                                                      */
/************************************************************************/

/************************************************************************/
/*                  Demo Registration Definitions                       */
/************************************************************************/
#define REGDEMKEY   0                   /* Demo version key detected    */
#define REGEVLKEY   1                   /* Eval version key detected    */
#define REGFULKEY   2                   /* Full version key detected    */
#define REGMASKEY   3                   /* Mstr version key detected    */

/************************************************************************/
/* (36**4-1 = 1,679,616) Prime numbers above 1000:                      */
/*                                                                      */
/* 1087, 1091, 1093, 1097, 1117, 1123, 1129, 1151, 1153, 1163, 1171     */
/* 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249     */
/* 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319     */
/* 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429     */
/************************************************************************/

/************************************************************************/
/* Reg Key = strrev (BASE36 (App Key || Usr Key))                       */
/* Seq#: 1193 x 11 || xxxx = swap (j4a0 || xxxx)                        */ 
/* Eval: 1217 x 11 || xxxx = swap (vba0 || xxxx)                        */ 
/* Full: 1237 x 11 || xxxx = swap (zha0 || xxxx)                        */ 
/* Mstr: 1283 x 11 || xxxx = swap (1wa0 || xxxx)                        */ 
/* Mstr: 1283 x 99 || xxxx = swap (90q2 || xxxx)                        */ 
/************************************************************************/
#define KEYSEQAPP  "00001193"           /* Seq# version Appl key        */
#define KEYEVLAPP  "00001217"           /* Eval version Appl key        */
#define KEYFULAPP  "00001237"           /* Full version Appl key        */
#define KEYMASAPP  "00001283"           /* Mstr version Appl key        */

/************************************************************************/
/*                  Demo Registration Functions                         */
/************************************************************************/
#define         ChkRegKey CRK           /* Reduce for library exposure  */
static  WORD    ChkRegKey (char far *, char far *, WORD FAR *);
#define         GetEncKey CEK           /* Reduce for library exposure  */
static  WORD    GetEncKey (char far *, WORD, WORD FAR *);

/************************************************************************/
/*                  Hardware Key Access Functions                       */
/************************************************************************/
#define IDR_CODE        101                 // User Defined Resource Type
#define AZ_KECHK        1000                // User Defined Resource Id

typedef struct tagSCB
{
   short            return_code;
   short            return_status;
   short            return_function;
   char             function_code;
   char             id[5];
   char             product_name[9];
   char             fillerA;
   char             fillerB;
   char             fillerC;
   unsigned long    product_serial;
   unsigned long    pin_number;
   short            exec_limit;
   short            exp_days_limit;
   char             expire_date[9];
   unsigned char    user_data[16];
   unsigned char    lasered_id[6];
} SCB;

extern short far pascal KECHK (SCB FAR *);

