#include "stdafx.h"
#include "UserInfo.h"

#include "Utils.h"

// 默认初始化 单实例对象
UserInfo* UserInfo::p = nullptr;

std::mutex UserInfo::singleton_p_mutex;

UserInfo::~UserInfo() {}

UserInfo::UserInfo() {
	InitDefault();
}

UserInfo* UserInfo::GetInstance() {
	if (p == nullptr) {
		singleton_p_mutex.lock();
		if (p == nullptr) {
			p = new UserInfo();
		}
		singleton_p_mutex.unlock();
	}
	return p;
}


UserInfo & UserInfo::operator=(const UserInfo &) {
	return *GetInstance();
}

void UserInfo::InitDefault() {
	mResolList.push_back(L"320x240");
	mResolList.push_back(L"480x270");
	mResolList.push_back(L"480x360");
	mResolList.push_back(L"640x360");
	mResolList.push_back(L"640x480");
	mResolList.push_back(L"720x480");
	mResolList.push_back(L"720x540");
	mResolList.push_back(L"960x540");
	mResolList.push_back(L"960x720");
	mResolList.push_back(L"1280x720");
	mResolList.push_back(L"1280x960");
	mResolList.push_back(L"1920x1080");

	int maxWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int maxheight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	m_CapResolutionList.push_back(L"320x240");
	m_CapResolutionList.push_back(L"480x360");
	m_CapResolutionList.push_back(L"640x360");
	m_CapResolutionList.push_back(L"640x480");
	m_CapResolutionList.push_back(L"960x540");
	m_CapResolutionList.push_back(L"960x720");
	if (maxWidth <= 1280 || maxheight <= 720) {
		m_CapResolutionList.push_back(L"全屏");
		return;
	}
	m_CapResolutionList.push_back(L"1280x720");
	if (maxWidth <= 1920 || maxheight <= 1920) {
		m_CapResolutionList.push_back(L"全屏");
		return;
	}

	m_CapResolutionList.push_back(L"1920x1920");
	m_CapResolutionList.push_back(L"全屏");
}

wstring UserInfo::LoadUserInfo(wstring file) {
	if (Util::IsDirExist(file)) {
		return L"user IsDirExist，file：" + file;
	}
	if (!Util::IsFileExist(file)) {
		return L"user not IsFileExist，file：" + file;
	}
	if (!Util::IsFileReadAble(file)) {
		return L"user not IsFileReadAble，file：" + file;
	}
	wstring json_str = Util::WStringReadFile(file);

	return LoadByStr(json_str);
}


wstring UserInfo::DumpUserInfo(wstring file) {
	wstring json_str = DumpToStr();
	if (json_str.empty()) {
		return L"user skip Flag:" + Util::int2wstr(Flag);
	}

	if (Util::IsDirExist(file)) {
		return L"user IsDirExist，file：" + file;
	}
	if (Util::IsFileExist(file) && !Util::IsFileWriteAble(file)) {
		return L"user not IsFileWriteAble，file：" + file;
	}

	return Util::WStringWriteFile(file, json_str);
}

void UserInfo::SetOutResoluton(wstring outResoluton) {
	_outResoluton = outResoluton;
}

wstring UserInfo::GetOutResolution() {
	return _outResoluton;
}

void UserInfo::SetMic(wstring audioDevice) {
	_audioDevice = audioDevice;
}

wstring UserInfo::GetMic() {
	return _audioDevice;
}

void UserInfo::SetMicIndex(int audioIndex) {
	_audioIndex = audioIndex;
}

unsigned int UserInfo::GetMicIndex() {
	return _audioIndex;
}

void UserInfo::EnableAudioMix(bool audioMix) {
	_audioMix = audioMix;
}

void UserInfo::EnableAntiNoise(bool antiNoise) {
	_antiNoise = antiNoise;
}

void UserInfo::EnableCapWndLocked(bool bLockCapWnd) {
	_bLockCapWnd = bLockCapWnd;
}

bool UserInfo::IsAudioMix() {
	return _audioMix;
}

bool UserInfo::IsAntiNoise() {
	return _antiNoise;
}

bool UserInfo::IsCapWndLocked() {
	return _bLockCapWnd;
}

void UserInfo::SetVolume(int iVolume) {
	_iVolume = iVolume;
}

