#
#   SEGA SATURN Graphic library make file for GNU

# macro
CC = sh-elf-gcc
AS = sh-elf-as
LD = sh-elf-ld
RM = rm
CONV = sh-elf-objcopy

# directory
SGLDIR = /saturn/SBL6/SEGALIB
SGLIDR = $(SGLDIR)/inc
SGLLDR = $(SGLDIR)/lib
GFS = l:/saturn/SBL6/SEGALIB/GFS2/gfs2.c l:/saturn/SBL6/SEGALIB/GFS2/gfs_cdb2.c l:/saturn/SBL6/SEGALIB/GFS2/gfs_cdc2.c l:/saturn/SBL6/SEGALIB/GFS2/gfs_cdf2.c \
l:/saturn/SBL6/SEGALIB/GFS2/gfs_trn2.c l:/saturn/SBL6/SEGALIB/GFS2/gfs_buf2.c l:/saturn/SBL6/SEGALIB/GFS2/gfs_dir2.c 

CMNDIR = ../common
####OBJECTS = ./saturn/obj
OBJECTS = ./objects

# option
#-mspace 

#CCFLAGS = -m2 -O4 -c  -I. -I./cpu
#CCFLAGS = -m2 -O3 -fomit-frame-pointer -c  -I. -I./cpu -Il:/saturn/SBL6/SEGASMP/PER/SMPCLIB  

#CCFLAGS = -m2 -O2 -mrelax -fomit-frame-pointer -foptimize-sibling-calls  -funit-at-a-time -fstrength-reduce -fexpensive-optimizations -fsort-data -c -Wanalyze -Wanalyze-rule=24 -Il:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 
#VBT(randylin advice) -funit-at-a-time et compilation complete=> tout est optimisé
#CCFLAGS = -m2 -O2  -mrelax -foptimize-sibling-calls  -fstrength-reduce -fexpensive-optimizations -fsort-data -c  -Il:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 
CCFLAGS = -m2 -O2 -falign-functions=4 -fpeel-loops  -Wall -Wl,-fuse-ld=gold -fno-lto -ftree-partial-pre -fweb -fno-common -fsingle-precision-constant -fno-printf-return-value -fmerge-all-constants -ffast-math --param max-inline-insns-single=500 -fms-extensions -freorder-blocks-algorithm=simple -fno-align-loops -Wno-array-bounds -Wno-missing-braces -Wextra -fno-web -Wl,-v -funit-at-a-time -Wl,--strip-all -Wl,--verbose -Wl,--allow-multiple-definition -mno-fsrra -maccumulate-outgoing-args -std=gnu99 -Wfatal-errors -fomit-frame-pointer -D_SH -DMODEL_S -c -Il:/saturn/SBL6/SEGALIB/INCLUDE  -I. -I./cpu 
#CCFLAGS = -m2 -O2 -c  -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 
#CCFLAGS = -m2 -Os  -c  -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 
#CCFLAGS = -m2 -Os -c  -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 
							   #-Wanalyze
#CCFLAGS = -m2 -S -O2 -mrelax -fomit-frame-pointer -foptimize-sibling-calls  -fstrength-reduce -fexpensive-optimizations -fsort-data -Wanalyze -Wanalyze-rule=24 -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 

#CCFLAGS = -m2 -O2 -fomit-frame-pointer  -foptimize-sibling-calls  -fstrength-reduce -floop-optimize2 -fexpensive-optimizations -c -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu 
#CCFLAGS = -m2 -O -c -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu
#CCFLAGS = -c -IH:/saturn/SBL6/SEGASMP/PER/SMPCLIB  -I. -I./cpu
# 
#-I$(SGLIDR)

# -m2 must be specified in LDFLAGS so the linker will search the SH2 lib dirs
# Specify path of libsgl.a by using "-L" option

