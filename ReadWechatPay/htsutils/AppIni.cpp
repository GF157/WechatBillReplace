#include "stdafx.h"
#include "AppIni.h"

#include "Utils.h"
#include "CMyINI.h"

// 默认初始化 单实例对象
AppIni* AppIni::p = nullptr;

std::mutex AppIni::singleton_p_mutex;

AppIni::~AppIni() {}

AppIni * AppIni::GetInstance() {
	if (p == nullptr) {
		singleton_p_mutex.lock();
		if (p == nullptr) {
			p = new AppIni();
		}
		singleton_p_mutex.unlock();
	}
	return p;
}

AppIni & AppIni::operator=(const AppIni &) {
	return *GetInstance();
}


bool AppIni::IsDev() {
	return ENVIRON == "debug";
}


wstring AppIni::GetAppPath() {
	wstring StrPath = GetAppFullPath();
	size_t pos = StrPath.find_last_of(L"\\");
	StrPath = StrPath.substr(0, pos);
	return StrPath;
}

wstring AppIni::GetAppName() {
	wstring StrPath = GetAppFullPath();
	size_t pos = StrPath.find_last_of(L"\\");
	StrPath = StrPath.substr(pos + 1, StrPath.length() - pos);
	//去掉前缀和后缀

	size_t pos2 = StrPath.find_last_of(L".");
	if (pos2 > 0) {
		StrPath = StrPath.substr(0, pos2);
	}
	return StrPath;
}

wstring AppIni::GetAppFullPath() {
	wchar_t pathStr[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, pathStr, MAX_PATH);
	wstring StrPath(pathStr);
	return StrPath;
}

string AppIni::GetAppVersion() {
	wstring StrPath = GetAppFullPath();
	return Util::GetExeFileVersion(StrPath);
}

wstring AppIni::GetAppVersionW() {
	string ver = GetAppVersion();
	return Util::s2ws(ver);
}

string AppIni::GetResolution() {
	int nScreenWidth, nScreenHeight;
	nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
	string res = "x";
	res = Util::int2str(nScreenWidth) + res + Util::int2str(nScreenHeight);
	return res;
}

unsigned int AppIni::GetOSVerNum() {
	unsigned int vname;
	typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
	HINSTANCE hinst = LoadLibraryW(L"ntdll.dll");
	DWORD dwMajor, dwMinor, dwBuildNumber;
	NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
	proc(&dwMajor, &dwMinor, &dwBuildNumber);
	if (dwMajor == 6 && dwMinor == 3)	//win 8.1
	{
		return 20121026;  // "Microsoft Windows 8.1";
	}
	if (dwMajor == 10 && dwMinor == 0)	//win 10
	{
		return 20150729;  // "Microsoft Windows 10";
	}

	//判断win8.1以下的版本
	SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器  
	GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构  
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(disable:4996)
	if (GetVersionEx((OSVERSIONINFO *)&os)) {

		//下面根据版本信息判断操作系统名称  
		switch (os.dwMajorVersion) {                        //判断主版本号  
		case 4:
			switch (os.dwMinorVersion) {                //判断次版本号  
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					vname = 19960729;  // "Microsoft Windows NT 4.0";  //1996年7月发布  
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					vname = 19950824;   //  "Microsoft Windows 95";
				break;
			case 10:
				vname = 19980625;  //   "Microsoft Windows 98";
				break;
			case 90:
				vname = 20000914;  //  "Microsoft Windows Me";
				break;
			}
			break;
		case 5:
			switch (os.dwMinorVersion) {
			case 0:
				vname = 20000217;   // "Microsoft Windows 2000";
				break;
			case 1:
				vname = 20010824;   //  "Microsoft Windows XP";
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION &&
					info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					vname = 20010825;  //  "Microsoft Windows XP Professional x64 Edition";   XP + 1
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					vname = 20030328;  //  "Microsoft Windows Server 2003";
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					vname = 20030329;  //  "Microsoft Windows Server 2003 R2";   2003 + 1
				break;
			}
			break;
		case 6:
			switch (os.dwMinorVersion) {
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = 20070130;  //  "Microsoft Windows Vista";
				else
					vname = 20080313;  //  "Microsoft Windows Server 2008";
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = 20091022;  //  "Microsoft Windows 7";
				else
					vname = 20080314;   //  "Microsoft Windows Server 2008 R2";  2008 + 1
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = 20121026;  //  "Microsoft Windows 8";
				else
					vname = 20120904; //  "Microsoft Windows Server 2012";
				break;
			}
			break;
		default:
			vname = 0;
		}
		return vname;
	}

	return 0;
}

