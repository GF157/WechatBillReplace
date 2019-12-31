#pragma once

using namespace std;

/****************************************!
��ʵ�� APP ������
��� APP ��������
****************************************/
class AppIni {

public:
	~AppIni();

	static AppIni* GetInstance();

	bool IsDev();

	/****************************************!
	*@brief   ��ȡ ��ǰӦ�ó��� Ŀ¼
	*@param[out]
	*@return     std::wstring
	****************************************/
	static wstring GetAppPath();

	/****************************************!
	*@brief  ��ȡ��ǰ�������� ������չ��
	*@param[out]
	*@return     std::wstring
	****************************************/
	static wstring GetAppName();

	/****************************************!
	*@brief   ��ȡ ��ǰӦ�ó��� ȫ·��Ŀ¼
	*@param[out]
	*@return     std::wstring
	****************************************/
	static wstring GetAppFullPath();

	/****************************************!
	*@brief  ��ȡ��ǰ���� �汾��
	*@param[out]
	*@return     std::string
	****************************************/
	static string GetAppVersion();

	static wstring GetAppVersionW();

	static string GetResolution();

	static unsigned int GetOSVerNum();

	static string GetOSName();


	/****************************************!
	*@brief  ���ļ� �����û���Ϣ
	*@param[out]
	*@param[in]  wstring file  �ļ�ȫ·��
	*@return     std::wstring   ������Ϣ  �޴���Ϊ���ַ���
	****************************************/
	wstring LoadAppIni(wstring file);

	/****************************************!
	*@brief
	*@param[out]
	*@param[in]  wstring file   �ļ�ȫ·��
	*@return     std::wstring   ������Ϣ  �޴���Ϊ���ַ���
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
	// ��ʵ��ģʽ 
	static AppIni* p;

	static std::mutex singleton_p_mutex;

	AppIni();
	//�Ѹ��ƹ��캯����=������Ҳ��Ϊ˽��,��ֹ������
	AppIni(const AppIni&);
	AppIni& operator=(const AppIni&);

};
