#/**************************************************************
#Copyright (c) 2010, Payton Byrd
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or 
#without modification, are permitted provided that the following 
#conditions are met:
#
#* Redistributions of source code must retain the above 
#  copyright notice, this list of conditions and the following 
#  disclaimer.
#
#* Redistributions in binary form must reproduce the above 
#  copyright notice, this list of conditions and the following 
#  disclaimer in the documentation and/or other materials 
#  provided with the distribution.
#
#* Neither the name of A2-Command Team nor the names of its 
#  contributors may be used to endorse or promote products 
#  derived from this software without specific prior written 
#  permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
#CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
#INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
#MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
#CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
#SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
#NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
#LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
#HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
#CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
#OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
#EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#***************************************************************/

# Program declarations
CC := cc65
CA := ca65
LD := ld65
CL := cl65
T := apple2enh
JAVA := java
AC := AppleCommander-1.3.5.jar
DISK := a2command.po
MASTER_DISK := prodos.po
EMULATOR := C:/AppleWin/Applewin.exe -d1 $(CURDIR)/$(DISK)
JACE := java -jar /Applications/Apple\ II/jace.jar -computer.s6card disk -s6.d1 $(CURDIR)/$(DISK)

SOURCES	= \
	A2-Command.c \
	A2-disks.c \
	DriveMenu.c \
	drives.c \
	FileMenu.c \
	globals.c \
	input.c \
	menus.c \
	screen.c  \
	globalInput.c \
	Viewer.c \
    DiskImages.c \
    DiskCopy.c \
    Dispatcher.c \
    FileOPerations.c \
    HexEditor.c \
	BasicViewer.c \
	Formatter.c

ASMFILES = asm/HyperFormat-ca65.s



PROGRAM	=a2cmd
PROGRAMPLUS = a2cmp

TITLE	=$(PROGRAM)

LIBS	:=$(T).lib

# Default target - Builds for the Apple 2 Enhanced
all: $(PROGRAM)

A2Plus: T := apple2
A2Plus: LIBS :=$(T).lib
A2Plus: $(PROGRAMPLUS)

A2Debug: A2Disk
	$(EMULATOR)

A2JACE: A2Disk
	$(JACE)

A2Disk: all
	cp $(MASTER_DISK) $(DISK)
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system sys 0 < $(T)/$(PROGRAM).system
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.TV bin 0 < $(T)/$(PROGRAM).system.TV
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.DC bin 0 < $(T)/$(PROGRAM).system.DC
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.DI bin 0 < $(T)/$(PROGRAM).system.DI
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.FO bin 0 < $(T)/$(PROGRAM).system.FO
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.HE bin 0 < $(T)/$(PROGRAM).system.HE
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.BV bin 0 < $(T)/$(PROGRAM).system.BV
	$(JAVA) -jar $(AC) -p $(DISK) $(TITLE).system.FM bin 0 < $(T)/$(PROGRAM).system.FM

A2PlusDisk: T := apple2
A2PlusDisk: LIBS :=$(T).lib
A2PlusDisk: MASTER_DISK := prodos111.po
A2PlusDisk: DISK := a2cmdplus.po
A2PlusDisk: A2Plus
	cp $(MASTER_DISK) $(DISK)
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system sys 0 < $(T)/$(PROGRAMPLUS).system
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.TV bin 0 < $(T)/$(PROGRAMPLUS).system.TV
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.DC bin 0 < $(T)/$(PROGRAMPLUS).system.DC
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.DI bin 0 < $(T)/$(PROGRAMPLUS).system.DI
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.FO bin 0 < $(T)/$(PROGRAMPLUS).system.FO
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.HE bin 0 < $(T)/$(PROGRAMPLUS).system.HE
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.BV bin 0 < $(T)/$(PROGRAMPLUS).system.BV
	$(JAVA) -jar $(AC) -p $(DISK) $(PROGRAMPLUS).system.FM bin 0 < $(T)/$(PROGRAMPLUS).system.FM

$(T):
	mkdir $(T)

# Build our program
$(PROGRAM): $(SOURCES) $(ASMFILES) | $(T)
	$(CL) -O -t $(T) -C apple2enh-system.cfg -m $(T)/$(TITLE).map -o $(T)/$(PROGRAM).system $(SOURCES) $(ASMFILES) $(LIBS)

$(PROGRAMPLUS): $(SOURCES) $(ASMFILES) | $(T)
	$(CL) -O -t $(T) -C apple2enh-system.cfg -m $(T)/$(TITLE).map -o $(T)/$(PROGRAMPLUS).system $(SOURCES) $(ASMFILES) $(LIBS)

miniclean:
	$(RM) $(SOURCES:.c=.o) 
    # $(SOURCES:.c=.s)

# Our phony targets for managing the project
#.PHONY: clean
clean: miniclean
	$(RM) $(T)/$(PROGRAM).*
	$(RM) $(DISK)


A2PlusMiniClean: T := apple2
A2PlusMiniClean: DISK := a2cmdplus.po
A2PlusMiniClean:
	$(RM) $(SOURCES:.c=.o) 
    # $(SOURCES:.c=.s)

A2PlusClean:  A2PlusMiniClean
	$(RM) $(T)/$(PROGRAMPLUS).*
	$(RM) $(DISK)

both: A2Disk miniclean A2PlusDisk A2PlusMiniClean

cleanboth: clean A2PlusClean

payton: both
	cp a2command.po e:\a2command.po
	cp a2cmdplus.po e:\a2cmdplus.po
