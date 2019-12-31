#ifndef PROCESSORINFO_H
#define PROCESSORINFO_H
#include <Windows.h>
#include <stdio.h>

class CProcessorInfo
{
protected:

	SYSTEM_INFO m_sysInfo;

public:
	CProcessorInfo(void)
	{
		::GetSystemInfo(&m_sysInfo);
	}

	virtual ~CProcessorInfo(void)
	{
	}

	char *GetProcessorName(char *sRC, size_t size)
	{
		char sSpeed[256];
		char sVendor[256];

		// Get the processor speed info.
		HKEY hKey;
		LONG result = ::RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			_T("Hardware\\Description\\System\\CentralProcessor\\0"),
			0, KEY_QUERY_VALUE, &hKey);

		// Check if the function has succeeded.
		if (result == ERROR_SUCCESS) {
			DWORD data;
			DWORD dataSize = sizeof(data);
			result = ::RegQueryValueEx(hKey, _T("~MHz"), NULL, NULL,
				(LPBYTE)&data, &dataSize);

			if (result == ERROR_SUCCESS) {
				_snprintf(sSpeed, sizeof(sSpeed)-1, ", Speed: %dMHz, ", data);
			} else {
				_snprintf(sSpeed, sizeof(sSpeed)-1, ", Speed: Unknown, ");
			}

			TCHAR vendorData [64];
			dataSize = sizeof (vendorData);

			result = ::RegQueryValueEx (hKey, _T("VendorIdentifier"),
				NULL, NULL, (LPBYTE)vendorData, &dataSize);

			if (result == ERROR_SUCCESS) {
				sprintf(sVendor, _T("Vendor: %s"), vendorData);
			} else {
				sprintf(sVendor, _T("Vendor: Unknown"));
			}
			/* Make sure to close the reg key */
			RegCloseKey (hKey);
		} else {
			strcpy(sVendor, _T("Vendor: Unknown, "));
			strcpy(sSpeed, _T("Speed: Unknown, "));
		}


		char sType[256];

		switch (m_sysInfo.dwProcessorType) {
		case PROCESSOR_INTEL_386:
			strcpy(sType, _T("Type: Intel 386, "));
			break;
		case PROCESSOR_INTEL_486:
			strcpy(sType, _T("Type: Intel 486, "));
			break;
		case PROCESSOR_INTEL_PENTIUM:
			strcpy(sType, _T("Type: Intel Pentium compatible, "));
			break;
		case PROCESSOR_MIPS_R4000:
			strcpy(sType, _T("Type: MIPS, "));
			break;
		case PROCESSOR_ALPHA_21064:
			strcpy(sType, _T("Type: Alpha, "));
			break;
		default:
			strcpy(sType, _T("Type: Unknown, "));
			break;
		}

		char sProcessors[256];
		_snprintf(sProcessors, sizeof(sProcessors),
			_T("Number Of Processors: %lu, "),
			m_sysInfo.dwNumberOfProcessors);

		char sArchitecture[256];
		char sProcessorLevel[256];
		char sStepping[256];