unsigned int UserInfo::GetVolume() {
	return _iVolume;
}

wstring UserInfo::LoadByStr(wstring str) {
	try {
		json::value obj = json::value::parse(str);
		return LoadByJSON(obj);
	} catch (const  json::json_exception& err) {
		return L"json parse error:" + Util::s2ws(err.what());
	}
}

void UserInfo::LoadByApiWithCallback(string url, wstring user, wstring pwd, std::function<void(int, wstring, UserInfo*)> callback) {
	string _user = Util::ws2s(user);
	string _pwd = Util::ws2s(pwd);
	string _pwd_md5 = Util::md5(_pwd);

	map<string, string> m;
	m["id"] = _user;
	m["pwd"] = _pwd_md5;
	string query_url = Util::BuildGet(url, m);

	pplx::task<http_response> res = Util::curlRpcAsync(Util::s2ws(query_url), L"", L"GET", L"");

	auto task2 = res.then([user, pwd, callback, this](http_response response) {

		auto response_code = response.status_code();
		if (response_code == status_codes::OK) {
			json::value& json_value = response.extract_json().get();
			json_value[L"user"] = json::value::string(user);
			json_value[L"pwd"] = json::value::string(pwd);
			
			int flag = 100;
			wstring ret = this->LoadByJSON(json_value);
			callback(flag, ret, this);
			return;
		}
	});
/*
	try
	{
		task2.wait();
	}
	catch (std::exception& e)
	{
		wstring response_error = L"";

		response_error = Util::s2ws(e.what());

		json::value error_info;
		error_info[L"response_error"] = json::value::string(response_error);

		json::value error_value;
		error_value[L"Flag"] = json::value::number(500);
		error_value[L"FlagString"] = json::value::string(L"http错误");
		error_value[L"Info"] = json::value::value(error_info);

		error_value[L"user"] = json::value::string(user);
		error_value[L"pwd"] = json::value::string(pwd);

		int flag = 500;
		wstring ret = this->LoadByJSON(error_value);
		callback(flag, ret, this);
		return;
	}*/
}

wstring UserInfo::LoadByApi(string url, wstring user, wstring pwd) {
	string _user = Util::ws2s(user);
	string _pwd = Util::ws2s(pwd);
	string _pwd_md5 = Util::md5(_pwd);

	map<string, string> m;
	m["id"] = _user;
	m["pwd"] = _pwd_md5;
	string query_url = Util::BuildGet(url, m);

	json::value obj = Util::curlRpc(Util::s2ws(query_url), L"", L"GET", L"");

	obj[L"user"] = json::value::string(user);
	obj[L"pwd"] = json::value::string(pwd);

	return LoadByJSON(obj);
}

void UserInfo::SetAntiNoiseValue(int nAntiNoiseVal) {
	_nAntiNoiseVal = nAntiNoiseVal;
}

unsigned int UserInfo::GetAntiNoiseValue() {
	return _nAntiNoiseVal;
}

void UserInfo::SetLockMode(int lockMode) {
	_iLockMode = lockMode;
}

unsigned int UserInfo::GetLockMode() {
	return _iLockMode;
}

void UserInfo::SetRecordFilePath(wstring strRecordFilePath) {
	_strRecordFilePath = strRecordFilePath;
}

wstring UserInfo::GetRecordFilePath() {
	return _strRecordFilePath;
}

void UserInfo::SetCamIndex(int camIndex) {
	_camIndex = camIndex;
}

unsigned int UserInfo::GetCamIndex() {
	return _camIndex;
}

void UserInfo::SetCam(wstring czDevice) {
	_strCam = czDevice;
}

wstring UserInfo::GetCam() {
	return _strCam;
}

void UserInfo::SetResolutionIndex(int iResolutionIndex) {
	_iResolutionIndex = iResolutionIndex;
}

unsigned int UserInfo::GetResolutionIndex() {
	return _iResolutionIndex;
}

std::vector<wstring>& UserInfo::GetReslList() {
	return mResolList;
}

std::vector<wstring>& UserInfo::GetCapResolutionList() {
	return m_CapResolutionList;
}

