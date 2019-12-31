#pragma once
#include <cpprest\http_client.h>

using namespace std;
using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency;

class UserInfo;

/****************************************!
��ʵ�� �û���Ϣ��
����û��ʺ�����
API ������
****************************************/
class UserInfo {

public:
	~UserInfo();

	static UserInfo* GetInstance();

	map<string, string> VauleAsMap();

	/****************************************!
	*@brief  ���ļ� �����û���Ϣ
	*@param[out]
	*@param[in]  wstring file  �ļ�ȫ·��
	*@return     std::wstring   ������Ϣ  �޴���Ϊ���ַ���
	****************************************/
	wstring LoadUserInfo(wstring file);

	/****************************************!
	*@brief  ���ַ��� �����û���Ϣ
	*@param[out]
	*@param[in]  wstring str   JSON �ַ���
	*@return     std::wstring   ������Ϣ  �޴���Ϊ���ַ���
	****************************************/
	wstring LoadByStr(wstring str);

	wstring LoadByJSON(json::value obj);

	std::pair<int, int> GetMaxOutResolution(int dw = 0, int dh = 0);
	/****************************************!
	*@brief  ��API�ӿ� �����û���Ϣ  ͬ��ģʽ
	*@param[out]
	*@param[in]  wstring url  API url �� http:// ���� https:// ��ͷ
	*@param[in]  wstring user  �û��� ֧������
	*@param[in]  wstring pwd  �û���������
	*@return     std::wstring  ������Ϣ  �޴���Ϊ���ַ���
	****************************************/
	wstring LoadByApi(string url, wstring user, wstring pwd);

	void UserInfo::LoadByApiWithCallback(string url, wstring user, wstring pwd, std::function<void(int, wstring, UserInfo*)> callback);

	/****************************************!
	*@brief  ���л�����  ���� JSON �ַ���
	*@param[out]
	*@return     std::wstring  JSON �ַ���
	****************************************/
	wstring DumpToStr();

	//************************************
	// Method:    RefreshStr
	// FullName:  UserInfo::RefreshStr
	// Access:    public 
	// Returns:   std::wstring
	// Qualifier: �ж�ˢ������ǰ���Ƿ���ͬ
	//************************************
	wstring RefreshStr();

	/****************************************!
	*@brief
	*@param[out]
	*@param[in]  wstring file   �ļ�ȫ·��
	*@return     std::wstring   ������Ϣ  �޴���Ϊ���ַ���
	****************************************/
	wstring DumpUserInfo(wstring file);

	wstring user = L"";
	wstring pwd = L"";

	wstring dsc = L"��������";
	wstring play_rtmp_url = L"rtmp://test-live.htssoft.com/live/demo";
	wstring play_hls_url = L"http://test-live.htssoft.com/live/demo.m3u8";

	wstring PubUrlPub = L"push";
	wstring PublishRoomInfo = L"htslive";
	wstring stream = L"demo?vhost=test-live.htssoft.com";
	wstring PubWay = L"rtmp";
	wstring StrKey = L"";
	wstring Domain = L".htssoft.com";
	wstring strUserID = L"0";
	wstring Uin = L"402";
	wstring ChatUrl = L"";

	wstring MaxOutResolution = L"1280*720";
	unsigned int ADSampleRate = 44100;
	unsigned int ADChannels = 2;
	unsigned int ADBitperSample = 16;
	unsigned int ADBitRate = 32;

	unsigned int VDIntervalSecond = 1;
	unsigned int BFrameCount = 0;

	wstring VD_method = L"cbr";
	unsigned int VDFPS = 7;
	unsigned int abrDqmin = 1;
	unsigned int VDBitRate = 300;
	unsigned int VBRQuality = 30;

	wstring VC_method = L"cbr";
	unsigned int VCFPS = 25;
	unsigned int abrCqmin = 30;
	unsigned int VCBitRate = 300;
	unsigned int CQuality = 30;

	wstring VF_method = L"cbr";
	unsigned int VFFPS = 15;
	unsigned int abrFqmin = 30;
	unsigned int VFBitRate = 300;
	unsigned int VBRFQuality = 30;

	unsigned int Flag = 500;

	void SetOutResoluton(wstring outResoluton);
	wstring GetOutResolution();

	void SetMic(wstring audioDevice);
	wstring GetMic();

	void SetMicIndex(int audioIndex);
	unsigned int GetMicIndex();

	void EnableAudioMix(bool audioMix);
	void EnableAntiNoise(bool antiNoise);
	void EnableCapWndLocked(bool bLockCapWnd);
	bool IsAudioMix();
	bool IsAntiNoise();
	bool IsCapWndLocked();

	void SetVolume(int iVolume);
	unsigned int GetVolume();

	void SetLockMode(int iLockMode);
	unsigned int GetLockMode();

	void SetRecordFilePath(wstring strRecordFilePath);
	wstring GetRecordFilePath();

	void SetCamIndex(int camIndex);
	unsigned int GetCamIndex();

	void SetCam(wstring czDevice);
	wstring GetCam();

	void SetResolutionIndex(int iResolutionIndex);
	unsigned int GetResolutionIndex();

	void SetAntiNoiseValue(int nAntiNoiseVal);
	unsigned int GetAntiNoiseValue();

	std::vector<wstring>&  GetReslList();

	std::vector<wstring>&  GetCapResolutionList();

	void ResetPushConfig();
	void SetabrDqmin(int _abrDqmin);

	void SetVDWidth(int vDWidth);
	unsigned int GetVDWidth();
	void SetVDHeight(int vDHeight);
	unsigned int GetVDHeight();
private:
	// ��ʵ��ģʽ 
	static UserInfo* p;

	unsigned int _vDWidth = 640;
	unsigned int _vDHeight = 480;

	wstring _outResoluton = L"640x480";

	wstring _audioDevice = L"";
	unsigned int _audioIndex = 0;

	bool _bLockCapWnd = false;

	bool _audioMix = false;
	bool _antiNoise = false;
	unsigned int _iVolume = 60;
	unsigned int _iLockMode = 0;
	wstring _strRecordFilePath = L"";

	unsigned int _camIndex = 0;
	wstring _strCam = L"";

	unsigned int _iResolutionIndex = 3; //��ʼ������С

	unsigned int _nAntiNoiseVal = 0;

	static std::mutex singleton_p_mutex;

	UserInfo();
	//�Ѹ��ƹ��캯����=������Ҳ��Ϊ˽��,��ֹ������
	UserInfo(const UserInfo&);

	UserInfo& operator=(const UserInfo&);

	void UserInfo::InitDefault();

	std::vector<wstring> mResolList;  // ��������ֱ����б�

	std::vector<wstring> m_CapResolutionList;

};
