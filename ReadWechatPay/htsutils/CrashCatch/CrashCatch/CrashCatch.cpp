#include <Windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <DbgHelp.h>
#include <TlHelp32.h>
#include "CrashCatch.h"

//char dumpFileName[_MAX_PATH] = {0};
char param[1024] = { 0 };
char url[1024] = { 0 };
char logProgram[_MAX_PATH] = {0};

#pragma warning(disable:4996)
#pragma warning(disable:4477)
#pragma warning(disable:4473)
#pragma warning(disable:4101)

#include "version.h"
#include "ProcessorInfo.h"
#include "crash_dump.h"

typedef BOOL (_stdcall *tSymFromAddr)(IN HANDLE hProcess, IN DWORD64 Address,
									  OUT PDWORD64 Displacement, IN OUT PSYMBOL_INFO Symbol);
typedef DWORD (_stdcall *tSymGetOptions)();
typedef DWORD (_stdcall *tSymSetOptions)(IN DWORD SymOptions);
typedef BOOL (_stdcall *tSymCleanup)(IN HANDLE hProcess);
typedef BOOL (_stdcall *tSymInitialize)(IN HANDLE hProcess,
										IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
typedef BOOL (_stdcall *tSymGetLineFromAddr)(IN HANDLE hProcess, IN DWORD dwAddr,
											 OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line);
typedef BOOL (_stdcall *tStackWalk)(DWORD MachineType, HANDLE hProcess,
									HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,
									PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
									PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
									PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
									PTRANSLATE_ADDRESS_ROUTINE TranslateAddress);
typedef PVOID (_stdcall *tSymFunctionTableAccess)(HANDLE hProcess, DWORD AddrBase);
typedef DWORD (_stdcall *tSymGetModuleBase)(IN HANDLE hProcess, IN DWORD dwAddr);
typedef BOOL (_stdcall *tMiniDumpWriteDump)(HANDLE hProcess, DWORD ProcessId,
											HANDLE hFile, MINIDUMP_TYPE DumpType,
											PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
											PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
											PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

static tSymCleanup				pSymCleanup;
static tSymInitialize			pSymInitialize;
static tSymGetOptions			pSymGetOptions;
static tSymSetOptions			pSymSetOptions;
static tSymGetLineFromAddr		pSymGetLineFromAddr;
static tSymFromAddr				pSymFromAddr;
static tStackWalk				pStackWalk;
static tSymFunctionTableAccess	pSymFunctionTableAccess;
static tSymGetModuleBase		pSymGetModuleBase;
static tMiniDumpWriteDump		pMiniDumpWriteDump;

static int __has_nested = 0;
static LPTOP_LEVEL_EXCEPTION_FILTER __prev_filter;
static char __dump_file[MAX_PATH], __log_file[MAX_PATH];
static HANDLE __handle_log, __handle_dmp;
static char *__report_buf = NULL;

/* Try to get OpenThread and Thread32* function from kernel32.dll,
* since it's not available on Win95/98
*/
typedef HANDLE (WINAPI *tOpenThread)(DWORD dwDesiredAccess,
									 BOOL bInheritHandle, DWORD dwThreadId);
typedef BOOL (WINAPI *tThread32First)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef BOOL (WINAPI *tThread32Next)(HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef HANDLE (WINAPI *tCreateToolhelp32Snapshot)(DWORD dwFlags, DWORD th32ProcessID);


extern BOOL DisplaySystemVersion(LPTSTR buffer);

static void suspend_threads(void)
{

	tOpenThread		pOpenThread;
	tThread32First	pThread32First;
	tThread32Next	pThread32Next;
	tCreateToolhelp32Snapshot pCreateToolhelp32Snapshot;
	HMODULE hKernel32Dll;
	HANDLE hSnapshot;
	DWORD ownProcessId, ownThreadId;
	THREADENTRY32 entry;
	BOOL bNext;

	hKernel32Dll = LoadLibrary(_T("kernel32.dll"));
	if (!hKernel32Dll)
		return;

	pOpenThread = (tOpenThread) GetProcAddress(hKernel32Dll, "OpenThread");
	pThread32First = (tThread32First) GetProcAddress(hKernel32Dll, "Thread32First");
	pThread32Next = (tThread32Next) GetProcAddress(hKernel32Dll, "Thread32Next");
	pCreateToolhelp32Snapshot = (tCreateToolhelp32Snapshot)
		GetProcAddress(hKernel32Dll, "CreateToolhelp32Snapshot");

	if (!pOpenThread	||
		!pThread32First	||
		!pThread32Next	||
		!pCreateToolhelp32Snapshot) {
			CloseHandle(hKernel32Dll);
			return;
		}

		hSnapshot = pCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

		/* Get information about own process/thread */
		ownProcessId = GetCurrentProcessId();
		ownThreadId = GetCurrentThreadId();

		/* Enumerate threads */
		entry.dwSize = sizeof(THREADENTRY32);
		bNext = pThread32First(hSnapshot, &entry);

		while (bNext) {
			/* Suspen threads of own process */

			if (entry.th32OwnerProcessID == ownProcessId
				&& entry.th32ThreadID != ownThreadId) {
					HANDLE hThread;

					//hThread = pOpenThread(THREAD_SUSPEND_RESUME, FALSE, entry.th32ThreadID);
					//if (hThread)
					//	SuspendThread(hThread);
				}
				bNext = pThread32Next(hSnapshot, &entry);
		}
		CloseHandle(hKernel32Dll);
}

static void AddToReport(const char* pText)
{
	DWORD bytesWritten = 0;
	WriteFile(__handle_log, pText, (DWORD) strlen(pText), &bytesWritten, 0);
}

static void AddToReportHex(_int64 number, int minDigits /*=0*/)
{
	char buffer[sizeof(_int64) * 2 + 1];

	if (!minDigits)
		sprintf(buffer, "%I64X", number);
	else {
		char fmt[10];

		sprintf(fmt, "%%0%dI64X", minDigits);
		sprintf(buffer, fmt, number);
	}
	AddToReport(buffer);
}

static BOOL GetAddrDetails(PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset)
{
	// Get details about an address: Module name, section and offet

	// Get information about the provided address
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr, &mbi, sizeof(mbi)))
		return FALSE;

	// Get module
	DWORD hMod = (DWORD)mbi.AllocationBase;
	if (!GetModuleFileName((HMODULE)hMod, szModule, len))
		return FALSE;


	// Get DOS header of module
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hMod;

	// Advance to PE header and get the section information
	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(hMod + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHeader);

	// Get module load address
	DWORD lAddr = (DWORD)addr - hMod; 

	// Search for a section which contains the address
	for (unsigned int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++)
	{
		// Calculate section start and end addresses
		DWORD startAddr = pSection->VirtualAddress;
		DWORD endAddr = startAddr;
		if (pSection->SizeOfRawData > pSection->Misc.VirtualSize)
			endAddr += pSection->SizeOfRawData;
		else
			pSection->Misc.VirtualSize;

		// Look if provided address is between startAddr and endAddr
		if (lAddr >= startAddr && lAddr <= endAddr)
		{
			// We've found the section, set section index and offset
			section = i+1;
			offset = lAddr - startAddr;
			return TRUE;
		}
		pSection++;
	}

	// Section not found, very strange
	return FALSE;
}

static void StackWalk(CONTEXT Context)
{
	char  buf[256];
	AddToReport("\r\nCall stack:\r\n-----------\r\n\r\n");
	AddToReport("Address   Frame     Function			SourceFile\r\n");

	DWORD dwMachineType = 0;

	STACKFRAME sf;
	memset(&sf, 0, sizeof(sf));

#ifdef _M_IX86
	/* Initialize the STACKFRAME structure for the first call.  This is only
	* necessary for Intel CPUs, and isn't mentioned in the documentation.
	*/
	sf.AddrPC.Offset	= Context.Eip;
	sf.AddrPC.Mode		= AddrModeFlat;
	sf.AddrStack.Offset	= Context.Esp;
	sf.AddrStack.Mode	= AddrModeFlat;
	sf.AddrFrame.Offset	= Context.Ebp;
	sf.AddrFrame.Mode	= AddrModeFlat;

	dwMachineType = IMAGE_FILE_MACHINE_I386;
#endif

	while (1) {
		/* Get next stack frame */
		if (!pStackWalk(dwMachineType, GetCurrentProcess(), GetCurrentThread(),
			&sf, &Context, 0, 
			pSymFunctionTableAccess, pSymGetModuleBase,	0))
			break;

		if (!sf.AddrFrame.Offset)
			break; /* Invalid frame */

		AddToReportHex(sf.AddrPC.Offset, 8);
		AddToReport("  ");
		AddToReportHex(sf.AddrFrame.Offset, 8);
		AddToReport("  ");

		/* Get function name for stack frame entry */
		BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
		PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
		pSymbol->SizeOfStruct = sizeof(symbolBuffer);
		pSymbol->MaxNameLen = 1024;

		DWORD64 symDisplacement = 0;
		/* Displacement of the input address, 
		* relative to the start of the symbol
		*/

		if (pSymFromAddr(GetCurrentProcess(), sf.AddrPC.Offset,
			&symDisplacement, pSymbol)) {
				AddToReport(pSymbol->Name);
				AddToReport("+");
				AddToReportHex(symDisplacement, 0);
			} else	{ /* No symbol found.  Print out the logical address instead. */
				TCHAR szModule[MAX_PATH] = _T("");
				DWORD section = 0, offset = 0;

				GetAddrDetails((PVOID)sf.AddrPC.Offset,
					szModule, sizeof(szModule), section, offset);

				AddToReportHex(section, 4);
				AddToReport(":");
				AddToReportHex(offset, 8);
				AddToReport(" ");
				AddToReport(szModule);
			}

			/* Get the source line for this stack frame entry */
			IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
			DWORD dwLineDisplacement;

			if (pSymGetLineFromAddr(GetCurrentProcess(), sf.AddrPC.Offset,
				&dwLineDisplacement, &lineInfo)) {
					AddToReport("  ");
					AddToReport(lineInfo.FileName);
					AddToReport(" line ");
					sprintf(buf, "%d", lineInfo.LineNumber);
					AddToReport(buf);
				}

				AddToReport("\r\n");
	}
}

static LPTSTR GetExceptionString(DWORD dwCode)
{
#define EXCEPTION(x) case EXCEPTION_##x: return _T(#x);

	switch (dwCode) {
		EXCEPTION(ACCESS_VIOLATION)
			EXCEPTION(DATATYPE_MISALIGNMENT)
			EXCEPTION(BREAKPOINT)
			EXCEPTION(SINGLE_STEP)
			EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
			EXCEPTION(FLT_DENORMAL_OPERAND)
			EXCEPTION(FLT_DIVIDE_BY_ZERO)
			EXCEPTION(FLT_INEXACT_RESULT)
			EXCEPTION(FLT_INVALID_OPERATION)
			EXCEPTION(FLT_OVERFLOW)
			EXCEPTION(FLT_STACK_CHECK)
			EXCEPTION(FLT_UNDERFLOW)
			EXCEPTION(INT_DIVIDE_BY_ZERO)
			EXCEPTION(INT_OVERFLOW)
			EXCEPTION(PRIV_INSTRUCTION)
			EXCEPTION(IN_PAGE_ERROR)
			EXCEPTION(ILLEGAL_INSTRUCTION)
			EXCEPTION(NONCONTINUABLE_EXCEPTION)
			EXCEPTION(STACK_OVERFLOW)
			EXCEPTION(INVALID_DISPOSITION)
			EXCEPTION(GUARD_PAGE)
			EXCEPTION(INVALID_HANDLE)
	}

	/* Try to get descripbion of unknown exceptions */
	static TCHAR buffer[512] = {0};

	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
		GetModuleHandle(_T("NTDLL.DLL")),
		dwCode, 0, buffer, sizeof(buffer), 0);

	return buffer;
}