void UserInfo::ResetPushConfig() {
	MaxOutResolution = L"1280*720";
	ADSampleRate = 44100;
	ADChannels = 2;
	ADBitperSample = 16;
	ADBitRate = 32;

	VDIntervalSecond = 1;
	BFrameCount = 0;

	VD_method = L"cbr";
	VDFPS = 7;
	abrDqmin = 0;
	VDBitRate = 300;
	VBRQuality = 30;

	VC_method = L"cbr";
	VCFPS = 25;
	abrCqmin = 30;
	VCBitRate = 300;
	CQuality = 30;

	VF_method = L"cbr";
	VFFPS = 15;
	abrFqmin = 30;
	VFBitRate = 300;
	VBRFQuality = 30;
}

void UserInfo::SetabrDqmin(int _abrDqmin) {
	abrDqmin = _abrDqmin;
}

void UserInfo::SetVDWidth(int vDWidth) {
	_vDWidth = vDWidth;
}

unsigned int UserInfo::GetVDWidth() {
	return _vDWidth;
}

void UserInfo::SetVDHeight(int vDHeight) {
	_vDHeight = vDHeight;
}

unsigned int UserInfo::GetVDHeight() {
	return _vDHeight;
}

map<string, string> UserInfo::VauleAsMap() {
	map<string, string> m;

	m["user"] = Util::ws2s(user);
	m["pwd"] = Util::ws2s(pwd);

	// m["dsc"] = Util::ws2s(dsc);
	m["play_rtmp_url"] = Util::ws2s(play_rtmp_url);
	m["play_hls_url"] = Util::ws2s(play_hls_url);
	m["PubUrlPub"] = Util::ws2s(PubUrlPub);
	m["PublishRoomInfo"] = Util::ws2s(PublishRoomInfo);
	m["stream"] = Util::ws2s(stream);
	m["PubWay"] = Util::ws2s(PubWay);
	m["StrKey"] = Util::ws2s(StrKey);
	m["Domain"] = Util::ws2s(Domain);
	m["strUserID"] = Util::ws2s(strUserID);
	m["Uin"] = Util::ws2s(Uin);
	m["ChatUrl"] = Util::ws2s(ChatUrl);

	m["MaxOutResolution"] = Util::ws2s(MaxOutResolution);
	m["ADSampleRate"] = Util::int2str(ADSampleRate);
	m["ADChannels"] = Util::int2str(ADChannels);
	m["ADBitperSample"] = Util::int2str(ADBitperSample);
	m["ADBitRate"] = Util::int2str(ADBitRate);
	m["VDIntervalSecond"] = Util::int2str(VDIntervalSecond);
	m["BFrameCount"] = Util::int2str(BFrameCount);

	m["VD_method"] = Util::ws2s(VD_method);
	m["VDFPS"] = Util::int2str(VDFPS);
	m["abrDqmin"] = Util::int2str(abrDqmin);
	m["VDBitRate"] = Util::int2str(ADBitperSample);
	m["VBRQuality"] = Util::int2str(VBRQuality);

	m["VC_method"] = Util::ws2s(VC_method);
	m["VCFPS"] = Util::int2str(VCFPS);
	m["abrCqmin"] = Util::int2str(abrCqmin);
	m["VCBitRate"] = Util::int2str(VCBitRate);
	m["CQuality"] = Util::int2str(CQuality);

	m["VF_method"] = Util::ws2s(VF_method);
	m["VFFPS"] = Util::int2str(VFFPS);
	m["abrFqmin"] = Util::int2str(abrFqmin);
	m["VFBitRate"] = Util::int2str(VFBitRate);
	m["VBRFQuality"] = Util::int2str(VBRFQuality);

	m["Flag"] = Util::int2str(Flag);

	return m;
}