		switch(m_sysInfo.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_INTEL:
			strcpy(sArchitecture, _T("Architecture: Intel, "));
			switch (m_sysInfo.wProcessorLevel) {
			case 3:
				strcpy(sProcessorLevel, _T("Level: 80386, "));
				{
					int iSteppingLevel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping, _T("Stepping: %c%u"), iSteppingLevel, iStepping);
				}
				break;
			case 4:
				strcpy(sProcessorLevel, _T("Level: 80486, "));
				{
					int iSteppingLevel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping, _T("Stepping: %c%u"), iSteppingLevel, iStepping);
				}
				break;
			case 5:
				strcpy(sProcessorLevel, _T("Level: Pentium, "));
				{
					typedef BOOL (*PIPFP)(DWORD);
					PIPFP lpfn = (PIPFP)::GetProcAddress(GetModuleHandle( _T("kernel32.dll") ),
						"IsProcessorFeaturePresentA");
					if (lpfn) {
						if ((lpfn)(PF_MMX_INSTRUCTIONS_AVAILABLE)) {
							strcat(sProcessorLevel, _T (" MMX"));
						}
					}

					int iModel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping,  _T("Stepping: %u-%u"), iModel, iStepping);
				}
				break;
			case 6:
				strcpy(sProcessorLevel, _T("Level: Pentium II/Pro, "));
				{
					int iModel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping,  _T("Stepping: %u-%u"), iModel, iStepping);
				}
				break;
			default:
				sprintf(sProcessorLevel,  _T("Level: Unknown %u, "),
					m_sysInfo.wProcessorLevel);
				{
					int iModel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping,  _T("Stepping: %u-%u"), iModel, iStepping);
				}
				break;
			}
			break;
		case PROCESSOR_ARCHITECTURE_MIPS:
			strcpy(sArchitecture, "Architecture: MIPS, ");
			switch(m_sysInfo.wProcessorLevel) {
			case 0004:
				strcpy(sProcessorLevel, "Level: R4000, ");
				break;
			default:
				sprintf(sProcessorLevel,  _T("Level: Unknown %u, "),
					m_sysInfo.wProcessorLevel);
				break;
			}
			sprintf(sStepping,  _T("Stepping: 00%u"), m_sysInfo.wProcessorRevision);
			break;
		case PROCESSOR_ARCHITECTURE_ALPHA:
			strcpy(sArchitecture, "Architecture: Alpha, ");
			sprintf(sProcessorLevel, _T("Level: %u, "), m_sysInfo.wProcessorLevel);
			{
				int iModel = m_sysInfo.wProcessorRevision / 100;
				int iStepping = m_sysInfo.wProcessorRevision % 100;
				sprintf(sStepping, _T("Stepping: %c%u"), iModel, iStepping);
			}
			break;
		case PROCESSOR_ARCHITECTURE_PPC:
			strcpy(sArchitecture, _T("Architecture: PowerPC, "));
			switch(m_sysInfo.wProcessorLevel) {
			case 1:
				strcpy(sProcessorLevel, _T("Level: 601, "));
				break;
			case 3:
				strcpy(sProcessorLevel, _T("Level: 603, "));
				break;
			case 4:
				strcpy(sProcessorLevel, _T("Level: 604, "));
				break;
			case 6:
				strcpy(sProcessorLevel, _T("Level: 603+, "));
				break;
			case 9:
				strcpy(sProcessorLevel, _T("Level: 604+, "));
				break;
			case 20:
				strcpy(sProcessorLevel, _T("Level: 620, "));
				break;
			default:
				sprintf(sProcessorLevel, _T("Level: Unknown %u, "),
					m_sysInfo.wProcessorLevel);
				break;
			}
			{
				int iModel = m_sysInfo.wProcessorRevision / 100;
				int iStepping = m_sysInfo.wProcessorRevision % 100;
				sprintf(sStepping, _T("Stepping: %u.%u"), iModel, iStepping);
			}
			break;
			case PROCESSOR_ARCHITECTURE_UNKNOWN:
				strcpy(sArchitecture, "Architecture: Unknown, ");
				sprintf(sProcessorLevel, _T("Level: Unknown %u, "),
					m_sysInfo.wProcessorLevel);
				{
					int iModel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping, _T("Stepping: %u-%u"), iModel, iStepping);
				}
				break;
			default:
				sprintf(sArchitecture, _T("Architecture: Unknown %u, "),
					m_sysInfo.wProcessorArchitecture);
				sprintf(sProcessorLevel, _T("Level: Unknown %u, "),
					m_sysInfo.wProcessorLevel);
				{
					int iModel = m_sysInfo.wProcessorRevision / 100;
					int iStepping = m_sysInfo.wProcessorRevision % 100;
					sprintf(sStepping, _T("Stepping: %u-%u"), iModel, iStepping);
				}
				break;
		}

		_snprintf(sRC, size, "%s%s%s%s%s%s%s",
			sVendor, sSpeed, sType, sProcessors,
			sArchitecture, sProcessorLevel, sStepping);
		return sRC;
	}
};

class CMemoryInfo
{
protected:

public:

   CMemoryInfo(void)
   {
   }

   char *GetMemoryInfo(char *sRC, size_t size)
   {
	   MEMORYSTATUS memoryStatus;

	   memset (&memoryStatus, 0, sizeof(MEMORYSTATUS));
	   memoryStatus.dwLength = sizeof (MEMORYSTATUS);
	   GlobalMemoryStatus (&memoryStatus);

      DWORD dwMinWSSize;
      DWORD dwMaxWSSize;

      ::GetProcessWorkingSetSize(GetCurrentProcess(), &dwMinWSSize, &dwMaxWSSize);

      _snprintf(sRC, size,  _T("Memory Used %lu%%, Total Physical Memory %luKB, Physical Memory Available %luKB, Total Virtual Memory %luKB, Available Virtual Memory %luKB, Working Set Min: %luKB Max: %luKB .\r\n"), memoryStatus.dwMemoryLoad, memoryStatus.dwTotalPhys / 1024, memoryStatus.dwAvailPhys / 1024, memoryStatus.dwTotalVirtual / 1024, memoryStatus.dwAvailVirtual / 1024, dwMinWSSize/1024, dwMaxWSSize/1024);

      return sRC;
   }

};

#endif

