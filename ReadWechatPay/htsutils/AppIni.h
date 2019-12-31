#pragma once

using namespace std;

/****************************************!
单实例 APP 配置类
存放 APP 基础配置
****************************************/
class AppIni {

public:
	~AppIni();

	static AppIni* GetInstance();

	bool IsDev();

	/****************************************!
	*@brief   获取 当前应用程序 目录
	*@param[out]
	*@return     std::wstring
	****************************************/
	static wstring GetAppPath();

	/****************************************!
	*@brief  获取当前程序名称 不带扩展名
	*@param[out]
	*@return     std::wstring
	****************************************/
	static wstring GetAppName();

	/****************************************!
	*@brief   获取 当前应用程序 全路径目录
	*@param[out]
	*@return     std::wstring
	****************************************/
	static wstring GetAppFullPath();

	/****************************************!
	*@brief  获取当前程序 版本号
	*@param[out]
	*@return     std::string
	****************************************/
	static string GetAppVersion();

	static wstring GetAppVersionW();

	static string GetResolution();

	static unsigned int GetOSVerNum();

	static string GetOSName();


	/****************************************!
	*@brief  从文件 加载用户信息
	*@param[out]
	*@param[in]  wstring file  文件全路径
	*@return     std::wstring   错误信息  无错误为空字符串
	****************************************/
	wstring LoadAppIni(wstring file);

	/****************************************!
	*@brief
	*@param[out]
	*@param[in]  wstring file   文件全路径
	*@return     std::wstring   错误信息  无错误为空字符串
	****************************************/
	wstring DumpAppIni(wstring file);

	map<string, string> VauleAsMap();

	string URL = "http://api.xdysoft.com/v1/live/publish?v=2";

	string COUNTLY_URL = "http://countly-node.xdysoft.com";

	unsigned int COUNTLY_PORT = 80;

	string COUNTLY_KEY = "85f03cc9eeb4d8eae9b36cd44adde60e693540c4";

	string ENVIRON = "prod";

	unsigned int FFMPEG_LOG = 0;

	unsigned int SHOW_ABOUT = 1;

	unsigned int AUTO_UPDATE = 1;

	unsigned int AUTO_UPDATE_LOG = 1;

	unsigned int EGG_DOUBLE_CLICK = 1;

	string DUMP_URL = "http://test-blue.wenshunsoft.com/api/LiveApi/dump";

	string UPDATE_URL = "http://test-steel.wenshunsoft.com/api/ApiHub/checkToolVer";

	string UPDATE_TAG = "base";

private:
	// 单实例模式 
	static AppIni* p;

	static std::mutex singleton_p_mutex;

	AppIni();
	//把复制构造函数和=操作符也设为私有,防止被复制
	AppIni(const AppIni&);
	AppIni& operator=(const AppIni&);

};
