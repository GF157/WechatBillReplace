#pragma once

#include <cpprest\http_client.h>
using namespace std;
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency;

//delete 宏助手

#ifndef SAFE_RELEASE

#define SAFE_RELEASE(ptr)\
do{\
    if (ptr){(ptr)->Release(); (ptr)=NULL;}\
}while (0)

#endif // !SAFE_RELEASE


#ifndef SAFE_DELETE

#define SAFE_DELETE(ptr)\
do{\
    if (ptr){delete (ptr); (ptr) = NULL;}\
}while (0)

#endif // !SAFE_DELETE


#ifndef SAFE_DELETE_ARRAY

#define SAFE_DELETE_ARRAY(ptr)\
do{\
    if (ptr){delete[] (ptr); (ptr) = NULL;}\
}while (0)

#endif // !SAFE_DELETE_ARRAY


#ifndef SAFE_RELEASE_PTR

#define SAFE_RELEASE_PTR(ptr)\
do{\
    if (ptr){delete (ptr); (ptr) = NULL;}\
}while (0)

#endif // !SAFE_RELEASE_PTR


/****************************************!
纯静态类
封装了常用方法
除应用程序环境变量外  不依赖其他状态
****************************************/
class Util {
public:
	~Util() {};

	static unsigned long long GetTimestamp();

	static int gcd(int a, int b);

	static pair<int, int> parseXY(wstring str, wstring seq, pair<int, int> wh);

	static unsigned long long GetCurrentTimeMsec();

	static wstring Util::fmtNow(wstring fmt = L"%Y-%m-%d %H:%M:%S");

	static string dw2s(DWORD err);

	static wstring dw2ws(DWORD err);

	static string tolower(string s);

	static string toupper(string s);

	static wstring tolower(wstring s);

	static wstring toupper(wstring s);

	static bool str_cmp(const string & s1, const string & s2);

	static bool str_cmp(const wstring & s1, const wstring & s2);

	static bool stri_cmp(const string & s1, const string & s2);

	static bool stri_cmp(const wstring & s1, const wstring & s2);

	static bool str_startwith(const string & s1, const string & s2);

	static bool str_startwith(const wstring & s1, const wstring & s2);

	static bool stri_startwith(const string & str, const string & need);

	static bool stri_startwith(const wstring & str, const wstring & need);

	static bool str_endwith(const string & s1, const string & s2);

	static bool str_endwith(const wstring & s1, const wstring & s2);

	static bool stri_endwith(const string & str, const string & need);

	static bool stri_endwith(const wstring & str, const wstring & need);

	/****************************************!
	*@brief  从文件中读取 wstring
	*@param[out]
	*@param[in]  wstring file  文件名称
	*@return     std::wstring  文件内容字符串  读取失败 返回空字符串
	****************************************/
	static wstring WStringReadFile(wstring file);

	/****************************************!
	*@brief  向文件中写入 wstring
	*@param[out]
	*@param[in]  wstring file  文件名称
	*@param[in]  wstring str  要写入的内容
	*@return     std::wstring  错误信息  无错误为空字符串
	****************************************/
	static wstring WStringWriteFile(wstring file, wstring str);

	/****************************************!
	*@brief  string  转换  wstring
	*@param[out]
	*@param[in]  const string & s
	*@return     std::wstring
	****************************************/
	static wstring s2ws(string s);

	/****************************************!
	*@brief  wstring  转换  string
	*@param[out]
	*@param[in]  const wstring & ws
	*@return     std::string
	****************************************/
	static string ws2s(wstring ws);

	//************************************
	// Method:    wc2c
	// FullName:  Util::wc2c
	// Access:    public 
	// Returns:   std::string
	// Qualifier: WCHAR 转 char
	// Parameter: const wchar_t * wp
	//************************************
	string Util::wc2c(const wchar_t* wp);

	static string md5(const string& str);

	static string md5(const wstring &ws);

	/****************************************!
	*@brief  去除 string 前后空白
	*@param[out]
	*@param[in]  const string & str
	*@return     std::string
	****************************************/
	static string trim(const string& str);

	static wstring wtrim(const wstring& str);

	static unsigned char ToHex(unsigned char x);

