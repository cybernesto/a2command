/**************************************************************
Copyright (c) 2010-2013, Payton Byrd
All rights reserved.

Redistribution and use in source and binary forms, with or 
without modification, are permitted provided that the following 
conditions are met:

* Redistributions of source code must retain the above 
  copyright notice, this list of conditions and the following 
  disclaimer.

* Redistributions in binary form must reproduce the above 
  copyright notice, this list of conditions and the following 
  disclaimer in the documentation and/or other materials 
  provided with the distribution.

* Neither the name of A2-Command Team nor the names of its 
  contributors may be used to endorse or promote products 
  derived from this software without specific prior written 
  permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

#include "constants.h"
#include "drives.h"
#include "globals.h"
#include "globalInput.h"
#include "input.h"
#include "menus.h"
#include "screen.h"
#include "Viewer.h"
#include "A2-disks.h"
#include "Dispatcher.h"
#include "HexEditor.h"
#include "BasicViewer.h"
#include "Formatter.h"

void  readKeyboard(void)
{
	struct dir_node *currentNode;
	unsigned char key;
	bool decision = false;

	key = toupper(cgetc());

	switch((int)key)
	{
	case HK_FORMATTER:
		if(loadOverlay(7))
		{
			formatDisk(selectedPanel);

			clrscr();
			writeMenuBar();
			reloadPanels();
		}
		break;
	case HK_BASIC_VIEWER:
		if(loadOverlay(6))
		{
			viewFileAsBASIC(selectedPanel);

			clrscr();
			writeMenuBar();
			reloadPanels();
		}
		break;
	case HK_HEX_EDIT:
		if(loadOverlay(5))
		{
			hexEditCurrentFile(selectedPanel);

			clrscr();
			writeMenuBar();
			reloadPanels();
		}
		break;

	case CH_ENTER:
		currentNode = getSelectedNode(selectedPanel);
		if(isDirectory(selectedPanel))
		{
			enterDirectory(selectedPanel);
		}
		else if(currentNode != NULL)
		{
			sprintf(filePath, "%s/%s", selectedPanel->path, currentNode->name);
			if(currentNode->type == 0x06
				|| currentNode->type == 0xFF)
			{
				saveScreen();
				decision = writeYesNo("Confirm", quit_message, 1);
				retrieveScreen();

				if(decision == true)
				{
					exec(filePath, NULL);
				}
			}
			else if(currentNode->type == 0xFC)
			{
				if(loadOverlay(6))
				{
					viewFileAsBASIC(selectedPanel);

					clrscr();
					writeMenuBar();
					reloadPanels();
				}
			}
			else
			{
				if(loadOverlay(1))
					viewFile(filePath);
			}
		}
		break;
	case KEY_F4:
		rereadSelectedPanel();
		break;
	case KEY_F3:
		selectDrive(selectedPanel);
		rereadSelectedPanel();
		break;
	case HK_SELECT:
		selectCurrentFile();
		break;
#ifdef __APPLE2ENH__
		case CH_CURS_UP:
#else
		case CH_CURS_LEFT:
#endif
		moveSelectorUp(selectedPanel);
		break;
#ifdef __APPLE2ENH__
		case CH_CURS_DOWN:
#else
		case CH_CURS_RIGHT:
#endif
		moveSelectorDown(selectedPanel);
		break;
#ifdef __APPLE2ENH__
	case CH_CURS_LEFT:
		if(selectedPanel == &rightPanelDrive
			&& strlen(leftPanelDrive.path) > 0)
		{
			selectedPanel = &leftPanelDrive;
			writeSelectorPosition(&leftPanelDrive, '>');
			writeSelectorPosition(&rightPanelDrive, ' ');
			writeCurrentFilename(selectedPanel);
		}
		break;
	case CH_CURS_RIGHT:
		if(selectedPanel == &leftPanelDrive
			&& strlen(rightPanelDrive.path) > 0)
		{
			selectedPanel = &rightPanelDrive;
			writeSelectorPosition(&leftPanelDrive, ' ');
			writeSelectorPosition(&rightPanelDrive, '>');
			writeCurrentFilename(selectedPanel);
		}
		break;
#endif
	case HK_SWITCH_PANEL:
		if(selectedPanel == &leftPanelDrive
			&& strlen(rightPanelDrive.path) > 0)
		{
			selectedPanel = &rightPanelDrive;
			writeSelectorPosition(&leftPanelDrive, ' ');
			writeSelectorPosition(&rightPanelDrive, '>');
			writeCurrentFilename(selectedPanel);
		}
		else if(selectedPanel == &rightPanelDrive
			&& strlen(leftPanelDrive.path) > 0)
		{
			selectedPanel = &leftPanelDrive;
			writeSelectorPosition(&leftPanelDrive, '>');
			writeSelectorPosition(&rightPanelDrive, ' ');
			writeCurrentFilename(selectedPanel);
		}
		break;

	case KEY_SH_PLUS:
		enterDirectory(selectedPanel);
		break;
	case KEY_SH_MINUS:
	case CH_ESC:
		leaveDirectory(selectedPanel);
		break;
	//case 188: // C= C - Command Menu
	//	writeMenu(command);
	//	break;
	//case 182: // C= L - Left Menu
	//	writeMenu(left);
	//	break;
	//case 178: // C= R - Right Menu
	//	writeMenu(right);
	//	break;
	//case 187: // C= F - File Menu
	//	writeMenu(file);
	//	break;
	//case 185: // C= O - Options Menu
	//	writeMenu(options);
	//	break;
	case HK_REREAD_LEFT:
		rereadDrivePanel(left);
		break;
	case HK_REREAD_RIGHT:
		rereadDrivePanel(right);
		break;
	case HK_DRIVE_LEFT:
		writeDriveSelectionPanel(left);
		break;
	case HK_DRIVE_RIGHT:
		writeDriveSelectionPanel(right);
		break;
	case HK_SELECT_ALL:
		selectAllFiles(selectedPanel, true);
		break;
	case HK_DESELECT_ALL:
		selectAllFiles(selectedPanel, false);
		break;
	case KEY_F1:
		if(loadOverlay(1))
			writeHelpPanel();
		break;
	case KEY_F2:
		quit();
		break;
	case KEY_F5:
		if(loadOverlay(4))
			copyFiles();
		break;
	case HK_RENAME:
	case KEY_F6:
		if(loadOverlay(4))
			renameFile();
		break;
	case HK_DELETE:
	case KEY_F8:
		if(loadOverlay(4))
			deleteFiles();
		break;
	//case KEY_AT:
	//	inputCommand();
	//	break;
	case KEY_F7:
		if(loadOverlay(4))
			makeDirectory();
		break;
	case HK_TO_TOP:
		moveTop(selectedPanel);
		break;
	case HK_TO_BOTTOM:
		moveBottom(selectedPanel);
		break;
	case HK_PAGE_UP:
		movePageUp(selectedPanel);
		break;
	case HK_PAGE_DOWN:
		movePageDown(selectedPanel);
		break;
	case HK_WRITE_DISK_IMAGE:
		if(loadOverlay(3))
			writeDiskImage();
		break;
	case HK_CREATE_DISK_IMAGE:
		if(loadOverlay(3))
			createDiskImage();
		break;
	case HK_COPY_DISK:
		if(loadOverlay(2))
			copyDisk();
		break;
	default:
		//writeStatusBarf("%c", key);
		break;
	}
}
