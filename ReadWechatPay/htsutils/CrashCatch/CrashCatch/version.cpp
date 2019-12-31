#include <stdio.h>
#include <tchar.h>
#include "version.h"
#include <Windows.h>

#pragma comment(lib, "version.lib")
#pragma warning(disable:4996)

static void GetProgramFile(char *buf, size_t size)
{
	GetModuleFileName(0, buf, (DWORD) size);
}

char *GetVersionString(char *version, size_t size)
{
	char  fullpath[MAX_PATH], *str;

	/* Fill the version info */
	GetProgramFile(fullpath, sizeof(fullpath));

	str = strdup(fullpath);

	DWORD tmp=0;
	DWORD len=GetFileVersionInfoSize(str, &tmp);
	if (!len) {
		strncpy(version, "Version", size);
		return (version);
	}
	void *pBlock = malloc(len);
	GetFileVersionInfo(str, 0, len, pBlock);
	void *ptr;
	UINT ptrlen;

	char *ProductName;
	/* Retreive the product name */
	
	TCHAR SubBlock[50];
			
	// Structure used to store enumerated languages and code pages.
	struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;

	UINT cbTranslate;

	/* Read the list of languages and code pages. */
	if (VerQueryValue(pBlock, 
				TEXT("\\VarFileInfo\\Translation"),
				(LPVOID*)&lpTranslate,
				&cbTranslate)) {
		/* Read the file description for each language and code page. */
	
		_snprintf(SubBlock, sizeof(SubBlock),
	           _T("\\StringFileInfo\\%04x%04x\\ProductName"),
	           lpTranslate[0].wLanguage,
	           lpTranslate[0].wCodePage);
		// Retrieve file description for language and code page "0". 
		if (VerQueryValue(pBlock, 
				SubBlock, 
				&ptr, 
				&ptrlen)) {
			ProductName = (TCHAR*) ptr;
		}
	}

	/* Format the versionstring */
	if (VerQueryValue(pBlock, _T("\\"), &ptr, &ptrlen)) {
		VS_FIXEDFILEINFO *fi=(VS_FIXEDFILEINFO*)ptr;
		unsigned __int64 curver=(((__int64)fi->dwFileVersionMS)<<32)+fi->dwFileVersionLS;
		
		if (fi->dwFileVersionMS>>16) {
			/* v1.00+ */
			if ((fi->dwFileVersionLS&0xFFFF)%100) { /* betas */
				if (fi->dwFileVersionLS>>16)
					_snprintf(version, size, _T("%s version %d.%d.%d beta %d"),
						ProductName, fi->dwFileVersionMS>>16,
						fi->dwFileVersionMS&0xFFFF,
						fi->dwFileVersionLS>>16,
						(fi->dwFileVersionLS&0xFFFF)%100);
				else
					_snprintf(version, size,
						_T("%s version %d.%d beta %d"),
						ProductName,
						fi->dwFileVersionMS>>16,
						fi->dwFileVersionMS&0xFFFF,
						(fi->dwFileVersionLS&0xFFFF)%100);
			} else { /* final versions */
				if ((fi->dwFileVersionLS&0xFFFF)/100) {
					if (fi->dwFileVersionLS>>16) /* minor releases */
						_snprintf(version, size,
							_T("%s version %d.%d.%d%c"),
							ProductName,
							fi->dwFileVersionMS>>16,
							fi->dwFileVersionMS&0xFFFF,
							fi->dwFileVersionLS>>16,
							'a'+(fi->dwFileVersionLS&0xFFFF)/100-1);
					else
						_snprintf(version, size,
							_T("%s version %d.%d%c"),
							ProductName,
							fi->dwFileVersionMS>>16,
							fi->dwFileVersionMS&0xFFFF,
							'a'+(fi->dwFileVersionLS&0xFFFF)/100-1);
				} else {
					if (fi->dwFileVersionLS>>16)
						_snprintf(version, size,
							_T("%s version %d.%d.%d"),
							ProductName,
							fi->dwFileVersionMS>>16,
							fi->dwFileVersionMS&0xFFFF,
							fi->dwFileVersionLS>>16);
					else
						_snprintf(version, size,
							_T("%s version %d.%d"),
							ProductName,
							fi->dwFileVersionMS>>16,
							fi->dwFileVersionMS&0xFFFF);
				}
			}
		} else { /* beta versions */
			if (fi->dwFileVersionLS&0xFFFF) /* beta */
				 _snprintf(version, size, _T("%s Beta %d.%d beta %d"),
					ProductName, fi->dwFileVersionMS&0xFFFF,
					fi->dwFileVersionLS>>16,fi->dwFileVersionLS&0xFFFF);
			else /* final version */
				_snprintf(version, size, _T("%s Beta %d.%d"),
					ProductName, fi->dwFileVersionMS&0xFFFF,
					fi->dwFileVersionLS>>16);
		}
			
	}

	free(str);
	free(pBlock);

	return (version);
}