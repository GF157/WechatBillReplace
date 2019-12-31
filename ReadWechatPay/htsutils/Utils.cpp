#include "stdafx.h"
#include "Utils.h"
#include "Md5Encode.h"

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _WIN32
#include <winsock2.h>
#include <time.h>
#else
#include <sys/time.h>
#endif

using namespace std;
using namespace boost;

unsigned long long Util::GetTimestamp() {
	FILETIME lp;
	GetSystemTimeAsFileTime(&lp);
	unsigned long long res;
	res = lp.dwHighDateTime;
	res <<= 32;
	res |= lp.dwLowDateTime;
	res /= 10;
	res -= 11644473600000000ULL;
	return res / 1000;
}

wstring Util::fmtNow(wstring fmt) {
	struct tm tmVal;
	time_t timep;
	time(&timep);
	char tmp[64];
	localtime_s(&tmVal, &timep);
	strftime(tmp, sizeof(tmp), ws2s(fmt).c_str(), &tmVal);
	return s2ws(tmp);
}


unsigned long long Util::GetCurrentTimeMsec() {
#ifdef _WIN32
	struct timeval tv;
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;

	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tv.tv_sec = clock;
	tv.tv_usec = wtm.wMilliseconds * 1000;
	return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ((unsigned long long)tv.tv_sec * 1000 + (unsigned long long)tv.tv_usec / 1000);
#endif
}

int Util::gcd(int a, int b) {
	int c;
	c = (a > b) ? b : a;
	while (a%c != 0 || b%c != 0) {
		c--;
	}
	return c;
}

pair<int, int> Util::parseXY(wstring str, wstring seq, pair<int, int> wh) {
	int pos = str.find_first_of(seq);
	pos = str.find_first_of(L"(");
	if (pos > 0) {
		str = str.substr(0, pos);
	}

	pos = str.find_first_of(seq);
	if (pos > 0) {
		return pair<int, int>(ws2int(str.substr(0, pos)), ws2int(str.substr(pos + 1)));
	}

	return wh;
}

string Util::dw2s(DWORD err) {
	char _char[16];
	_ltoa_s(err, _char, 16);
	return "0x" + string(_char);
}

wstring Util::dw2ws(DWORD err) {
	return s2ws(dw2s(err));
}

string Util::tolower(string s) {
	string b(s);
	transform(b.begin(), b.end(), b.begin(), ::tolower);
	return b;
}

string Util::toupper(string s) {
	string b(s);
	transform(b.begin(), b.end(), b.begin(), ::toupper);
	return b;
}

wstring Util::tolower(wstring s) {
	wstring b(s);
	transform(b.begin(), b.end(), b.begin(), ::towlower);
	return b;
}

wstring Util::toupper(wstring s) {
	wstring b(s);
	transform(b.begin(), b.end(), b.begin(), ::towupper);
	return b;
}

bool Util::str_cmp(const string & s1, const string & s2) {
	return s1 == s2;
}

bool Util::str_cmp(const wstring & s1, const wstring & s2) {
	return s1 == s2;
}

bool Util::stri_cmp(const string & s1, const string & s2) {
	return tolower(s1) == tolower(s2);
}

bool Util::stri_cmp(const wstring &  s1, const wstring & s2) {
	return tolower(s1) == tolower(s2);
}

bool Util::str_startwith(const string & str, const string & need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto pre = str.substr(0, len);
	return str_cmp(pre, need);
}

bool Util::str_startwith(const wstring &  str, const wstring &  need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto pre = str.substr(0, len);
	return str_cmp(pre, need);
}

bool Util::stri_startwith(const string & str, const string & need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto pre = str.substr(0, len);
	return stri_cmp(pre, need);
}

bool Util::stri_startwith(const wstring & str, const wstring & need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto pre = str.substr(0, len);
	return stri_cmp(pre, need);
}

bool Util::str_endwith(const string & str, const string & need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto last = str.substr(str.length() - len, len);
	return str_cmp(last, need);
}

bool Util::str_endwith(const wstring &  str, const wstring &  need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto last = str.substr(str.length() - len, len);
	return str_cmp(last, need);
}

