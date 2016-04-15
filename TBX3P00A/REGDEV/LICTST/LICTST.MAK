# Microsoft Visual C++ generated build script - Do not modify

PROJ = LICTST
DEBUG = 1
PROGTYPE = 0
CALLER = 
ARGS = 
DLLS = 
D_RCDEFINES = -d_DEBUG
R_RCDEFINES = -dNDEBUG
ORIGIN = MSVC
ORIGIN_VER = 1.00
PROJPATH = E:\BETDEV\TBXDEV\REGDEV\LICTST\
USEMFC = 0
CC = cl
CPP = cl
CXX = cl
CCREATEPCHFLAG = 
CPPCREATEPCHFLAG = 
CUSEPCHFLAG = 
CPPUSEPCHFLAG = 
FIRSTC = LICTST.C    
FIRSTCPP =             
RC = rc
CFLAGS_D_WEXE = /nologo /G2 /W3 /Zi /AM /Od /D "_DEBUG" /D "W31" /FR /GA /Fd"LICTST.PDB"
CFLAGS_R_WEXE = /nologo /W3 /AM /O1 /D "NDEBUG" /D "W31" /FR /GA 
LFLAGS_D_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /CO /ALIGN:16 /STACK:10240
LFLAGS_R_WEXE = /NOLOGO /ONERROR:NOEXE /NOD /PACKC:61440 /ALIGN:16 /STACK:10240
LIBS_D_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
LIBS_R_WEXE = oldnames libw commdlg shell olecli olesvr mlibcew
RCFLAGS = /nologo
RESFLAGS = /nologo
RUNFLAGS = 
DEFFILE = LICTST.DEF
OBJS_EXT = 
LIBS_EXT = 
!if "$(DEBUG)" == "1"
CFLAGS = $(CFLAGS_D_WEXE)
LFLAGS = $(LFLAGS_D_WEXE)
LIBS = $(LIBS_D_WEXE)
MAPFILE = nul
RCDEFINES = $(D_RCDEFINES)
!else
CFLAGS = $(CFLAGS_R_WEXE)
LFLAGS = $(LFLAGS_R_WEXE)
LIBS = $(LIBS_R_WEXE)
MAPFILE = nul
RCDEFINES = $(R_RCDEFINES)
!endif
!if [if exist MSVC.BND del MSVC.BND]
!endif
SBRS = LICTST.SBR \
		TBXLIC.SBR \
		WINMEM.SBR \
		WINMSG.SBR \
		CFGSUP.SBR \
		FILLOW.SBR


LICTST_DEP = e:\betdev\tbxdev\regdev\lictst\lictst.h \
	e:\betdev\tbxdev\regdev\lictst\tbxlic.h


LICTST_RCDEP = e:\betdev\tbxdev\regdev\lictst\lictst.ico


TBXLIC_DEP = e:\betdev\tbxdev\os_dev\winmem.h \
	e:\betdev\tbxdev\os_dev\winmsg.h \
	e:\betdev\tbxdev\regdev\lictst\visdef.h \
	e:\betdev\tbxdev\os_dev\dosmem.h \
	e:\betdev\tbxdev\os_dev\dosmsg.h \
	e:\betdev\tbxdev\fiodev\filutl.h \
	e:\betdev\tbxdev\os_dev\cfgsup.h \
	e:\betdev\tbxdev\regdev\lictst\tbxlic.h \
	e:\betdev\tbxdev\regdev\lictst\licsup.h \
	e:\betdev\tbxdev\regdev\lictst\licdlg.h \
	e:\betdev\tbxdev\regdev\lictst\lictst.h \
	e:\betdev\tbxdev\regdev\lictst\licdlg.c \
	e:\betdev\tbxdev\regdev\lictst\licsup.c


WINMEM_DEP = e:\betdev\tbxdev\os_dev\winmem.h \
	e:\betdev\tbxdev\os_dev\winmsg.h


WINMSG_DEP = e:\betdev\tbxdev\os_dev\winmsg.h


CFGSUP_DEP = e:\betdev\tbxdev\os_dev\winmem.h \
	e:\betdev\tbxdev\os_dev\dosmem.h \
	e:\betdev\tbxdev\fiodev\filutl.h \
	e:\betdev\tbxdev\os_dev\cfgsup.h


FILLOW_DEP = e:\betdev\tbxdev\fiodev\filutl.h \
	e:\betdev\tbxdev\fiodev\bitrev.tab


all:	$(PROJ).EXE $(PROJ).BSC

LICTST.OBJ:	LICTST.C $(LICTST_DEP)
	$(CC) $(CFLAGS) $(CCREATEPCHFLAG) /c LICTST.C

LICTST.RES:	LICTST.RC $(LICTST_RCDEP)
	$(RC) $(RCFLAGS) $(RCDEFINES) -r LICTST.RC

TBXLIC.OBJ:	TBXLIC.C $(TBXLIC_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c TBXLIC.C

WINMEM.OBJ:	..\..\OS_DEV\WINMEM.C $(WINMEM_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\OS_DEV\WINMEM.C

WINMSG.OBJ:	..\..\OS_DEV\WINMSG.C $(WINMSG_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\OS_DEV\WINMSG.C

CFGSUP.OBJ:	..\..\OS_DEV\CFGSUP.C $(CFGSUP_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\OS_DEV\CFGSUP.C

FILLOW.OBJ:	..\..\FIODEV\FILLOW.C $(FILLOW_DEP)
	$(CC) $(CFLAGS) $(CUSEPCHFLAG) /c ..\..\FIODEV\FILLOW.C


$(PROJ).EXE::	LICTST.RES

$(PROJ).EXE::	LICTST.OBJ TBXLIC.OBJ WINMEM.OBJ WINMSG.OBJ CFGSUP.OBJ FILLOW.OBJ $(OBJS_EXT) $(DEFFILE)
	echo >NUL @<<$(PROJ).CRF
LICTST.OBJ +
TBXLIC.OBJ +
WINMEM.OBJ +
WINMSG.OBJ +
CFGSUP.OBJ +
FILLOW.OBJ +
$(OBJS_EXT)
$(PROJ).EXE
$(MAPFILE)
E:\C151\CDK16\LIB\+
e:\c151\lib\+
e:\c151\mfc\lib\+
$(LIBS)
$(DEFFILE);
<<
	link $(LFLAGS) @$(PROJ).CRF
	$(RC) $(RESFLAGS) LICTST.RES $@
	@copy $(PROJ).CRF MSVC.BND

$(PROJ).EXE::	LICTST.RES
	if not exist MSVC.BND 	$(RC) $(RESFLAGS) LICTST.RES $@

run: $(PROJ).EXE
	$(PROJ) $(RUNFLAGS)


$(PROJ).BSC: $(SBRS)
	bscmake @<<
/o$@ $(SBRS)
<<