wstring UserInfo::LoadByJSON(json::value obj) {
	if (!obj.has_integer_field(L"Flag") || obj.at(L"Flag").as_integer() != 100) {
		if (obj.has_string_field(L"FlagString")) {
			wstring _FlagString = obj.at(L"FlagString").as_string();
			if (!_FlagString.empty()) {
				return _FlagString;
			}
		}
		return L"登录失败！代码：" + Util::int2wstr(obj.has_integer_field(L"Flag") ? obj.at(L"Flag").as_integer() : 500);
	}

	try {
		user = obj.has_string_field(L"user") ? obj.at(L"user").as_string() : user;
		pwd = obj.has_string_field(L"pwd") ? obj.at(L"pwd").as_string() : pwd;

		dsc = obj.has_string_field(L"dsc") ? obj.at(L"dsc").as_string() : dsc;
		play_rtmp_url = obj.has_string_field(L"play_rtmp_url") ? obj.at(L"play_rtmp_url").as_string() : play_rtmp_url;
		play_hls_url = obj.has_string_field(L"play_hls_url") ? obj.at(L"play_hls_url").as_string() : play_hls_url;
		PubUrlPub = obj.has_string_field(L"PubUrlPub") ? obj.at(L"PubUrlPub").as_string() : PubUrlPub;
		PublishRoomInfo = obj.has_string_field(L"PublishRoomInfo") ? obj.at(L"PublishRoomInfo").as_string() : PublishRoomInfo;
		stream = obj.has_string_field(L"stream") ? obj.at(L"stream").as_string() : stream;

		PubWay = obj.has_string_field(L"PubWay") ? obj.at(L"PubWay").as_string() : PubWay;
		StrKey = obj.has_string_field(L"StrKey") ? obj.at(L"StrKey").as_string() : StrKey;
		Domain = obj.has_string_field(L"Domain") ? obj.at(L"Domain").as_string() : Domain;
		strUserID = obj.has_string_field(L"strUserID") ? obj.at(L"strUserID").as_string() : strUserID;
		Uin = obj.has_string_field(L"Uin") ? obj.at(L"Uin").as_string() : Uin;
		ChatUrl = obj.has_string_field(L"ChatUrl") ? obj.at(L"ChatUrl").as_string() : ChatUrl;

		MaxOutResolution = obj.has_string_field(L"MaxOutResolution") ? obj.at(L"MaxOutResolution").as_string() : MaxOutResolution;
		wstring _ADSampleRate = obj.has_string_field(L"ADSampleRate") ? obj.at(L"ADSampleRate").as_string() : Util::int2wstr(ADSampleRate);
		ADSampleRate = Util::ws2int(_ADSampleRate);
		wstring _ADChannels = obj.has_string_field(L"ADChannels") ? obj.at(L"ADChannels").as_string() : Util::int2wstr(ADChannels);
		ADChannels = Util::ws2int(_ADChannels);
		wstring _ADBitperSample = obj.has_string_field(L"ADBitperSample") ? obj.at(L"ADBitperSample").as_string() : Util::int2wstr(ADBitperSample);
		ADBitperSample = Util::ws2int(_ADBitperSample);
		wstring _ADBitRate = obj.has_string_field(L"ADBitRate") ? obj.at(L"ADBitRate").as_string() : Util::int2wstr(ADBitRate);
		ADBitRate = Util::ws2int(_ADBitRate);
		wstring _VDIntervalSecond = obj.has_string_field(L"VDIntervalSecond") ? obj.at(L"VDIntervalSecond").as_string() : Util::int2wstr(VDIntervalSecond);
		VDIntervalSecond = Util::ws2int(_VDIntervalSecond);
		wstring _BFrameCount = obj.has_string_field(L"BFrameCount") ? obj.at(L"BFrameCount").as_string() : Util::int2wstr(BFrameCount);
		BFrameCount = Util::ws2int(_BFrameCount);

		VD_method = obj.has_string_field(L"VD_method") ? obj.at(L"VD_method").as_string() : VD_method;
		VD_method = L"cbr" == VD_method ? L"cbr" : L"vbr";
		wstring _VDFPS = obj.has_string_field(L"VDFPS") ? obj.at(L"VDFPS").as_string() : Util::int2wstr(VDFPS);
		VDFPS = Util::ws2int(_VDFPS);
		wstring _abrDqmin = obj.has_string_field(L"abrDqmin") ? obj.at(L"abrDqmin").as_string() : Util::int2wstr(abrDqmin);
		abrDqmin = Util::ws2int(_abrDqmin);
		wstring _VDBitRate = obj.has_string_field(L"VDBitRate") ? obj.at(L"VDBitRate").as_string() : Util::int2wstr(VDBitRate);
		VDBitRate = Util::ws2int(_VDBitRate);
		wstring _VBRQuality = obj.has_string_field(L"VBRQuality") ? obj.at(L"VBRQuality").as_string() : Util::int2wstr(VBRQuality);
		VBRQuality = Util::ws2int(_VBRQuality);

		VC_method = obj.has_string_field(L"VC_method") ? obj.at(L"VC_method").as_string() : VC_method;
		VC_method = L"cbr" == VC_method ? L"cbr" : L"vbr";
		wstring _VCFPS = obj.has_string_field(L"VCFPS") ? obj.at(L"VCFPS").as_string() : Util::int2wstr(VCFPS);
		VCFPS = Util::ws2int(_VCFPS);
		wstring _abrCqmin = obj.has_string_field(L"abrCqmin") ? obj.at(L"abrCqmin").as_string() : Util::int2wstr(abrCqmin);
		abrCqmin = Util::ws2int(_abrCqmin);
		wstring _VCBitRate = obj.has_string_field(L"VCBitRate") ? obj.at(L"VCBitRate").as_string() : Util::int2wstr(VCBitRate);
		VCBitRate = Util::ws2int(_VCBitRate);
		wstring _CQuality = obj.has_string_field(L"CQuality") ? obj.at(L"CQuality").as_string() : Util::int2wstr(CQuality);
		CQuality = Util::ws2int(_CQuality);

		VF_method = obj.has_string_field(L"VF_method") ? obj.at(L"VF_method").as_string() : VF_method;
		VF_method = L"cbr" == VF_method ? L"cbr" : L"vbr";
		wstring _VFFPS = obj.has_string_field(L"VFFPS") ? obj.at(L"VFFPS").as_string() : Util::int2wstr(VFFPS);
		VFFPS = Util::ws2int(_VFFPS);
		wstring _abrFqmin = obj.has_string_field(L"abrFqmin") ? obj.at(L"abrFqmin").as_string() : Util::int2wstr(abrFqmin);
		abrFqmin = Util::ws2int(_abrFqmin);
		wstring _VFBitRate = obj.has_string_field(L"VFBitRate") ? obj.at(L"VFBitRate").as_string() : Util::int2wstr(VFBitRate);
		VFBitRate = Util::ws2int(_VFBitRate);
		wstring _VBRFQuality = obj.has_string_field(L"VBRFQuality") ? obj.at(L"VBRFQuality").as_string() : Util::int2wstr(VBRFQuality);
		VBRFQuality = Util::ws2int(_VBRFQuality);

		Flag = obj.has_integer_field(L"Flag") ? obj.at(L"Flag").as_integer() : Flag;

		/********************    临时状态 保存与读取    ********************/

		_vDWidth = obj.has_integer_field(L"_vDWidth") ? obj.at(L"_vDWidth").as_integer() : _vDWidth;
		_vDHeight = obj.has_integer_field(L"_vDHeight") ? obj.at(L"_vDHeight").as_integer() : _vDHeight;
		_outResoluton = obj.has_string_field(L"_outResoluton") ? obj.at(L"_outResoluton").as_string() : _outResoluton;
		_audioDevice = obj.has_string_field(L"_audioDevice") ? obj.at(L"_audioDevice").as_string() : _audioDevice;
		_audioIndex = obj.has_integer_field(L"_audioIndex") ? obj.at(L"_audioIndex").as_integer() : _audioIndex;
		_bLockCapWnd = obj.has_boolean_field(L"_bLockCapWnd") ? obj.at(L"_bLockCapWnd").as_bool() : _bLockCapWnd;
		_audioMix = obj.has_boolean_field(L"_audioMix") ? obj.at(L"_audioMix").as_bool() : _audioMix;
		_antiNoise = obj.has_boolean_field(L"_antiNoise") ? obj.at(L"_antiNoise").as_bool() : _antiNoise;
		_iVolume = obj.has_integer_field(L"_iVolume") ? obj.at(L"_iVolume").as_integer() : _iVolume;
		_iLockMode = obj.has_integer_field(L"_iLockMode") ? obj.at(L"_iLockMode").as_integer() : _iLockMode;
		_strRecordFilePath = obj.has_string_field(L"_strRecordFilePath") ? obj.at(L"_strRecordFilePath").as_string() : _strRecordFilePath;
		_camIndex = obj.has_integer_field(L"_camIndex") ? obj.at(L"_camIndex").as_integer() : _camIndex;
		_strCam = obj.has_string_field(L"_strCam") ? obj.at(L"_strCam").as_string() : _strCam;
		_iResolutionIndex = obj.has_integer_field(L"_iResolutionIndex") ? obj.at(L"_iResolutionIndex").as_integer() : _iResolutionIndex;
		_nAntiNoiseVal = obj.has_integer_field(L"_nAntiNoiseVal") ? obj.at(L"_nAntiNoiseVal").as_integer() : _nAntiNoiseVal;

		return L"";
	} catch (const  json::json_exception& err) {
		return L"json value error:" + Util::s2ws(err.what());
	}
	return L"json value error empty";
}