	static string int2str(int int_temp);

	static string Util::long2str(unsigned long long int_temp);

	static string dou2str(double double_temp, int num);

	static wstring int2wstr(int int_temp);

	static wstring long2wstr(unsigned long long int_temp);

	static int str2int(const string &string_temp);

	static int ws2int(const wstring & string_temp);

	static json::value curlRpc(wstring query_url, wstring auth_str, wstring type, wstring content);

	static pplx::task<http_response> Util::curlRpcAsync(wstring query_url, wstring auth_str, wstring type, wstring content);

	static unsigned char FromHex(unsigned char x);

	static wstring Util::StringEscape(const wstring& str);

	static string UrlEncode(const string & str);

	static string UrlDecode(const string & str);

	static string BuildGet(string url, map<string, string> & args);

	static string GetExeFileVersion(wstring lpszFileName);

	/****************************************!
	*@brief  判断 文件夹是否存在
	*@param[out]
	*@param[in]  const std::string & csDir
	*@return     bool
	****************************************/
	static bool IsDirExist(const wstring & csDir);

	static bool IsFileExist(const wstring & csDir);

	static bool IsFileReadAble(const wstring & csDir);

	static bool IsFileWriteAble(const wstring & csDir);

	/****************************************!
	*  StringUtil
	****************************************/

	static std::string& str_replace(std::string&s1, const std::string&s2, const std::string&s3) {
		std::string::size_type pos = 0;
		std::string::size_type a = s2.size();
		std::string::size_type b = s3.size();
		while ((pos = s1.find(s2, pos)) != std::string::npos) {
			s1.replace(pos, a, s3);
			pos += b;
		}
		return s1;
	}

	static std::wstring& wstr_replace(std::wstring&s1, const std::wstring&s2, const std::wstring&s3) {
		std::wstring::size_type pos = 0;
		std::wstring::size_type a = s2.size();
		std::wstring::size_type b = s3.size();
		while ((pos = s1.find(s2, pos)) != std::wstring::npos) {
			s1.replace(pos, a, s3);
			pos += b;
		}
		return s1;
	}

	static std::string trimLeft(const std::string& str, const std::string& token = " ") {
		std::string t = str;
		t.erase(0, t.find_first_not_of(token));
		return t;
	}

	static std::string trimRight(const std::string& str, const std::string& token = " ") {
		std::string t = str;
		t.erase(t.find_last_not_of(token) + 1);
		return t;
	}

	static std::string trimBoth(const std::string& str, const std::string& token = " ") {
		std::string t = str;
		t.erase(0, t.find_first_not_of(token));
		t.erase(t.find_last_not_of(token) + 1);
		return t;
	}

	static std::string toLower(const std::string& str) {
		std::string t = str;
		std::transform(t.begin(), t.end(), t.begin(), ::tolower);
		return t;
	}

	static std::string toUpper(const std::string& str) {
		std::string t = str;
		std::transform(t.begin(), t.end(), t.begin(), ::toupper);
		return t;
	}

	static bool	startsWith(const std::string& str, const std::string& substr) {
		return str.find(substr) == 0;
	}

	static bool endsWith(const std::string& str, const std::string& substr) {
		return str.rfind(substr) == (str.length() - substr.length());
	}

	static bool equalsIgnoreCase(const std::string& str1, const std::string& str2) {
		return toLower(str1) == toLower(str2);
	}

	static std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
		char* save = nullptr;
#ifdef _WIN32
		char* token = strtok_s(const_cast<char*>(str.c_str()), delimiter.c_str(), &save);
#else
		char* token = strtok_r(const_cast<char*>(str.c_str()), delimiter.c_str(), &save);
#endif
		std::vector<std::string> result;
		while (token != nullptr) {
			result.emplace_back(token);
#ifdef _WIN32
			token = strtok_s(nullptr, delimiter.c_str(), &save);
#else
			token = strtok_r(nullptr, delimiter.c_str(), &save);
#endif
		}
		return result;
	}

	static bool contains(const std::string& str, const std::string& token) {
		return str.find(token) == std::string::npos ? false : true;
	}

private:
	Util() {};

public:
	static wstring Util::CFormatWString(const wchar_t *fmt, ...);
};

