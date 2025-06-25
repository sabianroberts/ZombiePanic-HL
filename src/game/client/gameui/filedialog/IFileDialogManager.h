// ============== Copyright (c) 2025 Monochrome Games ============== \\

#ifndef GAMEUI_FILEDIALOG_MANAGER_H
#define GAMEUI_FILEDIALOG_MANAGER_H

enum OpenFileDialog_e
{
	FILE_ANY = 0,		// Any file
	FILE_TGA,			// .tga files
	FILE_JPG,			// .jpg files
	FILE_BSP,			// .bsp files
	FILE_TXT,			// .txt files
	FOLDER				// Folders only
};

// File selected dialog data
struct DialogData
{
	std::string File;
	std::string FileExtension;
	std::string LocalPath;
	std::string LocalGamePath;
	std::string FullPath;
	bool IsFolder;
};

// The function we will fire (if valid), once we have selected our
// file or folder.
typedef void (*DialogSelected_t)( DialogData *pData );

class IFileDialogManager
{
public:
	virtual void OpenFileBrowser( OpenFileDialog_e eFilter, const char *szFolder, const char *szPathID, DialogSelected_t pFunction ) = 0;
};

extern IFileDialogManager *g_pFileDialogManager;

#endif