std::pair<int, int> UserInfo::GetMaxOutResolution(int dw, int dh) {
	dw = dw >= 0 && dw <= 1920 ? dw : 1920;
	dh = dh >= 0 && dh <= 1080 ? dh : 1080;
	int pos = MaxOutResolution.find_first_of(L"*");
	if (pos > 0) {
		return std::make_pair(Util::ws2int(MaxOutResolution.substr(0, pos)), Util::ws2int(MaxOutResolution.substr(pos + 1)));
	}
	return std::make_pair(dw, dh);
}

wstring UserInfo::RefreshStr() {
	if (Flag != 100) {
		return L"";
	}
	
	std::vector<std::string> arr = Util::split(Util::ws2s(stream), "?");
	
	json::value obj;


	obj[L"PubUrlPub"] = json::value::string(PubUrlPub);
	obj[L"PublishRoomInfo"] = json::value::string(PublishRoomInfo);
	
	obj[L"stream"] = json::value::string(Util::s2ws(arr[0]));
	obj[L"PubWay"] = json::value::string(PubWay);
	obj[L"StrKey"] = json::value::string(StrKey);
	obj[L"Domain"] = json::value::string(Domain);
	obj[L"strUserID"] = json::value::string(strUserID);
	obj[L"Uin"] = json::value::string(Uin);
	obj[L"ChatUrl"] = json::value::string(ChatUrl);

	obj[L"MaxOutResolution"] = json::value::string(MaxOutResolution);
	obj[L"ADSampleRate"] = json::value::string(Util::int2wstr(ADSampleRate));
	obj[L"ADChannels"] = json::value::string(Util::int2wstr(ADChannels));
	obj[L"ADBitperSample"] = json::value::string(Util::int2wstr(ADBitperSample));
	obj[L"ADBitRate"] = json::value::string(Util::int2wstr(ADBitRate));
	obj[L"VDIntervalSecond"] = json::value::string(Util::int2wstr(VDIntervalSecond));
	obj[L"BFrameCount"] = json::value::string(Util::int2wstr(BFrameCount));

	obj[L"VD_method"] = json::value::string(VD_method);
	obj[L"VDFPS"] = json::value::string(Util::int2wstr(VDFPS));
	obj[L"abrDqmin"] = json::value::string(Util::int2wstr(abrDqmin));
	obj[L"VDBitRate"] = json::value::string(Util::int2wstr(VDBitRate));
	obj[L"VBRQuality"] = json::value::string(Util::int2wstr(VBRQuality));

	obj[L"VC_method"] = json::value::string(VC_method);
	obj[L"VCFPS"] = json::value::string(Util::int2wstr(VCFPS));
	obj[L"abrCqmin"] = json::value::string(Util::int2wstr(abrCqmin));
	obj[L"VCBitRate"] = json::value::string(Util::int2wstr(VCBitRate));
	obj[L"CQuality"] = json::value::string(Util::int2wstr(CQuality));

	obj[L"VF_method"] = json::value::string(VF_method);
	obj[L"VFFPS"] = json::value::string(Util::int2wstr(VFFPS));
	obj[L"abrFqmin"] = json::value::string(Util::int2wstr(abrFqmin));
	obj[L"VFBitRate"] = json::value::string(Util::int2wstr(VFBitRate));
	obj[L"VBRFQuality"] = json::value::string(Util::int2wstr(VBRFQuality));

	return obj.serialize();
}