static void create_report(PEXCEPTION_POINTERS pExceptionInfo)
{
	char  version[256];

	/* Start out with a banner */
	AddToReport("Exception report created by ");
	AddToReport(GetVersionString(version, sizeof(version)));
	AddToReport("\r\n===================================================\r\n\r\n");
	AddToReport("System details:\r\n");
	AddToReport("---------------\r\n\r\nOperating System:      ");

	char buffer[2048];

	if (DisplaySystemVersion(buffer)) {
		AddToReport(buffer);
		AddToReport("\r\n");
	} else
		AddToReport("Could not get OS version\r\n");

	CProcessorInfo pi;
	CMemoryInfo mi;
	AddToReport("Processor Information: ");
	AddToReport(pi.GetProcessorName(buffer, sizeof(buffer)));
	AddToReport("\r\nMemory Information:    ");
	AddToReport(mi.GetMemoryInfo(buffer, sizeof(buffer)));

	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

	/* Print fault type */
	AddToReport("\r\nException Details:\r\n------------------\r\n\r\nException code: ");
	AddToReportHex(pExceptionRecord->ExceptionCode, 8);
	AddToReport(" ");
	AddToReport(GetExceptionString(pExceptionRecord->ExceptionCode));

	// Add fault address and module
	TCHAR szModule[MAX_PATH];
	memset(szModule, 0, MAX_PATH);
	DWORD dwSection, dwOffset;
	GetAddrDetails(pExceptionRecord->ExceptionAddress,
		szModule, sizeof(szModule), dwSection, dwOffset);

	AddToReport("\r\nFault address:  ");
	AddToReportHex((int)pExceptionRecord->ExceptionAddress, 8);
	AddToReport(" ");
	AddToReportHex(dwSection, 2);
	AddToReport(":");
	AddToReportHex(dwOffset, 8);
	AddToReport(" ");
	AddToReport(szModule);
	AddToReport("\r\n");

	// Set up the symbol engine.
	DWORD dwOptions = pSymGetOptions() ;

	// Turn on line loading and deferred loading.
	pSymSetOptions(dwOptions | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

	PCONTEXT pContext = pExceptionInfo->ContextRecord;

	// Initialize DbgHelp
	if (!pSymInitialize(GetCurrentProcess(), 0, TRUE))
		return;

	StackWalk(*pContext);

	pSymCleanup(GetCurrentProcess());
}

static void create_dump(PEXCEPTION_POINTERS pExceptionInfo)
{
	/* Write the minidump to the file */
	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	MINIDUMP_CALLBACK_INFORMATION cbMiniDump;

	if (__handle_dmp == INVALID_HANDLE_VALUE)
		return;

	eInfo.ThreadId = GetCurrentThreadId();
	eInfo.ExceptionPointers = pExceptionInfo;
	eInfo.ClientPointers = FALSE;

	cbMiniDump.CallbackRoutine = 0;
	cbMiniDump.CallbackParam = 0;

	pMiniDumpWriteDump(GetCurrentProcess(),
		GetCurrentProcessId(),
		__handle_dmp,
		MiniDumpNormal,
		pExceptionInfo ? &eInfo : NULL,
		NULL,
		&cbMiniDump);
}

static void create_files(void)
{
	//del by whg 20090212 __handle_log = CreateFile(__log_file, GENERIC_WRITE,FILE_SHARE_READ,
		//0, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0);
	__handle_dmp = CreateFile(__dump_file, GENERIC_WRITE,FILE_SHARE_READ,
		0, CREATE_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0);
}

static void close_files(void)
{
	//del by whg 20090212if (__handle_log != INVALID_HANDLE_VALUE)
		// CloseHandle(__handle_log);
	if (__handle_dmp != INVALID_HANDLE_VALUE)
		CloseHandle(__handle_dmp);
}


bool GetSelfFileName(char *fileNameBuff, int bufSize)
{
	DWORD len = GetModuleFileName( 0, fileNameBuff, bufSize);
	if (len)
	{
		fileNameBuff[len] = 0;
		return true;
	}

	return false;
}


BOOL GetAppVersion(char *LibName, WORD *MajorVersion, WORD *MinorVersion, WORD *BuildNumber, WORD *RevisionNumber )
{
	DWORD dwHandle, dwLen;
	UINT BufLen;
	LPTSTR lpData;
	VS_FIXEDFILEINFO *pFileInfo;

	dwLen = GetFileVersionInfoSize( LibName, &dwHandle );
	if (!dwLen)
		return FALSE;
	
	lpData = (LPTSTR) malloc (dwLen);
	if (!lpData) 
		return FALSE;
	if( !GetFileVersionInfo( LibName, dwHandle, dwLen, lpData ) )
	{
		free(lpData);
		return FALSE;
	}

	if( VerQueryValue( lpData, "\\", (LPVOID *) &pFileInfo, (PUINT)&BufLen ) ) 
	{
		*MajorVersion = HIWORD(pFileInfo->dwFileVersionMS);
		*MinorVersion = LOWORD(pFileInfo->dwFileVersionMS);
		*BuildNumber = HIWORD(pFileInfo->dwFileVersionLS);
		*RevisionNumber = LOWORD(pFileInfo->dwFileVersionLS);
		free (lpData);
		return TRUE;
	}
	free (lpData);
	
	return FALSE;
}



LONG WINAPI filter_callback(PEXCEPTION_POINTERS pExceptionInfo)
{
	HMODULE hDll;

	/* Suspend all threads to freeze the current state */
	suspend_threads();

	hDll = LoadLibrary(_T("dbghelp.dll"));
	if (!hDll) {
		if (__prev_filter)
			__prev_filter(pExceptionInfo);

		return (EXCEPTION_CONTINUE_SEARCH);
	}


	pSymCleanup				= (tSymCleanup)GetProcAddress(hDll, "SymCleanup");
	pSymInitialize			= (tSymInitialize)GetProcAddress(hDll, "SymInitialize");
	pSymGetOptions			= (tSymGetOptions)GetProcAddress(hDll, "SymGetOptions");
	pSymSetOptions			= (tSymSetOptions)GetProcAddress(hDll, "SymSetOptions");
	pSymGetLineFromAddr		= (tSymGetLineFromAddr)GetProcAddress(hDll, "SymGetLineFromAddr");
	pSymFromAddr			= (tSymFromAddr)GetProcAddress(hDll, "SymFromAddr");
	pStackWalk				= (tStackWalk)GetProcAddress(hDll, "StackWalk");
	pSymFunctionTableAccess	= (tSymFunctionTableAccess)GetProcAddress(hDll, "SymFunctionTableAccess");
	pSymGetModuleBase		= (tSymGetModuleBase)GetProcAddress(hDll, "SymGetModuleBase");
	pMiniDumpWriteDump		= (tMiniDumpWriteDump)GetProcAddress(hDll, "MiniDumpWriteDump");

	if (!pSymCleanup			||
		!pSymInitialize			||
		!pSymGetOptions			||
		!pSymSetOptions			||
		!pSymGetLineFromAddr	||
		!pSymFromAddr			||
		!pStackWalk				||
		!pSymFunctionTableAccess||
		!pSymGetModuleBase		||
		!pMiniDumpWriteDump) {
			FreeLibrary(hDll);
			if (__prev_filter)
				return __prev_filter(pExceptionInfo);
			else
				return EXCEPTION_CONTINUE_SEARCH;
		}

		create_files();
	//del by whg 20090212 for vs 2003	create_report(pExceptionInfo);
		create_dump(pExceptionInfo);

		close_files();
		FreeLibrary(hDll);

		//调用报告程序
		if (logProgram[0])
		{
			char app[1024] = {0};

			WORD MajorVersion, MinorVersion, BuildNumber, RevisionNumber;
			MajorVersion = MinorVersion = BuildNumber = RevisionNumber = 0;
			if (GetSelfFileName(app, _MAX_PATH-1))
				GetAppVersion(app, &MajorVersion, &MinorVersion, &BuildNumber, &RevisionNumber);


			
			TCHAR lpFileName[MAX_PATH];
			//返回应用程序全路径
			//TODO lpFileName 未写文件尾
			::GetModuleFileName(NULL, lpFileName, MAX_PATH);//added by whg 2009.01.14

			_snprintf(app, 1024-1, "-f \"%s\" -u \"%s\" -t \"%s\" -v \"%d.%d.%d.%d\" -a \"%s\" -p %lu",
				__dump_file, url, lpFileName, MajorVersion, MinorVersion, BuildNumber, RevisionNumber, 
				param, GetCurrentProcessId());

			//MessageBoxA(NULL,param,NULL,0);
			//MessageBoxA(NULL,app,NULL,0);
			//此处可以调用报告发送程序
			if (strlen(logProgram))
				ShellExecute(
					NULL,           //句柄
					_T("open"),     //指定动作
					_T(logProgram), //指定打开文件或程序
					app,            //为打开的程序指定参数
					NULL,			//缺省目录
#ifdef _DEBUG						//打开选项
					SW_SHOW			//用最近的大小和位置显示，不激活
#else
					SW_HIDE			//隐藏
#endif
				);
		}


		if (__prev_filter)
			return (__prev_filter(pExceptionInfo));
		else
			return EXCEPTION_CONTINUE_SEARCH;
}


void crash_dump_init()
{

	char *ptr;


	if (__has_nested) {
		return;
	}

	GetModuleFileName(0, __dump_file, sizeof(__dump_file));
	if (__dump_file[0] == 0) {
		printf("(%d): can't get filename", __LINE__);
		return;
	} else if (strlen(__dump_file) >= MAX_PATH) {
		printf("%s(%d): file(%s) too large", __LINE__, __dump_file);
		return;
	}

	ptr = strrchr(__dump_file, '.');
	if (ptr) {
		*ptr = 0;
		_snprintf(__log_file, sizeof(__log_file), "%s.rpt", __dump_file);
		_snprintf(ptr, sizeof(__dump_file) - (ptr - __dump_file), ".dmp");
	} else {
		_snprintf(__log_file, sizeof(__log_file), "%s.rpt", __dump_file);
		_snprintf(__dump_file, sizeof(__dump_file) - strlen(ptr), ".dmp");
	}

	if (__report_buf == NULL)
		__report_buf = (char *)malloc(2048);

	__prev_filter = SetUnhandledExceptionFilter(filter_callback);
}

void Update_CrashCatch(const char *logProgramFile, const char *reportUrl, const char *reportParam) {
	if (logProgramFile)
		strncpy(logProgram, logProgramFile, _MAX_PATH - 1);

	if (reportParam)
		strncpy(param, reportParam, sizeof(param) - 1);

	if (reportUrl)
		strncpy(url, reportUrl, sizeof(url) - 1);
}


void Init_CrashCatch(const char *logProgramFile, const char *reportUrl, const char *reportParam)
{
	if (logProgramFile)
		strncpy(logProgram, logProgramFile, _MAX_PATH-1);

	if (reportParam)
		strncpy(param, reportParam, sizeof(param) - 1);

	if (reportUrl)
		strncpy(url, reportUrl, sizeof(url) - 1);

	crash_dump_init();	
}

void CrashUpArg(const char *reportParam)
{
	strncpy(param, reportParam, sizeof(param)-1);
}