LDFLAGS = -m2  -O2 -L$(SGLLDR) -Xlinker -T$(LDFILE) -Xlinker -Map \
          -Xlinker $(MPFILE) -Xlinker -e -Xlinker START -nostartfiles
DFLAGS =
# source_program
include $(OBJECTS)

TARGET   = root/sl.coff
TARGET1  = root/sl.bin
LDFILE	 = ./$(TARGET:.coff=.lnk)
MPFILE   = $(TARGET:.coff=.maps)
MAKEFILE = Makefile

all: $(TARGET) $(TARGET1)
####all: $(OBJECTS)/z80.a  #$(TARGET) $(TARGET1)
####$(OBJECTS)/z80.a	: $(OBJECTS)/z80.oa

####$(OBJECTS)/z80.oa : cpu/z80.c cpu/z80.h
####	$(CC) -c cpu/z80.c -o $(OBJECTS)/z80.oa $(FLAGS)$(SYSOBJS) $(OBJS) $(LIBS) 
####	$(AR) rs $(OBJECTS)/z80.a $(OBJECTS)/z80.oa

# Use gcc to link so it will automagically find correct libs directory

#$(TARGET) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(OBJECTS) $(LDFILE)
#	$(CC) $(LDFLAGS) $(SYSOBJS) $(OBJS) $(LIBS) -o $@

$(TARGET) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS) $(SYSOBJS) $(OBJS) $(LIBS) -o $@


$(TARGET1) : $(SYSOBJS) $(OBJS) $(MAKEFILE) $(LDFILE)
	$(CONV) -O binary $(TARGET) $(TARGET1)

$(LDFILE) : $(MAKEFILE)
#	@echo Making $(LDFILE)
#	@echo SECTIONS {		> $@
#	@echo 	SLSTART 0x06004000 : {	>> $@
#	@echo 		___Start = .;	>> $@
#	@echo 		*(SLSTART)	>> $@
#	@echo 	}			>> $@
#	@echo 	.text ALIGN(0x20) :			>> $@
#	@echo 	{			>> $@
#	@echo 		* (.text)			>> $@
#	@echo 		*(.strings)			>> $@
#	@echo 		__etext = .;			>> $@
#	@echo 	}			>> $@
#	@echo 	SLPROG ALIGN(0x20): {	>> $@
#	@echo 		__slprog_start = .;	>> $@
#	@echo 		*(SLPROG)	>> $@
#	@echo 		__slprog_end = .;	>> $@
#	@echo 	}			>> $@
#	@echo 	.tors  ALIGN(0x10) :			>> $@
#	@echo 	{			>> $@
#	@echo 		___ctors = . ;			>> $@
#	@echo 		*(.ctors)			>> $@
#	@echo 		___ctors_end = . ;			>> $@
#	@echo 		___dtors = . ;			>> $@
#	@echo 		*(.dtors)			>> $@
#	@echo 		___dtors_end = . ;			>> $@
#	@echo 	}			>> $@
#	@echo 	.data ALIGN(0x10):			>> $@
#	@echo 	{			>> $@
#	@echo 		* (.data)			>> $@
#	@echo 		_edata = . ;			>> $@
#	@echo 	}			>> $@
#	@echo 	.bss ALIGN(0x10) (NOLOAD):			>> $@
#	@echo 	{			>> $@
#	@echo 		__bstart = . ;			>> $@
#	@echo 		*(.bss)			>> $@
#	@echo 		* ( COMMON )			>> $@
#	@echo 		__bend = . ;			>> $@
#	@echo 	_end = .;			>> $@
#	@echo 	}			>> $@
#	@echo }				>> $@

# suffix
.SUFFIXES: .asm

.s.o:
	$(AS) $< $(ASFLAGS) $(_ASFLAGS) -o $@

.c.o:
	$(CC) $< $(DFLAGS) $(CCFLAGS) -o $@


clean:
	$(RM) $(OBJS) $(TARGET:.coff=.*)
