#ifndef _CORE_CRASH_CATCH_DDKSJLD_
#define _CORE_CRASH_CATCH_DDKSJLD_

#ifdef _WIN32
#pragma comment(lib, "version")


#ifdef	__cplusplus
extern "C" {
#endif

	//char *logProgramFile, 处理dump文件的程序名
	//char *reportParam		调用dump程序时的附加参数
	void Init_CrashCatch(const char *logProgramFile = NULL, const char *reportUrl = NULL, const char *reportParam = NULL);

	void Update_CrashCatch(const char *logProgramFile, const char *reportUrl, const char *reportParam);

	void CrashUpArg(const char *reportParam);

	LONG WINAPI filter_callback(PEXCEPTION_POINTERS pExceptionInfo);

#ifdef	__cplusplus
}

#endif

#endif

#endif