string AppIni::GetOSName() {
	string vname;
	typedef void(__stdcall*NTPROC)(DWORD*, DWORD*, DWORD*);
	HINSTANCE hinst = LoadLibraryW(L"ntdll.dll");
	DWORD dwMajor, dwMinor, dwBuildNumber;
	NTPROC proc = (NTPROC)GetProcAddress(hinst, "RtlGetNtVersionNumbers");
	proc(&dwMajor, &dwMinor, &dwBuildNumber);
	if (dwMajor == 6 && dwMinor == 3)	//win 8.1
	{
		return "Microsoft Windows 8.1";
	}
	if (dwMajor == 10 && dwMinor == 0)	//win 10
	{
		return "Microsoft Windows 10";
	}

	//判断win8.1以下的版本
	SYSTEM_INFO info;                //用SYSTEM_INFO结构判断64位AMD处理器  
	GetSystemInfo(&info);            //调用GetSystemInfo函数填充结构  
	OSVERSIONINFOEX os;
	os.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
#pragma warning(disable:4996)
	if (GetVersionEx((OSVERSIONINFO *)&os)) {

		//下面根据版本信息判断操作系统名称  
		switch (os.dwMajorVersion) {                        //判断主版本号  
		case 4:
			switch (os.dwMinorVersion) {                //判断次版本号  
			case 0:
				if (os.dwPlatformId == VER_PLATFORM_WIN32_NT)
					vname = "Microsoft Windows NT 4.0";  //1996年7月发布  
				else if (os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
					vname = "Microsoft Windows 95";
				break;
			case 10:
				vname = "Microsoft Windows 98";
				break;
			case 90:
				vname = "Microsoft Windows Me";
				break;
			}
			break;
		case 5:
			switch (os.dwMinorVersion) {
			case 0:
				vname = "Microsoft Windows 2000";
				break;
			case 1:
				vname = "Microsoft Windows XP";
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION &&
					info.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
					vname = "Microsoft Windows XP Professional x64 Edition";
				else if (GetSystemMetrics(SM_SERVERR2) == 0)
					vname = "Microsoft Windows Server 2003";
				else if (GetSystemMetrics(SM_SERVERR2) != 0)
					vname = "Microsoft Windows Server 2003 R2";
				break;
			}
			break;
		case 6:
			switch (os.dwMinorVersion) {
			case 0:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = "Microsoft Windows Vista";
				else
					vname = "Microsoft Windows Server 2008";
				break;
			case 1:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = "Microsoft Windows 7";
				else
					vname = "Microsoft Windows Server 2008 R2";
				break;
			case 2:
				if (os.wProductType == VER_NT_WORKSTATION)
					vname = "Microsoft Windows 8";
				else
					vname = "Microsoft Windows Server 2012";
				break;
			}
			break;
		default:
			vname = "UNKNOWN";
		}
		return vname;
	}

	return "UNKNOWN";
}


wstring AppIni::LoadAppIni(wstring file) {
	if (Util::IsDirExist(file)) {
		return L"ini IsDirExist，file：" + file;
	}
	if (!Util::IsFileExist(file)) {
		return L"ini not IsFileExist，file：" + file;
	}
	if (!Util::IsFileReadAble(file)) {
		return L"ini not IsFileReadAble，file：" + file;
	}

	CMyINI *p = new CMyINI();
	string sfile = Util::ws2s(file);
	p->ReadINI(sfile);
	string _URL = p->GetValue("PARAMS", "URL");
	string _COUNTLY_URL = p->GetValue("PARAMS", "COUNTLY_URL");
	string _COUNTLY_KEY = p->GetValue("PARAMS", "COUNTLY_KEY");
	string _ENVIRON = p->GetValue("PARAMS", "ENVIRON");

	string _COUNTLY_PORT_STR = p->GetValue("PARAMS", "COUNTLY_PORT");

	string _FFMPEG_LOG_STR = p->GetValue("PARAMS", "FFMPEG_LOG");

	string _SHOW_ABOUT_STR = p->GetValue("PARAMS", "SHOW_ABOUT");

	string _AUTO_UPDATE_STR = p->GetValue("PARAMS", "AUTO_UPDATE");

	string _AUTO_UPDATE_LOG_STR = p->GetValue("PARAMS", "AUTO_UPDATE_LOG");

	string _EGG_DOUBLE_CLICK_STR = p->GetValue("PARAMS", "EGG_DOUBLE_CLICK");

	string _DUMP_URL = p->GetValue("PARAMS", "DUMP_URL");

	string _UPDATE_URL = p->GetValue("PARAMS", "UPDATE_URL");

	string _UPDATE_TAG = p->GetValue("PARAMS", "UPDATE_TAG");

	if (!_URL.empty()) {
		URL = Util::trim(_URL);
	}
	if (!_COUNTLY_URL.empty()) {
		COUNTLY_URL = Util::trim(_COUNTLY_URL);
	}
	if (!_COUNTLY_KEY.empty()) {
		COUNTLY_KEY = Util::trim(_COUNTLY_KEY);
	}
	if (!_ENVIRON.empty()) {
		char * tmp = (char*)_ENVIRON.c_str();
		_strlwr_s(tmp, strlen(tmp) + 1);
		ENVIRON = Util::trim(_ENVIRON) == "debug" ? "debug" : "prod";
	}

	int _COUNTLY_PORT = Util::str2int(_COUNTLY_PORT_STR);
	if (_COUNTLY_PORT > 0) {
		COUNTLY_PORT = _COUNTLY_PORT;
	}

	int _FFMPEG_LOG = Util::str2int(_FFMPEG_LOG_STR);
	if (_FFMPEG_LOG >= 0) {
		FFMPEG_LOG = _FFMPEG_LOG > 0 ? 1 : 0;
	}

	int _SHOW_ABOUT = Util::str2int(_SHOW_ABOUT_STR);
	if (_SHOW_ABOUT >= 0) {
		SHOW_ABOUT = _SHOW_ABOUT > 0 ? 1 : 0;
	}

	int _AUTO_UPDATE = Util::str2int(_AUTO_UPDATE_STR);
	if (_AUTO_UPDATE >= 0) {
		AUTO_UPDATE = _AUTO_UPDATE > 0 ? 1 : 0;
	}

	int _AUTO_UPDATE_LOG = Util::str2int(_AUTO_UPDATE_LOG_STR);
	if (_AUTO_UPDATE_LOG >= 0) {
		AUTO_UPDATE_LOG = _AUTO_UPDATE_LOG > 0 ? 1 : 0;
	}

	int _EGG_DOUBLE_CLICK = Util::str2int(_EGG_DOUBLE_CLICK_STR);
	if (_EGG_DOUBLE_CLICK >= 0) {
		EGG_DOUBLE_CLICK = _EGG_DOUBLE_CLICK > 0 ? 1 : 0;
	}

	if (!_DUMP_URL.empty()) {
		DUMP_URL = Util::trim(_DUMP_URL);
	}

	if (!_UPDATE_URL.empty()) {
		UPDATE_URL = Util::trim(_UPDATE_URL);
	}

	if (!_UPDATE_TAG.empty()) {
		UPDATE_TAG = Util::trim(_UPDATE_TAG);
	}


	return L"";
}

wstring AppIni::DumpAppIni(wstring file) {
	if (Util::IsDirExist(file)) {
		return L"ini IsDirExist，file：" + file;
	}
	if (Util::IsFileExist(file) && !Util::IsFileWriteAble(file)) {
		return L"ini not IsFileWriteAble，file：" + file;
	}

	CMyINI *p = new CMyINI();
	string sfile = Util::ws2s(file);

	p->SetValue("PARAMS", "URL", URL);
	p->SetValue("PARAMS", "COUNTLY_URL", COUNTLY_URL);
	p->SetValue("PARAMS", "COUNTLY_KEY", COUNTLY_KEY);
	p->SetValue("PARAMS", "ENVIRON", ENVIRON);

	string _COUNTLY_PORT_STR = Util::int2str(COUNTLY_PORT);
	p->SetValue("PARAMS", "COUNTLY_PORT", _COUNTLY_PORT_STR);

	p->WriteINI(sfile);
	return L"";
}

map<string, string> AppIni::VauleAsMap() {
	map<string, string> m;
	m["URL"] = URL;
	m["COUNTLY_URL"] = COUNTLY_URL;
	m["COUNTLY_KEY"] = COUNTLY_URL;
	m["ENVIRON"] = ENVIRON;
	m["COUNTLY_PORT"] = Util::int2str(COUNTLY_PORT);
	return m;
}

AppIni::AppIni() {}