bool Util::stri_endwith(const string & str, const string & need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto last = str.substr(str.length() - len, len);
	return stri_cmp(last, need);
}

bool Util::stri_endwith(const wstring &  str, const wstring &  need) {
	size_t len = need.length();
	if (len <= 0) {
		return true;
	}
	if (str.length() < len) {
		return false;
	}
	auto last = str.substr(str.length() - len, len);
	return stri_cmp(last, need);
}

wstring Util::WStringReadFile(wstring file) {
	locale china("chs");
	wstring ret = L"";
	wstring line = L"";

	wifstream rf(file);
	if (rf) {
		rf.imbue(china);
		while (getline(rf, line)) {
			ret += line;
		}
		rf.close();
	}
	return ret;
}

wstring Util::WStringWriteFile(wstring file, wstring str) {
	locale china("chs");
	wofstream wf(file, wofstream::out);
	if (wf) {
		wf.imbue(china);
		wf << str;
		wf.close();
	}
	return L"";
}

wstring Util::s2ws(string s) {
	size_t convertedChars = 0;
	string curLocale = setlocale(LC_ALL, NULL);   //curLocale="C"
	setlocale(LC_ALL, "chs");
	const char* source = s.c_str();
	size_t charNum = sizeof(char)*s.size() + 1;
	wchar_t* dest = new wchar_t[charNum];
	mbstowcs_s(&convertedChars, dest, charNum, source, _TRUNCATE);
	wstring result = dest;
	delete[] dest;
	setlocale(LC_ALL, curLocale.c_str());
	return result;
}

string Util::ws2s(wstring ws) {
	string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";

	setlocale(LC_ALL, "chs");

	const wchar_t* _Source = ws.c_str();
	size_t _Dsize = 2 * ws.size() + 1;
	char *_Dest = new char[_Dsize];
	memset(_Dest, 0, _Dsize);
	size_t   i;
	wcstombs_s(&i, _Dest, _Dsize, _Source, _TRUNCATE);
	string result = _Dest;
	delete[]_Dest;

	setlocale(LC_ALL, curLocale.c_str());

	return result;
}

string Util::wc2c(const wchar_t* wp)
{
	std::string str;
	int len = WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), NULL, 0, NULL, NULL);
	char	*m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wp, wcslen(wp), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	str = m_char;
	delete m_char;
	return str;
}

string Util::md5(const string & str) {
	CMd5Encode* ctx = new CMd5Encode();
	return ctx->getMD5(str);
}

string Util::md5(const wstring & ws) {
	CMd5Encode* ctx = new CMd5Encode();
	return ctx->getMD5(ws2s(ws));
}

string Util::trim(const string & str) {
	string outstr = "";
	for (size_t i = 0; i < str.length(); i++) {
		if (str[i] != ' ' && str[i] != '\r' && str[i] != '\n' && str[i] != '\t')
			outstr.push_back(str[i]);
	}

	return outstr;
}


// trim from both ends
wstring Util::wtrim(const wstring & ws) {
	return s2ws(trim(ws2s(ws)));
}


string Util::int2str(int int_temp) {
	stringstream stream;
	stream << int_temp;
	return stream.str();
}

string Util::long2str(unsigned long long int_temp) {
	stringstream stream;
	stream << int_temp;
	return stream.str();
}

string Util::dou2str(double double_temp, int num) {
	return to_string(double_temp).substr(0, to_string(double_temp).size() - (6-num));
}

wstring Util::int2wstr(int int_temp) {
	return s2ws(int2str(int_temp));
}

wstring Util::long2wstr(unsigned long long int_temp) {
	return s2ws(long2str(int_temp));
}

int Util::str2int(const string & string_temp) {
	int int_temp = 0;
	stringstream stream(string_temp);
	stream >> int_temp;
	return int_temp;
}

int Util::ws2int(const wstring & string_temp) {
	int int_temp = 0;
	stringstream stream(Util::ws2s(string_temp));
	stream >> int_temp;
	return int_temp;
}