wstring UserInfo::DumpToStr() {
	if (Flag != 100) {
		return L"";
	}

	json::value obj;
	obj[L"user"] = json::value::string(user);
	obj[L"pwd"] = json::value::string(pwd);

	obj[L"dsc"] = json::value::string(dsc);
	obj[L"play_rtmp_url"] = json::value::string(play_rtmp_url);
	obj[L"play_hls_url"] = json::value::string(play_hls_url);
	obj[L"PubUrlPub"] = json::value::string(PubUrlPub);
	obj[L"PublishRoomInfo"] = json::value::string(PublishRoomInfo);
	obj[L"stream"] = json::value::string(stream);
	obj[L"PubWay"] = json::value::string(PubWay);
	obj[L"StrKey"] = json::value::string(StrKey);
	obj[L"Domain"] = json::value::string(Domain);
	obj[L"strUserID"] = json::value::string(strUserID);
	obj[L"Uin"] = json::value::string(Uin);
	obj[L"ChatUrl"] = json::value::string(ChatUrl);

	obj[L"MaxOutResolution"] = json::value::string(MaxOutResolution);
	obj[L"ADSampleRate"] = json::value::string(Util::int2wstr(ADSampleRate));
	obj[L"ADChannels"] = json::value::string(Util::int2wstr(ADChannels));
	obj[L"ADBitperSample"] = json::value::string(Util::int2wstr(ADBitperSample));
	obj[L"ADBitRate"] = json::value::string(Util::int2wstr(ADBitRate));
	obj[L"VDIntervalSecond"] = json::value::string(Util::int2wstr(VDIntervalSecond));
	obj[L"BFrameCount"] = json::value::string(Util::int2wstr(BFrameCount));

	obj[L"VD_method"] = json::value::string(VD_method);
	obj[L"VDFPS"] = json::value::string(Util::int2wstr(VDFPS));
	obj[L"abrDqmin"] = json::value::string(Util::int2wstr(abrDqmin));
	obj[L"VDBitRate"] = json::value::string(Util::int2wstr(VDBitRate));
	obj[L"VBRQuality"] = json::value::string(Util::int2wstr(VBRQuality));

	obj[L"VC_method"] = json::value::string(VC_method);
	obj[L"VCFPS"] = json::value::string(Util::int2wstr(VCFPS));
	obj[L"abrCqmin"] = json::value::string(Util::int2wstr(abrCqmin));
	obj[L"VCBitRate"] = json::value::string(Util::int2wstr(VCBitRate));
	obj[L"CQuality"] = json::value::string(Util::int2wstr(CQuality));

	obj[L"VF_method"] = json::value::string(VF_method);
	obj[L"VFFPS"] = json::value::string(Util::int2wstr(VFFPS));
	obj[L"abrFqmin"] = json::value::string(Util::int2wstr(abrFqmin));
	obj[L"VFBitRate"] = json::value::string(Util::int2wstr(VFBitRate));
	obj[L"VBRFQuality"] = json::value::string(Util::int2wstr(VBRFQuality));

	obj[L"Flag"] = json::value::number(Flag);

	/********************    临时状态 保存与读取    ********************/
	obj[L"_vDWidth"] = json::value::number(_vDWidth);
	obj[L"_vDHeight"] = json::value::number(_vDHeight);
	obj[L"_outResoluton"] = json::value::string(_outResoluton);
	obj[L"_audioDevice"] = json::value::string(_audioDevice);
	obj[L"_audioIndex"] = json::value::number(_audioIndex);
	obj[L"_bLockCapWnd"] = json::value::boolean(_bLockCapWnd);
	obj[L"_audioMix"] = json::value::boolean(_audioMix);
	obj[L"_antiNoise"] = json::value::boolean(_antiNoise);

	obj[L"_iVolume"] = json::value::number(_iVolume);
	obj[L"_iLockMode"] = json::value::number(_iLockMode);
	obj[L"_strRecordFilePath"] = json::value::string(_strRecordFilePath);

	obj[L"_camIndex"] = json::value::number(_camIndex);
	obj[L"_strCam"] = json::value::string(_strCam);

	obj[L"_iResolutionIndex"] = json::value::number(_iResolutionIndex);
	obj[L"_nAntiNoiseVal"] = json::value::number(_nAntiNoiseVal);

	return obj.serialize();
}
