#ifndef _CORE_CRASH_CATCH_DDKSJLD_
#define _CORE_CRASH_CATCH_DDKSJLD_

#ifdef _WIN32
#pragma comment(lib, "version")

//#include "../../RtmpLib/jni/def.h"

#ifdef	__cplusplus
extern "C" {
#endif

	//char *logProgramFile, ����dump�ļ��ĳ�����
	//char *reportParam		����dump����ʱ�ĸ��Ӳ���
	void Init_CrashCatch(const char *pDumpName = NULL, const char *logProgramFile = NULL, const char *reportParam = NULL);

	void CrashUpArg(const char *reportParam);

	LONG WINAPI filter_callback(PEXCEPTION_POINTERS pExceptionInfo);

#ifdef	__cplusplus
}

#endif

#endif

#endif