pplx::task<http_response> Util::curlRpcAsync(wstring query_url, wstring auth_str, wstring type, wstring content) {
	query_url = Util::stri_startwith(query_url, L"http://") || Util::stri_startwith(query_url, L"https://") ? query_url : L"http://" + query_url;

	size_t pos = Util::stri_startwith(query_url, L"https://") ? 8 : 7;
	wstring tmp = query_url.substr(pos);
	size_t pos2 = tmp.find_first_of(L"/");
	wstring api_host = pos2 > 0 ? query_url.substr(0, pos + pos2) : query_url;
	wstring query_path = pos2 > 0 ? query_url.substr(pos + pos2) : L"/";

	auto conf = http_client_config();
	http_client client(api_host, conf);
	uri_builder builder(query_path);

	auto auth_header =
		[&auth_str](http_request request, std::shared_ptr<http_pipeline_stage> next_stage) -> pplx::task<http_response> {
		if (!auth_str.empty()) {
			request.headers().add(L"Authorization", auth_str);
		}
		return next_stage->propagate(request);
	};

	client.add_handler(auth_header);
	pplx::task<http_response> task2 = client.request(stri_cmp(type, L"post") ? methods::POST : methods::GET, builder.to_string(), content, L"application/json");
	return task2;
}

json::value Util::curlRpc(wstring query_url, wstring auth_str, wstring type, wstring content) {
	query_url = Util::stri_startwith(query_url, L"http://") || Util::stri_startwith(query_url, L"https://") ? query_url : L"http://" + query_url;

	size_t pos = Util::stri_startwith(query_url, L"https://") ? 8 : 7;
	wstring tmp = query_url.substr(pos);
	size_t pos2 = tmp.find_first_of(L"/");
	wstring api_host = pos2 > 0 ? query_url.substr(0, pos + pos2) : query_url;
	wstring query_path = pos2 > 0 ? query_url.substr(pos + pos2) : L"/";

	auto conf = http_client_config();
	http_client client(api_host, conf);
	uri_builder builder(query_path);

	auto auth_header =
		[&auth_str](http_request request, std::shared_ptr<http_pipeline_stage> next_stage) -> pplx::task<http_response> {
		if (!auth_str.empty()) {
			request.headers().add(L"Authorization", auth_str);
		}
		return next_stage->propagate(request);
	};

	client.add_handler(auth_header);
	http_response response = client.request(stri_cmp(type, L"post") ? methods::POST : methods::GET, builder.to_string(), content, L"application/json").get();
	auto response_code = response.status_code();
	wstring response_error = L"";
	if (response_code == status_codes::OK) {
		try {
			const json::value& json_value = response.extract_json().get();
			return json_value;
		} catch (const std::exception& e) {
			response_error = s2ws(e.what());
		}
	}

	json::value error_info;
	error_info[L"response_code"] = json::value::number(response_code);
	error_info[L"response_error"] = json::value::string(response_error);

	json::value error_value;
	error_value[L"Flag"] = json::value::number(500);
	error_value[L"FlagString"] = json::value::string(L"http错误");
	error_value[L"Info"] = json::value::value(error_info);
	return error_value;
}

unsigned char Util::ToHex(unsigned char x) {
	return  x > 9 ? x + 55 : x + 48;
}

unsigned char Util::FromHex(unsigned char x) {
	unsigned char y;
	if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
	else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
	else if (x >= '0' && x <= '9') y = x - '0';
	else assert(0);
	return y;
}


wstring Util::StringEscape(const wstring& str) {
	wstring strTemp = L"";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++) {
		char_t c = str[i];
		if (c == '"') {
			strTemp += '\\';
			strTemp += c;
		}
		else {
			strTemp += c;
		}
	}
	return strTemp;
}

string Util::UrlEncode(const string& str) {
	string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++) {
		if (isalnum((unsigned char)str[i]) ||
			(str[i] == '-') ||
			(str[i] == '_') ||
			(str[i] == '.') ||
			(str[i] == '~'))
			strTemp += str[i];
		else if (str[i] == ' ')
			strTemp += "+";
		else {
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}
	return strTemp;
}

