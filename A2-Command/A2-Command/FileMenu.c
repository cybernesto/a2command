/**************************************************************
Copyright (c) 2010, Payton Byrd
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
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <errno.h>
#include <peekpoke.h>
#include <unistd.h>

#include "Configuration.h"
#include "constants.h"
#include "globalInput.h"
#include "globals.h"
#include "drives.h"
#include "input.h"
#include "menus.h"
#include "screen.h"

unsigned char *quit_message[1] =
{
	"Quit A2-Command?"
};
	

void  writeHelpPanel(void)
{
	writeStatusBar("See http://a2command.codeplex.com/documentation");
}

unsigned char fileBuffer[COPY_BUFFER_SIZE];
struct panel_drive *targetPanel = NULL, *tempPanel = NULL;
void  copyFiles(void)
{
	FILE *sourceFile = NULL, *targetFile = NULL;
	static unsigned char sourcePath[256], targetPath[256];
	unsigned char i = 0, j = 0, sd = 0, td = 0, bit = 0, r = 0;
	unsigned int index = 0, bytes = 0, readByte;
	unsigned RELOAD = false;
	unsigned long  bytesCopied = 0;
	static unsigned char targetFilename[21], type[2], status[40];
	struct dir_node *currentNode;

	targetPanel = selectedPanel == &leftPanelDrive ? &rightPanelDrive : &leftPanelDrive;

	if(strcmp(selectedPanel->path, targetPanel->path) == 0)
	{
		saveScreen();
		writeStatusBar("Cannot copy to the same path.");
		waitForEnterEsc();
		retrieveScreen();
		return;
	}

	for(i=0; i<selectedPanel->length / 8 + 1; ++i)
	{
		for(j=0; j<8; ++j)
		{
			bit = 1 << j;
			r = selectedPanel->selectedEntries[i] & bit;
			if(r != 0)
			{
				currentNode = getSpecificNode(selectedPanel, i*8+j);
				if(currentNode == NULL)
				{
					getDirectory(selectedPanel, i*8+j);
					currentNode = getSpecificNode(selectedPanel, i*8+j);
					{
						if(currentNode == NULL)
						{
							writeStatusBarf("Cannot get file %u", i*8+j); 
							waitForEnterEsc();
							return;
						}
					}
				}

				writeStatusBarf("Selected %s", currentNode->name);

				sprintf(sourcePath, "%s/%s", selectedPanel->path, currentNode->name);
				sourceFile = NULL;
				sourceFile = fopen(sourcePath, "rb");
				if(ferror(sourceFile) == 0)
				{
					sprintf(targetPath, "%s/%s", targetPanel->path, currentNode->name);
					targetFile = NULL;
					targetFile = fopen(targetPath, "wb");
					if(ferror(targetFile) == 0)
					{
						//for(index=0; index < currentNode->size; index+=(COPY_BUFFER_SIZE/254))
						bytesCopied = 0;
						while(true)
						{
							//bytes = fread(fileBuffer, COPY_BUFFER_SIZE, 1, sourceFile);
							readByte = fgetc(sourceFile);
							if(readByte == EOF)
							{
								writeStatusBarf("Reached end of %s", currentNode->name);
								break;
							}

							fileBuffer[0] = readByte;

							if(kbhit())
							{
								r = cgetc();
								if(r == CH_ESC)
								{
									fclose(sourceFile);
									fclose(targetFile);

									reloadPanels();

									writeStatusBar("Aborted copy.");
									return;
								}

							}

							r = fputc(fileBuffer[0], targetFile);
							if(r == -1)
							{
								writeStatusBarf("Problem (%d) writing %s", 
									_oserror, 
									currentNode->name); 
								waitForEnterEsc();
								break;
							}
							else
							{
								++bytesCopied;
							}
							//writeStatusBarf("%s - %d of %d.", currentNode->name, index, currentNode->size);
							if(bytesCopied % 100l == 0l)
							{
								fflush(targetFile);
								writeStatusBarf("%s - %u bytes copied.", currentNode->name, (unsigned)bytesCopied);
							}
						}
						RELOAD = true;

					}
					else
					{
						writeStatusBarf("Cannot open %s for write (%d)", 
							currentNode->name, r); 
						waitForEnterEsc();
					}
				}
				else
				{
					writeStatusBarf("Cannot open %s for read (%d)", 
						currentNode->name, r); 
					waitForEnterEsc();
				}
				fflush(targetFile);
				writeStatusBarf("%s - %u bytes copied.", currentNode->name, bytesCopied);
				fclose(sourceFile);
				fclose(targetFile);
			}
		}
	}

	if(RELOAD == true)
	{
		reloadPanels();
	}
}

void  reloadPanels(void)
{
	tempPanel = selectedPanel;
	selectedPanel = targetPanel;
	rereadSelectedPanel();
	selectedPanel = tempPanel;
	rereadSelectedPanel();
	writeSelectorPosition(selectedPanel, '>');
	writeSelectorPosition(targetPanel, ' ');
	writeCurrentFilename(selectedPanel);
}

unsigned char oldName[256], newName[256];
void  renameFile(void)
{
	enum results dialogResult;
	struct dir_node *selectedNode = NULL;
	unsigned char filename[17];
	unsigned char* dialogMessage[] =
	{
		{ "Enter new name" },
		{ "for file:" }
	};

	if(selectedPanel != NULL)
	{
		selectedNode = getSelectedNode(selectedPanel);
		if(selectedNode != NULL)
		{
			saveScreen();

			writeStatusBarf("Old name: %s", selectedNode->name);

			dialogResult = drawInputDialog(
				2, 16,
				dialogMessage,
				"Rename File",
				filename);

			retrieveScreen();

			if(dialogResult == OK_RESULT)
			{
				writeStatusBarf("Renaming to %s", filename);

				sprintf(oldName, "%s/%s", selectedPanel->path, selectedNode->name);
				sprintf(newName, "%s/%s", selectedPanel->path, filename);

				rename(oldName, newName);

				rereadSelectedPanel();

				writeStatusBarf("Renamed to %s", filename);
			}
		}
	}
}

void  makeDirectory(void)
{
	enum results dialogResult;
	struct dir_node *selectedNode = NULL;
	unsigned char command[40];
	unsigned char filename[17];
	unsigned char* dialogMessage[] =
	{
		{ "Enter name for" },
		{ "new directory:" }
	};

	if(selectedPanel != NULL)
	{
			saveScreen();

			dialogResult = drawInputDialog(
				2, 16,
				dialogMessage,
				"New Directory",
				filename);

			retrieveScreen();

			if(dialogResult == OK_RESULT)
			{
				strcpy(command, selectedPanel->path);
				strcat(command, filename);
				if(command[strlen(command)-1] != '/')
				{
					strcat(command, "/");
				}

				mkdir(command);

				getDirectory(selectedPanel, 
					selectedPanel->slidingWindowStartAt);
				displayDirectory(selectedPanel);
			}
	}
}

void  deleteFiles(void)
{
	const struct dir_node *selectedNode;
	unsigned i, k;
	unsigned char j;
	bool dialogResult, isBatch = false;
	static unsigned char* dialogMessage[] =
	{
		{ "Are you sure?" }
	};

	{
		for(i=0; i<(selectedPanel->length + (7 - 1)) / 8u; ++i)
		{
			for(j=0; j<8; ++j)
			{
				if ((selectedPanel->selectedEntries[i] & (1 << j)) != 0x00
					&& (k = i*8+j) < selectedPanel->length - 1)
				{
					isBatch = true;

					selectedNode = getSpecificNode(selectedPanel, k);
					if(selectedNode == NULL)
					{
						getDirectory(selectedPanel, k);
						selectedNode = getSpecificNode(selectedPanel, k);
					}

					sprintf(oldName, "%s/%s", selectedPanel->path, selectedNode->name);
					if (remove(oldName) < 0 ||
						// Let us change our minds, and stop a batch delete.
						kbhit() && cgetc() == CH_ESC)
					{
						rereadSelectedPanel();
						return;
					}
				}
			}
		}

		if(!isBatch)
		{
			// No names are highlighted; delete the current file.
			selectedNode = getSelectedNode(selectedPanel);
			if(selectedNode != NULL)
			{
				writeStatusBarf(
					"File to delete: %s",
					selectedNode->name);

				dialogResult = writeYesNo(
					"Delete",
					dialogMessage,
					1);

				retrieveScreen();
				writeCurrentFilename(selectedPanel);

				if(dialogResult)
				{
					sprintf(oldName, "%s/%s", selectedPanel->path, selectedNode->name);
					remove(oldName);
					rereadSelectedPanel();
				}
			}
		}
		else
		{
			rereadSelectedPanel();
		}
	}
}

void  quit(void)
{
	unsigned result;

	saveScreen();

	result = writeYesNo("Confirm", quit_message, 1);
	
	if(result == true)
	{
		writeStatusBar("Goodbye!");
		exit(EXIT_SUCCESS);
	}

	retrieveScreen();
}

void  writeAboutBox(void)
{
	writeStatusBarf("Thank You for using A2-Command Alpa");
}

void  executeSelectedFile(void)
{
	struct dir_node *currentNode;
	unsigned result;

	if(selectedPanel != NULL)
	{
		currentNode = getSelectedNode(selectedPanel);

		if(currentNode != NULL && currentNode->type == 2)
		{
			saveScreen();

			result = writeYesNo("Confirm", quit_message, 1);

			retrieveScreen();
	
			if(result == true)
			{
				clrscr();
				exit(EXIT_SUCCESS);
			}
		}
	}
}

void  inputCommand(void)
{
	enum results dialogResult;
	struct dir_node *selectedNode = NULL;
	unsigned char command[77];
	unsigned char* dialogMessage[] =
	{
		{ "Enter Command for drive:" }
	};

	if(selectedPanel != NULL)
	{
		selectedNode = getSelectedNode(selectedPanel);
		if(selectedNode != NULL)
		{
			saveScreen();

			dialogResult = drawInputDialog(
				1, 
				size_x - 4,
				dialogMessage,
				"Command",
				command
				);

			retrieveScreen();

			if(dialogResult == OK_RESULT)
			{
				sendCommand(selectedPanel, command);

				waitForEnterEsc();

				rereadSelectedPanel();				
			}
		}
	}
}