string Util::UrlDecode(const string& str) {
	string strTemp = "";
	size_t length = str.length();
	for (size_t i = 0; i < length; i++) {
		if (str[i] == '+') strTemp += ' ';
		else if (str[i] == '%') {
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		} else strTemp += str[i];
	}
	return strTemp;
}


string Util::BuildGet(string url, map<string, string> & args) {
	if (args.empty()) {
		return url;
	}
	url = Util::trim(url);

	int i = url.find_last_of("?");

	url += i > 0 ? "" : "?";

	url = url.substr(url.length() - 1, 1) == "?" || url.substr(url.length() - 1, 1) == "&" ? url : url + "&";
	vector<string> args_list = {};

	for (auto it = args.begin(); it != args.end(); ++it) {
		args_list.push_back(it->first + "=" + UrlEncode(it->second));
	}

	return !args_list.empty() ? url + boost::algorithm::join(args_list, "&") : url;
}

string Util::GetExeFileVersion(wstring lpszFileName) {
	string  strVersion;

	wchar_t  szSrcfn[MAX_PATH]; // Ҫ��ȡ��Ϣ���ļ�
	lstrcpy(szSrcfn, lpszFileName.c_str());

	// ���ļ���Ϣ
	DWORD  dwVerHnd = 0;
	DWORD  dwVerInfoSize = ::GetFileVersionInfoSizeW(szSrcfn, &dwVerHnd);
	if (dwVerInfoSize) {
		//  If  we  were  able  to  get  the  information,  process  it:
		HANDLE    hMem;
		LPVOID    lpvMem;
		unsigned  int  uInfoSize = 0;
		VS_FIXEDFILEINFO  *  pFileInfo;

		hMem = ::GlobalAlloc(GMEM_MOVEABLE, dwVerInfoSize);
		lpvMem = ::GlobalLock(hMem);
		::GetFileVersionInfo(szSrcfn, dwVerHnd, dwVerInfoSize, lpvMem);
		::VerQueryValue(lpvMem, (LPTSTR)_T("\\"), (void**)&pFileInfo, &uInfoSize);

		WORD  nVer[4];
		nVer[0] = HIWORD(pFileInfo->dwProductVersionMS);
		nVer[1] = LOWORD(pFileInfo->dwProductVersionMS);
		nVer[2] = HIWORD(pFileInfo->dwProductVersionLS);
		nVer[3] = LOWORD(pFileInfo->dwProductVersionLS);
		char VersionBuf[512] = { 0 };
		sprintf_s(VersionBuf, ("%d.%d.%d.%d"), nVer[0], nVer[1], nVer[2], nVer[3]);

		::GlobalUnlock(hMem);
		::GlobalFree(hMem);
		strVersion = VersionBuf;

	}
	return strVersion;
}

bool Util::IsDirExist(const wstring & csDir) {
	DWORD dwAttrib = GetFileAttributesW(csDir.c_str());
	return INVALID_FILE_ATTRIBUTES != dwAttrib && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool Util::IsFileExist(const wstring & csDir) {
	return !IsDirExist(csDir) && !(_waccess(csDir.c_str(), 0) == -1);
}

bool Util::IsFileReadAble(const wstring & csDir) {
	return !IsDirExist(csDir) && !(_waccess(csDir.c_str(), 2) == -1);
}

bool Util::IsFileWriteAble(const wstring & csDir) {
	return !IsDirExist(csDir) && !(_waccess(csDir.c_str(), 4) == -1);
}

wstring Util::CFormatWString(const wchar_t *fmt, ...) {
	std::wstring strResult = L"";
	if (NULL != fmt) {
		va_list marker = NULL;
		va_start(marker, fmt);                            //��ʼ����������
		size_t nLength = _vscwprintf(fmt, marker) + 1;    //��ȡ��ʽ���ַ�������
		std::vector<wchar_t> vBuffer(nLength, L'\0');    //�������ڴ洢��ʽ���ַ������ַ�����
		int nWritten = _vsnwprintf_s(&vBuffer[0], vBuffer.size(), nLength, fmt, marker);
		if (nWritten > 0) {
			strResult = &vBuffer[0];
		}
		va_end(marker);                                    //���ñ�������
	}
	return strResult;
}


